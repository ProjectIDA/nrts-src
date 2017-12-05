#pragma ident "$Id: pack.c,v 1.6 2017/10/20 00:10:29 dauerbach Exp $"
/* ======================================================================
 *
 *  Create IDA10 records from IDA_TS structures
 *
 *  ******   THIS CODE IS NOT REFERENCED   *******
 *  ******  ANYWHERE IN THE IDA CODE BASE  *******
 *  ******  HOWEVER, IT IS A GOOD READ :)  *******
 *  ====================================================================
 */
#include "ida10.h"

/* Pack a SAN time tag */

static int PackSanTtag(UINT8 *start, IDA10_SANTTAG *san)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, san->ext);
    ptr += utilPackUINT32(ptr, san->sys.sec);
    ptr += utilPackUINT16(ptr, san->sys.msc);
    ptr += utilPackUINT16(ptr, san->status.raw);
    ptr += utilPackUINT16(ptr, san->pll);
    ptr += utilPackUINT16(ptr, san->phase);
    ptr += utilPackUINT32(ptr, san->epoch.sec);
    ptr += utilPackUINT16(ptr, san->epoch.msc);

    return (int) (ptr - start);
}

/* Pack an OFIS time tag */

static int PackOfisTtag(UINT8 *start, IDA10_OFISTAG *ofis)
{
UINT8 *ptr;
UINT32 value;
char tmp[32];

    ptr = start;
    ptr += utilPackUINT32(ptr, ofis->ext);
    ptr += utilPackUINT64(ptr, ofis->sys);
    ptr += utilPackUINT64(ptr, ofis->pps);
    ptr += utilPackUINT16(ptr, ofis->status.raw);

    return (int) (ptr - start);
}

/* Pack a Q330 time tag */

static int PackQ330Ttag(UINT8 *start, IDA10_Q330TAG *q330)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, q330->seqno);
    ptr += utilPackINT32(ptr, q330->offset.sec);
    ptr += utilPackINT32(ptr, q330->offset.usec);
    ptr += utilPackINT32(ptr, q330->offset.index);
    ptr += utilPackINT32(ptr, q330->delay);
    ptr += utilPackINT16(ptr, q330->loss);
    *ptr++ = q330->qual.bitmap;
    *ptr++ = q330->qual.percent;

    return (int) (ptr - start);
}

/* Pack a generic time tag */

int PackGenericTtag(UINT8 *start, IDA10_GENTAG *gen)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT64(ptr, gen->tstamp);
    *ptr++ = gen->status.receiver;
    *ptr++ = gen->status.generic;

    return (int) (ptr - start);
}

/* Pack an OBS time tag */

int PackSBDHDR(UINT8 *start, IDA10_SBDHDR *sbd)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT64(ptr, sbd->imei);
    ptr += utilPackUINT16(ptr, sbd->momsn);
    ptr += utilPackUINT32(ptr, sbd->tstamp);

    return (int) (ptr - start);
}

int PackOBSTtag(UINT8 *start, IDA10_OBSTAG *obs)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT64(ptr, obs->sys);
    ptr += utilPackUINT64(ptr, obs->ref);
    ptr += utilPackUINT32(ptr, obs->ticrate);
    ptr += PackSBDHDR(ptr, &obs->sbd);

    return (int) (ptr - start);
}

int PackOBSTtag2(UINT8 *start, IDA10_OBSTAG2 *obs2)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT64(ptr, obs2->sys);
    ptr += utilPackUINT32(ptr, obs2->ref);
    ptr += utilPackUINT32(ptr, obs2->delay);
    ptr += utilPackUINT32(ptr, obs2->ticrate);
    ptr += PackSBDHDR(ptr, &obs2->sbd);

    return (int) (ptr - start);
}

/* Pack the 10.0 common header */

static int PackHdr0(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackSanTtag(ptr, &hdr->cmn.ttag.beg.san);
    ptr += PackSanTtag(ptr, &hdr->cmn.ttag.end.san);

    return (int) (ptr - start);
}

/* Pack the 10.1 common header */

static int PackHdr1(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackSanTtag(ptr, &hdr->cmn.ttag.beg.san);
    memset(ptr, 0xee, IDA101_RESERVED_BYTES); ptr += IDA101_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.2 common header */

static int PackHdr2(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackSanTtag(ptr, &hdr->cmn.ttag.beg.san);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    memset(ptr, 0xee, IDA102_RESERVED_BYTES); ptr += IDA102_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.3 common header */

static int PackHdr3(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackOfisTtag(ptr, &hdr->cmn.ttag.beg.ofis);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    memset(ptr, 0xee, IDA103_RESERVED_BYTES); ptr += IDA103_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.4 common header */

static int PackHdr4(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT64(ptr, hdr->cmn.serialno);
    ptr += PackQ330Ttag(ptr, &hdr->cmn.ttag.beg.q330);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    *ptr++ = ' ';
    *ptr++ = ' ';
    *ptr++ = hdr->cmn.extra.src[0];
    *ptr++ = hdr->cmn.extra.src[1];
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.5 common header */

static int PackHdr5(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr, *sname, *nname;

    sname = (UINT8 *) &hdr->cmn.serialno;
    nname = sname + IDA105_SNAME_LEN + 1;

    ptr = start;
    ptr += utilPackBytes(ptr, sname, 4);
    ptr += utilPackBytes(ptr, nname, 2);
    ptr += PackGenericTtag(ptr, &hdr->cmn.ttag.beg.gen);
    memset(ptr, 0xee, IDA105_RESERVED_BYTES); ptr += IDA105_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.6 common header */

static int PackHdr6(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackSanTtag(ptr, &hdr->cmn.ttag.beg.san);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.origseqno);
    memset(ptr, 0xee, IDA106_RESERVED_BYTES); ptr += IDA106_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.7 common header */

static int PackHdr7(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, hdr->cmn.boxid);
    ptr += PackOfisTtag(ptr, &hdr->cmn.ttag.beg.ofis);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.origseqno);
    memset(ptr, 0xee, IDA107_RESERVED_BYTES); ptr += IDA107_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.8 common header */

static int PackHdr8(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr, *sname, *nname;

    sname = (UINT8 *) &hdr->cmn.serialno;
    nname = sname + IDA105_SNAME_LEN + 1;

    ptr = start;
    ptr += utilPackBytes(ptr, sname, 4);
    ptr += utilPackBytes(ptr, nname, 2);
    ptr += PackGenericTtag(ptr, &hdr->cmn.ttag.beg.gen);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    memset(ptr, 0xee, IDA108_RESERVED_BYTES); ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.10 common header */

static int PackHdr10(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += PackOBSTtag(ptr, &hdr->cmn.ttag.beg.obs);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    memset(ptr, 0xee, IDA1010_RESERVED_BYTES); ptr += IDA1010_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the 10.11 common header */

static int PackHdr11(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += PackOBSTtag2(ptr, &hdr->cmn.ttag.beg.obs2);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.seqno);
    ptr += utilPackUINT32(ptr, hdr->cmn.extra.tstamp);
    memset(ptr, 0xee, IDA1010_RESERVED_BYTES); ptr += IDA1010_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, hdr->cmn.nbytes);

    return (int) (ptr - start);
}

/* Pack the common header */

int PackCmnHdr(UINT8 *start, IDA10_TSHDR *hdr)
{
UINT8 *ptr;

/* the 4-byte IDA10 common header */

    ptr = start;
    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = hdr->cmn.format;
    *ptr++ = hdr->cmn.subformat;

/* sub-format specific encode */

    switch (hdr->cmn.subformat) {
      case IDA10_SUBFORMAT_0:  ptr += PackHdr0(ptr, hdr); break;
      case IDA10_SUBFORMAT_1:  ptr += PackHdr1(ptr, hdr); break;
      case IDA10_SUBFORMAT_2:  ptr += PackHdr2(ptr, hdr); break;
      case IDA10_SUBFORMAT_3:  ptr += PackHdr3(ptr, hdr); break;
      case IDA10_SUBFORMAT_4:  ptr += PackHdr4(ptr, hdr); break;
      case IDA10_SUBFORMAT_5:  ptr += PackHdr5(ptr, hdr); break;
      case IDA10_SUBFORMAT_6:  ptr += PackHdr6(ptr, hdr); break;
      case IDA10_SUBFORMAT_7:  ptr += PackHdr7(ptr, hdr); break;
      case IDA10_SUBFORMAT_8:  ptr += PackHdr8(ptr, hdr); break;
      case IDA10_SUBFORMAT_10: ptr += PackHdr10(ptr, hdr); break;
      case IDA10_SUBFORMAT_11: ptr += PackHdr11(ptr, hdr); break;
      /* case IDA10_SUBFORMAT_12: ptr += PackHdr12(ptr, hdr); break; */
      default: return 0;
    }

    return (int) (ptr - start);
}

/* Pack a TS header */

int ida10PackTSHdr(UINT8 *start, IDA10_TSHDR *tshdr)
{
UINT8 *ptr;

    ptr = start;
    ptr += PackCmnHdr(ptr, tshdr);
    ptr += utilPackBytes(ptr, (UINT8 *) tshdr->dl.strm, IDA10_CNAMLEN);
    *ptr++ = tshdr->dl.format;
    *ptr++ = tshdr->dl.gain;
    ptr += utilPackUINT16(ptr, tshdr->dl.nsamp);
    ptr += utilPackINT16(ptr, tshdr->dl.srate.factor);
    ptr += utilPackINT16(ptr, tshdr->dl.srate.multiplier);

    return (int) (ptr - start);
}

/* Pack a TS record, data included.  NO BOUNDS CHECKING! */

int ida10PackTS(UINT8 *start, IDA10_TS *ts)
{
int subformat;
int i, AvailableDataSpace;
UINT8 *ptr;

    if (ts->hdr.cmn.type != IDA10_TYPE_TS) {
        errno = EINVAL;
        return -1;
    }

    ptr = start;
    ptr += ida10PackTSHdr(ptr, &ts->hdr);

    switch (ts->hdr.datatype) {

      case IDA10_DATA_INT8:
        utilPackBytes(ptr, (UINT8 *) ts->data.int8, ts->hdr.nsamp);
        break;

      case IDA10_DATA_INT16:
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilPackINT16(ptr, ts->data.int16[i]);
        break;

      case IDA10_DATA_INT32:
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilPackINT32(ptr, ts->data.int32[i]);
        break;

      case IDA10_DATA_REAL32:
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilPackREAL32(ptr, ts->data.real32[i]);
        break;

      case IDA10_DATA_REAL64:
        for (i = 0; i < ts->hdr.nsamp; i++) ptr += utilPackREAL64(ptr, ts->data.real64[i]);
        break;

      default:
        errno = EINVAL;
        return -2;
    }

    return (int) (ptr - start);
}
/*
 *  ******   THIS CODE IS NOT REFERENCED   *******
 *  ******  ANYWHERE IN THE IDA CODE BASE  *******
 *  ******  HOWEVER, IT IS A GOOD READ :)  *******
 */

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.6  2017/10/20 00:10:29  dauerbach
 * fix commit msgs, hopefully...
 *
 * Revision 1.5  2017/10/19 23:49:58  dauerbach
 * inserted comments about being this source file being unreferenced
 *
 * Revision 1.4  2015/12/04 22:06:27  dechavez
 * casts and format fixes to calm OS X compiles
 *
 * Revision 1.3  2013/07/19 17:50:25  dechavez
 * IDA10_OBSTAG2 and IDA10.11 support
 *
 * Revision 1.2  2013/05/11 22:51:43  dechavez
 * IDA10.10 support
 *
 * Revision 1.1  2013/02/07 17:58:54  dechavez
 * created
 *
 */
