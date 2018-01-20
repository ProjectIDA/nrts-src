#pragma ident "$Id: unpack.c,v 1.45 2018/01/18 23:31:09 dechavez Exp $"
/*======================================================================
 *
 *  Unpack various objects into host byte order.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

/* Figure out the TS datatype enum */

static int datatype(int subformat, UINT8 descriptor)
{
    switch (subformat) {

      case IDA10_SUBFORMAT_3:
        switch (descriptor & IDA10_MASK_TYPE) {
          case IDA10_MASK_INT16:  return IDA10_DATA_INT16;
          case IDA10_MASK_INT32:  return IDA10_DATA_INT32;
          case IDA10_MASK_REAL32: return IDA10_DATA_REAL32;
          case IDA10_MASK_REAL64: return IDA10_DATA_REAL64;
          default: return IDA10_DATA_UNKNOWN;
        }
        break;

      case IDA10_SUBFORMAT_12:
        return IDA10_DATA_MSEED512;

      default:
        switch (descriptor & IDA10_MASK_TYPE) {
          case IDA10_MASK_INT8:   return IDA10_DATA_INT8;
          case IDA10_MASK_INT16:  return IDA10_DATA_INT16;
          case IDA10_MASK_INT32:  return IDA10_DATA_INT32;
          case IDA10_MASK_REAL32: return IDA10_DATA_REAL32;
          default: return IDA10_DATA_UNKNOWN;
        }
    }

    return IDA10_DATA_UNKNOWN;
}

/* determine packet type */

int ida10Type(UINT8 *raw)
{
int rev;

/* Special allowance for ISP injected logs (I~I) */

    if (raw[0] == 0x49 && raw[1] == 0xb6) return IDA10_TYPE_ISPLOG;

/* Special allowance for IDA9 data packets */

#ifdef INCLUDE_IDA9_SUPPORT
    if ((rev = idaPacketFormat(raw)) == 9) return IDA10_TYPE_TS;
#endif /* INCLUDE_IDA9_SUPPORT */

/* "true" ida10 tests follow */

    if (ida10FormatCode(raw) != IDA10_FORMAT_CODE) return IDA10_TYPE_UNKNOWN;

    if (memcmp(raw, "TS", 2) == 0) return IDA10_TYPE_TS;
    if (memcmp(raw, "CA", 2) == 0) return IDA10_TYPE_CA;
    if (memcmp(raw, "CF", 2) == 0) return IDA10_TYPE_CF;
    if (memcmp(raw, "LM", 2) == 0) return IDA10_TYPE_LM;

    return IDA10_TYPE_UNKNOWN;
}

/* Unpack a SAN time tag */

static int UnpackSanClockStatus(UINT8 *start, IDA10_SAN_CLOCK_STATUS *status)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &status->raw);

    status->receiver = (status->raw >> 9) & 0x3f;
    status->type = SANIO_TT_STAT_EXTTIME_TYPE(status->raw);
    status->init = status->raw & SANIO_TT_STAT_EXTTIME_NOINIT ? FALSE : TRUE;
    status->pps = status->raw & SANIO_TT_STAT_EXTPPS_AVAIL ? TRUE : FALSE;
    status->pll = status->raw & SANIO_TT_STAT_PLL_ENABLE ? TRUE : FALSE;
    status->avail = status->raw & SANIO_TT_STAT_EXTTIME_AVAIL ? TRUE : FALSE;
    status->rtt_fail = status->raw & SANIO_TT_STAT_RTT_FAILURE ? TRUE : FALSE;
    status->suspect = status->raw & SANIO_TT_STAT_SUSPICIOUS_GPS ? TRUE : FALSE;
    status->locked = status->raw & SANIO_TT_STAT_EXTTIME_QUALITY ? TRUE : FALSE;

    return (int) (ptr - start);
}


static int UnpackSanTtag(UINT8 *start, IDA10_SANTTAG *san)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &san->ext);
    ptr += utilUnpackUINT32(ptr, &san->sys.sec);
    ptr += utilUnpackUINT16(ptr, &san->sys.msc);
    ptr += UnpackSanClockStatus(ptr, &san->status);
    ptr += utilUnpackUINT16(ptr, &san->pll);
    ptr += utilUnpackUINT16(ptr, &san->phase);
    ptr += utilUnpackUINT32(ptr, &san->epoch.sec);
    ptr += utilUnpackUINT16(ptr, &san->epoch.msc);

    return (int) (ptr - start);
}

/* Unpack an OFIS time tag */

static int UnpackOfisClockStatus(UINT8 *start, IDA10_OFIS_CLOCK_STATUS *status)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &status->raw);

    status->receiver = (status->raw >> 4) & 0xf;
    status->init = status->raw & OFIS_TT_STAT_EXTTIME_NOINIT ? FALSE : TRUE;
    status->pps = status->raw & OFIS_TT_STAT_EXTPPS_AVAIL ? TRUE : FALSE;
    status->avail = status->raw & OFIS_TT_STAT_EXTTIME_AVAIL ? TRUE : FALSE;
    status->suspect = status->raw & OFIS_TT_STAT_SUSPICIOUS_GPS ? TRUE : FALSE;
    status->locked = status->raw & OFIS_TT_STAT_EXTTIME_QUALITY ? TRUE : FALSE;
    status->leapsec = status->raw & OFIS_TT_STAT_LEAPSEC ? TRUE : FALSE;

    return (int) (ptr - start);
}

static int UnpackOfisTtag(UINT8 *start, IDA10_OFISTAG *ofis)
{
UINT8 *ptr;
UINT32 value;
char tmp[32];

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &ofis->ext);
    ptr += utilUnpackUINT64(ptr, &ofis->sys);
    ptr += utilUnpackUINT64(ptr, &ofis->pps);
    ptr += UnpackOfisClockStatus(ptr, &ofis->status);

    return (int) (ptr - start);
}

/* Unpack a Q330 time tag */

static int UnpackQ330Ttag(UINT8 *start, IDA10_Q330TAG *q330)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &q330->seqno);
    ptr += utilUnpackINT32(ptr, &q330->offset.sec);
    ptr += utilUnpackINT32(ptr, &q330->offset.usec);
    ptr += utilUnpackINT32(ptr, &q330->offset.index);
    ptr += utilUnpackINT32(ptr, &q330->delay);
    ptr += utilUnpackUINT16(ptr, &q330->loss);
    q330->qual.bitmap = *ptr++;
    q330->qual.percent = *ptr++;

    return (int) (ptr - start);
}

/* Unpack a generic time tag */

static int UnpackGenericTtag(UINT8 *start, IDA10_GENTAG *gen)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &gen->tstamp);
    gen->status.receiver = *ptr++;
    gen->status.generic = *ptr++;

    return (int) (ptr - start);
}

int ida10PackGenericTtag(UINT8 *start, IDA10_GENTAG *gen)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT64(ptr, gen->tstamp);
    *ptr++ = gen->status.receiver;
    *ptr++ = gen->status.generic;

    return (int) (ptr - start);
}

/* Unpack an OBS time tag */

static int UnpackSBDHDR(UINT8 *start, IDA10_SBDHDR *sbd)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &sbd->imei);
    ptr += utilUnpackUINT16(ptr, &sbd->momsn);
    ptr += utilUnpackUINT32(ptr, &sbd->tstamp);

    return (int) (ptr - start);
}

static int UnpackOBSTtag(UINT8 *start, IDA10_OBSTAG *obs)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &obs->sys);
    ptr += utilUnpackUINT64(ptr, &obs->ref);
    ptr += utilUnpackUINT32(ptr, &obs->ticrate);
    ptr += UnpackSBDHDR(ptr, &obs->sbd);

    return (int) (ptr - start);
}

static int UnpackOBSTtag2(UINT8 *start, IDA10_OBSTAG2 *obs2)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &obs2->sys);
    ptr += utilUnpackUINT32(ptr, &obs2->ref);
    ptr += utilUnpackINT32(ptr, &obs2->delay);
    ptr += utilUnpackUINT32(ptr, &obs2->ticrate);
    ptr += UnpackSBDHDR(ptr, &obs2->sbd);

    return (int) (ptr - start);
}

/* Unpack a MiniSeed time tag */

static int UnpackSEEDtag(UINT8 *start, IDA10_SEEDTAG *seed)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &seed->tstamp);
    seed->status.percent = *ptr++;
    seed->status.bitmap = *ptr++;

    return (int) (ptr - start);
}

/* Unpack the 10.0 common header */

static int UnpackCmnHdr0(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT8 *ptr;

    cmnhdr->subformat = IDA10_SUBFORMAT_0;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = cmnhdr->ttag.beg.type = IDA10_TIMER_SAN;
    cmnhdr->ttag.beg.derived = cmnhdr->ttag.end.derived = FALSE;
    ptr += UnpackSanTtag(ptr, &cmnhdr->ttag.beg.san);
    ptr += UnpackSanTtag(ptr, &cmnhdr->ttag.end.san);

    cmnhdr->nbytes = IDA10_FIXEDRECLEN - IDA10_CMNHEADLEN; /* fixed for 10.0 */

    return (int) (ptr - start);
}

/* Unpack the 10.1 common header */

static int UnpackCmnHdr1(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;

    cmnhdr->subformat = IDA10_SUBFORMAT_1;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_SAN;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackSanTtag(ptr, &cmnhdr->ttag.beg.san);
    ptr += IDA101_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.1 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.2 common header */

static int UnpackCmnHdr2(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
char tmp[1024];
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_2;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_SAN;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackSanTtag(ptr, &cmnhdr->ttag.beg.san);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    cmnhdr->extra.valid = TRUE;
    ptr += IDA102_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.2 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

/* Some early 10.2 CF records have a bogus nbytes field */

    if (cmnhdr->type == IDA10_TYPE_CF && cmnhdr->nbytes == 0x2e0c) cmnhdr->nbytes = 974;

    return (int) (ptr - start);
}

/* Unpack the 10.3 common header */

static int UnpackCmnHdr3(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
char tmp[1024], *tmpstr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_3;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_OFIS;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackOfisTtag(ptr, &cmnhdr->ttag.beg.ofis);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    if (cmnhdr->extra.seqno != 0 || cmnhdr->extra.tstamp != 0) {
        cmnhdr->extra.valid = TRUE;
    } else {
        cmnhdr->extra.valid = FALSE;
    }
    ptr += IDA103_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.3 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.4 common header */

static int UnpackCmnHdr4(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
char tmp[1024], *tmpstr;
IDA10_TTAG *ttag;
static UINT8 UndefinedSrc[4] = {0x31, 0x30, 0x2E, 0x34}; /* "10.4" */

    cmnhdr->subformat = IDA10_SUBFORMAT_4;

    ptr = &start[4];
    ptr += utilUnpackUINT64(ptr, &cmnhdr->serialno);
    cmnhdr->boxid = IDA10_64BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_Q330;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackQ330Ttag(ptr, &cmnhdr->ttag.beg.q330);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    if (memcmp(ptr, UndefinedSrc, 4) == 0) {
        ptr += 4; /* skip over original reserved */
    } else {
        ptr += 2; /* skip over new reserved */
        cmnhdr->extra.src[0] = *ptr++;
        cmnhdr->extra.src[1] = *ptr++;
    }
    cmnhdr->extra.valid = TRUE;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.4 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.5 common header */

static int UnpackCmnHdr5(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr, *sname, *nname;
char tmp[1024], *tmpstr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_5;

    ptr = &start[4];
    cmnhdr->boxid = IDA10_64BIT_BOXID;

/* The IDA10.5 "serial number" is the station name and network code */

    cmnhdr->serialno = 0;
    sname = (UINT8 *) &cmnhdr->serialno;
    nname = sname + IDA105_SNAME_LEN + 1;
    ptr += utilUnpackBytes(ptr, sname, IDA105_SNAME_LEN);
    ptr += utilUnpackBytes(ptr, nname, IDA105_NNAME_LEN);
    ptr += UnpackGenericTtag(ptr, &cmnhdr->ttag.beg.gen);
    cmnhdr->ttag.beg.type = IDA10_TIMER_GENERIC;
    ptr += IDA105_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;
    cmnhdr->extra.valid = FALSE;

/* 10.5 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.6 common header */

static int UnpackCmnHdr6(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
char tmp[1024];
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_6;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_SAN;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackSanTtag(ptr, &cmnhdr->ttag.beg.san);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.origseqno);
    cmnhdr->extra.valid = TRUE;
    ptr += IDA106_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.6 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.7 common header */

static int UnpackCmnHdr7(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
char tmp[1024], *tmpstr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_7;

    ptr = &start[4];
    ptr += utilUnpackUINT16(ptr, &cmnhdr->boxid);
    cmnhdr->serialno = IDA10_16BIT_BOXID;
    cmnhdr->ttag.beg.type = IDA10_TIMER_OFIS;
    cmnhdr->ttag.beg.derived = FALSE;
    ptr += UnpackOfisTtag(ptr, &cmnhdr->ttag.beg.ofis);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.origseqno);
    if (cmnhdr->extra.seqno != 0 || cmnhdr->extra.tstamp != 0) {
        cmnhdr->extra.valid = TRUE;
    } else {
        cmnhdr->extra.valid = FALSE;
    }
    ptr += IDA107_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.7 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.8 common header */

static int UnpackCmnHdr8(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr, *sname, *nname;
char tmp[1024], *tmpstr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_8;

    ptr = &start[4];
    cmnhdr->boxid = IDA10_64BIT_BOXID;

/* The IDA10.8 "serial number" is the station name and network code */

    cmnhdr->serialno = 0;
    sname = (UINT8 *) &cmnhdr->serialno;
    nname = sname + IDA108_SNAME_LEN + 1;
    ptr += utilUnpackBytes(ptr, sname, IDA108_SNAME_LEN);
    ptr += utilUnpackBytes(ptr, nname, IDA108_NNAME_LEN);
    ptr += UnpackGenericTtag(ptr, &cmnhdr->ttag.beg.gen);
    cmnhdr->ttag.beg.type = IDA10_TIMER_GENERIC;
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    cmnhdr->extra.valid = TRUE;
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.8 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.10 common header */

static int UnpackCmnHdr10(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_10;

    ptr = &start[4];
    ptr += UnpackOBSTtag(ptr, &cmnhdr->ttag.beg.obs);
    cmnhdr->ttag.beg.type = IDA10_TIMER_OBS;
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    cmnhdr->extra.valid = TRUE;
    ptr += IDA1010_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.10 serial number is the IMEI number from the SBD header */

    cmnhdr->boxid = IDA10_64BIT_BOXID;
    cmnhdr->serialno = cmnhdr->ttag.beg.obs.sbd.imei;

/* 10.10 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

/* Unpack the 10.11 common header */

static int UnpackCmnHdr11(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
IDA10_TTAG *ttag;

    cmnhdr->subformat = IDA10_SUBFORMAT_11;

    ptr = &start[4];
    ptr += UnpackOBSTtag2(ptr, &cmnhdr->ttag.beg.obs2);
    cmnhdr->ttag.beg.type = IDA10_TIMER_OBS2;
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    cmnhdr->extra.valid = TRUE;
    ptr += IDA1011_RESERVED_BYTES;
    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.11 serial number is the IMEI number from the SBD header */

    cmnhdr->boxid = IDA10_64BIT_BOXID;
    cmnhdr->serialno = cmnhdr->ttag.beg.obs.sbd.imei;

/* 10.11 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

static int UnpackCmnHdr12(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
UINT16 stmp;
UINT8 *ptr;
IDA10_TTAG *ttag;
char sname[IDA1012_SNAME_LEN+1];

    cmnhdr->subformat = IDA10_SUBFORMAT_12;

    ptr = &start[4];
    cmnhdr->boxid = IDA10_64BIT_BOXID;
    ptr += utilUnpackUINT64(ptr, &cmnhdr->serialno);
    if (cmnhdr->serialno == 0) {
        ptr += utilUnpackBytes(ptr, sname, IDA1012_SNAME_LEN); sname[IDA1012_SNAME_LEN] = 0;
        sprintf((char *)&cmnhdr->serialno, "%s", sname);
    } else {
        ptr += IDA1012_SNAME_LEN; /* skip over station name */
    }
    ptr += IDA1012_NNAME_LEN; /* skip over network code */

    ptr += UnpackSEEDtag(ptr, &cmnhdr->ttag.beg.seed);
    cmnhdr->ttag.beg.type = IDA10_TIMER_SEED;
    cmnhdr->ttag.beg.derived = FALSE;

    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.seqno);
    ptr += utilUnpackUINT32(ptr, &cmnhdr->extra.tstamp);
    cmnhdr->extra.valid = TRUE;

    ptr += IDA1012_RESERVED_BYTES;

    ptr += utilUnpackUINT16(ptr, &stmp); cmnhdr->nbytes = (int) stmp;

/* 10.12 doesn't have end time tag... set to beginning for now.
 * Packets which care about end time will overwrite later.
 */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;

    return (int) (ptr - start);
}

#ifdef INCLUDE_IDA9_SUPPORT
/* Unpack the 10.9 common header */
/* NOTE: 10.9 is really IDA9, so we go through some contortions here */

static int UnpackCmnHdr9(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
IDA *ida;
IDA_DHDR dhdr;

    if ((ida = ida10AppSuppliedGetIdaHandle()) == NULL) { /* this MUST exist in the app */
        memset(cmnhdr, 0, sizeof(IDA10_CMNHDR));
        cmnhdr->format = 10;
        cmnhdr->subformat = IDA10_SUBFORMAT_9;
        return IDA10_CMNHEADLEN;
    }

    if (dhead_rev9(ida, &dhdr, start) != 0) {
        memset(cmnhdr, 0xee, sizeof(IDA10_CMNHDR));
        cmnhdr->format = 10;
        cmnhdr->subformat = IDA10_SUBFORMAT_9;
        return IDA10_CMNHEADLEN;
    }

    cmnhdr->ttag.beg.type = IDA10_TIMER_IDA9;
    cmnhdr->ttag.beg.ida9 = dhdr.beg;

    cmnhdr->ttag.end.type = IDA10_TIMER_IDA9;
    cmnhdr->ttag.end.ida9 = dhdr.end;

    cmnhdr->nbytes = IDA10_DEFDATALEN;

    cmnhdr->extra = dhdr.extra;
    cmnhdr->boxid = IDA10_64BIT_BOXID;

/* The IDA10.9 "serial number" is TBD */

    cmnhdr->serialno = 0;

    return IDA10_CMNHEADLEN;
}
#endif /* INCLUDE_IDA9_SUPPORT */

/* Unpack the common header */

int ida10UnpackCmnHdr(UINT8 *start, IDA10_CMNHDR *cmnhdr)
{
int len;

    if ((cmnhdr->type = ida10Type(start)) == IDA10_TYPE_UNKNOWN) return 0;

    cmnhdr->format = 10;

/* sub-format specific decoders */

    cmnhdr->extra = default_extra;
    switch (ida10SubFormatCode(start)) {
      case IDA10_SUBFORMAT_0:
        len = UnpackCmnHdr0(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_1:
        len = UnpackCmnHdr1(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_2:
        len = UnpackCmnHdr2(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_3:
        len = UnpackCmnHdr3(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_4:
        len = UnpackCmnHdr4(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_5:
        len = UnpackCmnHdr5(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_6:
        len = UnpackCmnHdr6(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_7:
        len = UnpackCmnHdr7(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_8:
        len = UnpackCmnHdr8(start, cmnhdr);
        break;
#ifdef INCLUDE_IDA9_SUPPORT
      case IDA10_SUBFORMAT_9:
        len = UnpackCmnHdr9(start, cmnhdr);
        break;
#endif /* INCLUDE_IDA9_SUPPORT */
      case IDA10_SUBFORMAT_10:
        len = UnpackCmnHdr10(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_11:
        len = UnpackCmnHdr11(start, cmnhdr);
        break;
      case IDA10_SUBFORMAT_12:
        len = UnpackCmnHdr12(start, cmnhdr);
        break;
      default:
        return 0;
    }

/* Derive the clock idependent timing stuff */

    ida10CompleteTimeTag(&cmnhdr->ttag.beg);
    ida10CompleteTimeTag(&cmnhdr->ttag.end);
    cmnhdr->beg = cmnhdr->ttag.beg.epoch + (REAL64) SAN_EPOCH_TO_1970_EPOCH;
    cmnhdr->end = cmnhdr->ttag.end.epoch + (REAL64) SAN_EPOCH_TO_1970_EPOCH;

    return len;
}

/* Unpack a TS header */

int ida10UnpackTSHdr(UINT8 *start, IDA10_TSHDR *tshdr)
{
UINT8 *ptr;

/* First, unpack the raw information */

    ptr = start;
    ptr += ida10UnpackCmnHdr(ptr, &tshdr->cmn);
    if (tshdr->cmn.type != IDA10_TYPE_TS) return 0;
/* Special exception for IDA9 support */

#ifdef INCLUDE_IDA9_SUPPORT
    if (tshdr->cmn.subformat == IDA10_SUBFORMAT_9) return UnpackIda9(start, tshdr);
#endif /* INCLUDE_IDA9_SUPPORT */

/* Sane IDA10 unpacking here */

    ptr += utilUnpackBytes(ptr, (UINT8 *) tshdr->dl.strm, IDA10_CNAMLEN);
    ptr += utilUnpackBytes(ptr, &tshdr->dl.format, 1);
    ptr += utilUnpackBytes(ptr, &tshdr->dl.gain, 1);
    ptr += utilUnpackUINT16(ptr, &tshdr->dl.nsamp);
    ptr += utilUnpackINT16(ptr, &tshdr->dl.srate.factor);
    ptr += utilUnpackINT16(ptr, &tshdr->dl.srate.multiplier);

/* Now derive the derived stuff */

    if (tshdr->cmn.subformat == IDA10_SUBFORMAT_5 || tshdr->cmn.subformat == IDA10_SUBFORMAT_8 || tshdr->cmn.subformat == IDA10_SUBFORMAT_12) {
        strlcpy(tshdr->sname, (char *) &tshdr->cmn.serialno, IDA10_SNAMLEN+1);
    } else if (tshdr->cmn.boxid == IDA10_64BIT_BOXID) {
        sprintf(tshdr->sname, "%04x", (int) tshdr->cmn.serialno & 0xffff);
    } else {
        sprintf(tshdr->sname, "%04x", tshdr->cmn.boxid);
    }
    tshdr->dl.strm[IDA10_CNAMLEN] = 0;
    strlcpy(tshdr->cname, tshdr->dl.strm, IDA10_CNAMLEN+1);
    tshdr->nsint = mseedFactMultToNsint(tshdr->dl.srate.factor, tshdr->dl.srate.multiplier);
    tshdr->sint  = utilNsToS(tshdr->nsint);
    if (tshdr->nsint != 0) {
        tshdr->srate = 1.0 / tshdr->sint;
    } else {
        tshdr->srate = 0; /* let the application deal with this */
    }
    tshdr->gain  = (int) tshdr->dl.gain;
    tshdr->nsamp = (int) tshdr->dl.nsamp;
    ida10SetEndTime(&tshdr->cmn, tshdr->nsint * (tshdr->nsamp - 1));
    tshdr->tofs   = tshdr->cmn.beg;
    tshdr->tols   = tshdr->cmn.end;
    tshdr->unused = 0; /* for now */
    tshdr->datatype = datatype(tshdr->cmn.subformat, tshdr->dl.format);

    return (int) (ptr - start);
}

/* function: UnpackMSEED512
 *
 * Unpack (and decompress) waveform data from MSEED 512-byte record
 *
 *  mseedptr: pointer to 512-byte raw miniseed data
 *        ts: pointer to IDA10_TS struct into which
 *            the uncompresesed miniseed timeseries data will be written
 *
 *   returns: The IDA10 data type of the waveform data
 */
static int UnpackMSEED512(UINT8 *mseedptr, IDA10_TS *ts)
{
int wrdsiz, nbyte, retval;
MSEED_RECORD mseed;

    if (!mseedUnpackRecord(&mseed, mseedptr)) return IDA10_DATA_UNKNOWN;

    switch (mseed.hdr.format) {
        case MSEED_FORMAT_INT_16:
            wrdsiz = 2;
            retval = IDA10_DATA_INT16;
            break;
        case MSEED_FORMAT_INT_32:
            wrdsiz = 4;
            retval = IDA10_DATA_INT32;
            break;
        case MSEED_FORMAT_IEEE_F:
            wrdsiz = 4;
            retval = IDA10_DATA_REAL32;
            break;
        case MSEED_FORMAT_IEEE_D:
            wrdsiz = 8;
            retval = IDA10_DATA_REAL64;
            break;
        default: return IDA10_DATA_TYPE_ERR;
    }

    if ((nbyte = wrdsiz * mseed.hdr.nsamp) > IDA10_MAXDATALEN) return IDA10_DATA_TYPE_ERR;
    memcpy(ts->data.int8, mseed.dat.uint8, nbyte);

    return retval;

}
/* Unpack a TS record, data included */

BOOL ida10UnpackTS(UINT8 *start, IDA10_TS *ts)
{
int subformat;
int i, AvailableDataSpace;
UINT8 *ptr;
BOOL VariableLengthData = FALSE;

    subformat = ida10SubFormatCode(start);

    ptr = start;
    ptr += ida10UnpackTSHdr(start, &ts->hdr);

    if (ts->hdr.cmn.type == IDA10_TYPE_UNKNOWN) {
        errno = EINVAL;
        return FALSE;
    } else if (ts->hdr.cmn.type != IDA10_TYPE_TS) {
        errno = 0;
        return FALSE;
    }

/* sanity check */

    if (ts->hdr.nsamp < 0) {
        errno = EINVAL;
        return FALSE;
    }

/* Can only deal with uncompressed data at the moment */

    if ((ts->hdr.dl.format & IDA10_MASK_COMP) != IDA10_COMP_NONE) {
        errno = UNSUPPORTED;
        return FALSE;
    }

/* Unpack the data */

    switch (ts->hdr.datatype) {

      case IDA10_DATA_INT8:
        if (ts->hdr.nsamp > IDA10_MAXDATALEN) {
            errno = EINVAL;
            return FALSE;
        }
        memcpy(ts->data.int8, ptr, ts->hdr.nsamp);
        ts->hdr.nbytes = ts->hdr.nsamp;
        break;

      case IDA10_DATA_INT16:
        if (ts->hdr.nsamp > IDA10_MAXDATALEN/sizeof(INT16)) {
            errno = EINVAL;
            return FALSE;
        }
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilUnpackINT16(ptr, &ts->data.int16[i]);
        ts->hdr.nbytes = ts->hdr.nsamp * sizeof(INT16);
        break;

      case IDA10_DATA_INT32:
        if (ts->hdr.nsamp > IDA10_MAXDATALEN/sizeof(INT32)) {
            errno = EINVAL;
            return FALSE;
        }
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilUnpackINT32(ptr, &ts->data.int32[i]);
        ts->hdr.nbytes = ts->hdr.nsamp * sizeof(INT32);
        break;

      case IDA10_DATA_REAL32:
        if (ts->hdr.nsamp > IDA10_MAXDATALEN/sizeof(REAL32)) {
            errno = EINVAL;
            return FALSE;
        }
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilUnpackREAL32(ptr, &ts->data.real32[i]);
        ts->hdr.nbytes = ts->hdr.nsamp * sizeof(REAL32);
        break;

      case IDA10_DATA_REAL64:
        if (ts->hdr.nsamp > IDA10_MAXDATALEN/sizeof(REAL64)) {
            errno = EINVAL;
            return FALSE;
        }
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilUnpackREAL64(ptr, &ts->data.real64[i]);
        ts->hdr.nbytes = ts->hdr.nsamp * sizeof(REAL64);
        break;

      case IDA10_DATA_MSEED512:
        ts->hdr.datatype = UnpackMSEED512(ptr, ts);
        ts->hdr.nbytes = ts->hdr.nsamp * sizeof(INT32);
        VariableLengthData = TRUE;

        break;

      default:
        errno = EINVAL;
        return FALSE;
    }

/* Set the short packet flag */

    if (VariableLengthData) {
        ts->hdr.unused = 0;
    } else {
        AvailableDataSpace = ts->hdr.cmn.nbytes - (IDA10_TSHEADLEN - IDA10_CMNHEADLEN);
        ts->hdr.unused = AvailableDataSpace - ts->hdr.nbytes;
    }

/* Initialize the time stamp increment error memory */

    ts->hdr.incerr.nsec = 0;
    ts->hdr.incerr.samp = 0;
    ts->hdr.incerr.ok = TRUE;

    return TRUE;
}

/* Unpack a LM record */

BOOL ida10UnpackLM(UINT8 *start, IDA10_LM *lm)
{
UINT16 len;
UINT8 *ptr;

    ptr = start;
    ptr += ida10UnpackCmnHdr(ptr, &lm->cmn);
    if (lm->cmn.type != IDA10_TYPE_LM) {
        errno = EINVAL;
        return FALSE;
    }
    ptr += utilUnpackUINT16(ptr, &len);
    if (len > IDA10_MAXDATALEN) {
        errno = EINVAL;
        return FALSE;
    }

    memcpy(lm->text, ptr, len);
    if (lm->text[len-1] != 0) lm->text[len-1] = 0;

/* Now derive the derived stuff */

    sprintf(lm->sname, "%04x", (int) lm->cmn.boxid);
    lm->tofs  = lm->cmn.beg;
    lm->tols  = lm->cmn.end;

    return TRUE;
}

/* Unpack a CF record */

BOOL ida10UnpackCF(UINT8 *start, IDA10_CF *cf)
{
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += ida10UnpackCmnHdr(ptr, &cf->cmn);

    if (cf->cmn.type != IDA10_TYPE_CF) {
        errno = EINVAL;
        return FALSE;
    }
    ptr += utilUnpackUINT16(ptr, &sval); cf->seqno = (int) sval;
    ptr += utilUnpackUINT16(ptr, &sval); cf->done  = (int) sval;
    cf->nbytes = cf->cmn.nbytes - (IDA10_CFHEADLEN - IDA10_CMNHEADLEN);
    memcpy(cf->data, ptr, cf->nbytes);

/* Now derive the derived stuff */

    sprintf(cf->sname, "%04x", (int) cf->cmn.boxid);
    cf->tstamp = cf->cmn.beg;

    return TRUE;
}

/* Revision History
 *
 * $Log: unpack.c,v $
 * Revision 1.45  2018/01/18 23:31:09  dechavez
 * changes to support rework of IDA10.12 definition
 *
 * Revision 1.44  2017/10/19 23:49:58  dauerbach
 * Added support for subformat 10.12 which has mseed 512 records as it's payload
 *
 * Revision 1.43  2015/12/04 22:06:27  dechavez
 * casts and format fixes to calm OS X compiles
 *
 * Revision 1.42  2014/08/29 20:36:55  dechavez
 * use mseedFactMultToNsint() instead of ms_nomsampinterval() to compute nsint
 *
 * Revision 1.41  2014/04/14 18:29:42  dechavez
 * use libmseed's ms_nomsampinterval() to get nominal sample interval
 *
 * Revision 1.40  2013/09/20 16:11:08  dechavez
 * IDA10_SUBFORMAT_10 replaced with IDA10_SUBFORMAT_11 in UnpackCmnHdr11()
 *
 * Revision 1.39  2013/07/19 17:50:25  dechavez
 * IDA10_OBSTAG2 and IDA10.11 support
 *
 * Revision 1.38  2013/05/11 22:51:43  dechavez
 * IDA10.10 support
 *
 * Revision 1.37  2011/10/10 19:52:49  dechavez
 * use global IDA10x_RESERVED_BYTES instead of local SUBFORMAT_x_RESERVED_BYTES,
 * set extra.valid FALSE for 10.5, added tstamp decode to 10.8 (and set extra.valid TRUE)
 *
 * Revision 1.36  2011/03/17 17:29:05  dechavez
 * moved setting of datatype from ida10UnpackTS() to ida10UnpackTSHdr()
 *
 * Revision 1.35  2010/09/30 17:42:05  dechavez
 * checkpoint with initial support of IDA9 included, but ifdef'd out
 *
 * Revision 1.34  2010/09/10 22:54:01  dechavez
 * added 10.6, 10.7, and 10.8 support
 *
 * Revision 1.33  2010/05/10 16:17:58  dechavez
 * avoid division by zero error when a bad header results in nsint == 0
 *
 * Revision 1.32  2010/03/12 00:39:30  dechavez
 * support for 10.4 src bytes
 *
 * Revision 1.30  2009/02/03 22:48:09  dechavez
 * fixed sign problem in sint() (aap)
 *
 * Revision 1.29  2008/01/07 22:00:11  dechavez
 * added ida10PackGenericTtag()
 *
 * Revision 1.28  2007/12/21 20:49:11  dechavez
 * identify 10.5 time stamps as being of type IDA10_TIMER_GENERIC
 *
 * Revision 1.27  2007/12/14 21:36:58  dechavez
 * IDA10.5 support
 *
 * Revision 1.26  2007/06/01 19:06:53  dechavez
 * use IDA_DEFAULT_EXTRA to initialize ida10CMNHDR "extra"
 *
 * Revision 1.25  2007/01/08 15:49:40  dechavez
 * strlcpy() instead of strncpy()
 *
 * Revision 1.24  2007/01/04 17:53:06  dechavez
 * support for nanosecond sint and incerr struct in TS header
 *
 * Revision 1.23  2006/12/22 02:46:59  dechavez
 * initial release
 *
 * Revision 1.22  2006/12/08 17:27:31  dechavez
 * preliminary 10.4 support
 *
 * Revision 1.21  2006/11/10 06:14:55  dechavez
 * updated 10.3 support, added proper REAL64 support
 *
 * Revision 1.20  2006/08/19 02:18:34  dechavez
 * removed trailing CVS junk from remarks
 *
 * Revision 1.19  2006/08/19 02:17:46  dechavez
 * further 10.3 support
 *
 * Revision 1.18  2006/08/15 00:53:44  dechavez
 * removed premature 64 bit data type support, added OFIS ttag support
 *
 * Revision 1.17  2005/10/06 19:38:40  dechavez
 * include format and subformat in common header, work around bogus nbuytes in early 10.2 CF records
 *
 * Revision 1.16  2005/08/26 18:41:44  dechavez
 * added 10.2 support
 *
 * Revision 1.15  2005/05/25 22:38:06  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.14  2005/03/23 21:26:01  dechavez
 * added (untested) support for 64-bit data types
 *
 * Revision 1.13  2004/06/25 18:39:18  dechavez
 * fixed dangling comment in WIN32 ifdef causing nasty ComputeEndTimeTS bug
 *
 * Revision 1.12  2004/06/24 18:19:12  dechavez
 * removed unnecessary includes, WIN32 port (aap)
 *
 * Revision 1.11  2003/12/10 05:36:53  dechavez
 * use sign/unsigned specific utilUnpack...s
 *
 * Revision 1.10  2003/10/16 16:49:11  dechavez
 * added support for all (uncompressed) data types in ida10UnpackTS()
 *
 * Revision 1.9  2003/05/23 19:49:16  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.8  2002/11/06 00:51:56  dechavez
 * Changed remaining "out" argument names to more type specific version
 *
 * Revision 1.7  2002/11/05 17:46:03  dechavez
 * added support for new derived "unused" flag in TS headers, changed "out"
 * argument name in various functions to a more type specific version to aid
 * source browsing
 *
 * Revision 1.6  2002/03/15 22:42:01  dec
 * added 10.1 support
 *
 * Revision 1.5  2001/09/09 01:06:06  dec
 * use IDA10_MAXDATALEN instead of obsolete IDA10_DATALEN
 *
 * Revision 1.4  2001/04/02 21:25:12  dec
 * removed test harness (copied over to new standalone program 'ida10')
 *
 * Revision 1.3  2000/11/08 01:55:19  dec
 * added CF support
 *
 * Revision 1.2  2000/11/02 20:27:56  dec
 * detect type IDA10_TYPE_ISPLOG
 *
 * Revision 1.1.1.1  2000/02/08 20:20:26  dec
 * import existing IDA/NRTS sources
 *
 */
