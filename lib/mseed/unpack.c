#pragma ident "$Id: unpack.c,v 1.10 2017/06/20 21:55:38 dechavez Exp $"
/*======================================================================
 * 
 * Unpack stuff
 *
 * This code is complicated because SEED neglected to specify anything
 * about byte order!  So, we have to guess what byte order the header
 * fields are in by looking at some multi-byte quantities and deciding
 * in what byte order they have reasonable values.  Gag.
 *
 *====================================================================*/
#include "mseed.h"
extern int msr_unpack_steim1 (UINT8*, int, int, int, INT32*, INT32*, INT32*, INT32*, int, int);
extern int msr_unpack_steim2 (UINT8*, int, int, int, INT32*, INT32*, INT32*, INT32*, int, int);

/* Unpack multi-byte quantities with possible byte swap */

INT16 mseedUnpackINT16(UINT8 *src, BOOL swap)
{
INT16 value;

    memcpy((void *) &value, src, sizeof(INT16));
    if (swap) utilSwapINT16(&value, 1);

    return value;
}

INT32 mseedUnpackINT32(UINT8 *src, BOOL swap)
{
INT32 value;

    memcpy((void *) &value, src, sizeof(INT32));
    if (swap) utilSwapINT32(&value, 1);

    return value;
}

INT64 mseedUnpackBtime(UINT8 *src, BOOL swap)
{
INT64 tstamp;
int year, day, hr, mn, sc, frac, ns;

    year = mseedUnpackINT16(src,   swap);
    day  = mseedUnpackINT16(src+2, swap);
    hr   = src[4];
    mn   = src[5];
    sc   = src[6];
    frac = mseedUnpackINT16(src+8, swap);
    ns   = (frac * 100) * NANOSEC_PER_USEC;

    tstamp = utilComposeTimestamp(year, day, hr, mn, sc, ns);

    return tstamp;
}

UINT64 mseedUnpackSampleInterval(UINT8 *src, BOOL swap)
{
int fact, mult;

    fact = (int) mseedUnpackINT16(src,   swap);
    mult = (int) mseedUnpackINT16(src+2, swap);

    return mseedFactMultToNsint(fact, mult);
}

/* Given a raw MiniSEED packet, figure out the header byte order.
 * We do this by looking at the year and day starting at offset 20
 * and seeing if they look reasonable.
 */

int mseedHeaderByteOrder(UINT8 *raw)
{
INT16 year, day;
int native_order, other_order, order;

    if (raw == NULL) return UNKNOWN_BYTE_ORDER;

    native_order = NATIVE_BYTE_ORDER;
    other_order  = (native_order == LTL_ENDIAN_BYTE_ORDER) ? BIG_ENDIAN_BYTE_ORDER : LTL_ENDIAN_BYTE_ORDER;

    memcpy((void *) &year, raw+20, 2);
    memcpy((void *) &day,  raw+22, 2);

    order = (year < 0 || year > 2038 || day < 0 || day > 366) ? other_order : native_order;

    return order;
}

void mseedUnpackB100(MSEED_HDR *dest, UINT8 *src, BOOL swap)
{
REAL32 srate;

    memcpy((void *) &srate, src, sizeof(REAL32));
    if (swap) utilSwapREAL32(&srate, 1);
    if (srate == 0.0) return;

    dest->asint = (UINT64) (NANOSEC_PER_SEC / srate);
}

void mseedUnpackB1000(MSEED_HDR *dest, UINT8 *src, BOOL unused)
{
    dest->format = src[0];
    dest->order = src[1];
    dest->reclen = pow(2, src[2]);
}

void mseedUnpackB1001(MSEED_HDR *dest, UINT8 *src, BOOL unused)
{
int usec;

    dest->tqual = src[0];
    usec        = src[2];
    /* ignore the rest */

    dest->tstamp += usec * NANOSEC_PER_USEC;
}

int mseedUnpackFSDH(MSEED_HDR *dest, UINT8 *src)
{
UINT8 *ptr;
int order, NextBlockette, type;
BOOL swap;

    if (dest == NULL || src == NULL) {
        errno = EINVAL;
        return -1;
    }

    mseedInitHeader(dest);

    if ((order = mseedHeaderByteOrder(src)) == UNKNOWN_BYTE_ORDER) return -1;
    swap = (order == NATIVE_BYTE_ORDER) ? FALSE : TRUE;

    sscanf((const char *)src, "%06dD", &dest->seqno);
    dest->flags.qc = src[6];
    strlcpy(dest->staid, (const char *)(src+ 8), MSEED_SNAMLEN+1); utilTrimString(dest->staid);
    strlcpy(dest->locid, (const char *)(src+13), MSEED_LNAMLEN+1);
    strlcpy(dest->chnid, (const char *)(src+15), MSEED_CNAMLEN+1); utilTrimString(dest->chnid);
    strlcpy(dest->netid, (const char *)(src+18), MSEED_NNAMLEN+1); utilTrimString(dest->netid);

    dest->tstamp    = mseedUnpackBtime(src+20, swap);
    dest->nsamp     = (INT32) mseedUnpackINT16(src+30, swap);
    dest->nsint     = mseedUnpackSampleInterval(src+32, swap);
    dest->flags.act = src[36];
    dest->flags.ioc = src[37];
    dest->flags.dat = src[38];
    /* skip number of blockettes, not needed */
    dest->tcorr     = mseedUnpackINT32(src+40, swap);
    dest->bod       = (int) mseedUnpackINT16(src+44, swap);
    NextBlockette   = mseedUnpackINT16(src+46, swap);

    return NextBlockette;
}

BOOL mseedUnpackHeader(MSEED_HDR *dest, UINT8 *src)
{
UINT8 *ptr;
int order, type, NextBlockette;
BOOL swap;

    if (dest == NULL || src == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Figure out what byte order the header is written in */

    if ((order = mseedHeaderByteOrder(src)) == UNKNOWN_BYTE_ORDER) return FALSE;
    swap = (order == NATIVE_BYTE_ORDER) ? FALSE : TRUE;

/* Decode the FSDH and any subsequent blockettes */

    NextBlockette = mseedUnpackFSDH(dest, src);
    while (NextBlockette != 0) {
        ptr = src + NextBlockette;
        type = mseedUnpackINT16(ptr, swap); ptr += sizeof(INT16);
        NextBlockette = mseedUnpackINT16(ptr, swap); ptr += sizeof(INT16);
        switch (type) {
          case 100:  mseedUnpackB100 (dest, ptr, swap); break;
          case 1000: mseedUnpackB1000(dest, ptr, swap); break;
          case 1001: mseedUnpackB1001(dest, ptr, swap); break;
        }
    }
    dest->sint = (dest->asint > 0) ? dest->asint : dest->nsint;
    mseedSetEndtime(dest);
    mseedSetIdent(dest);

    return TRUE;
}

static BOOL UnpackDataInt16(MSEED_RECORD *dest, UINT8 *src, BOOL swap)
{
    dest->hdr.format = mseedCopyINT16(dest->dat.int16, (INT16 *) src, dest->hdr.nsamp);
    if (swap) utilSwapINT16(dest->dat.int16, dest->hdr.nsamp);
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return TRUE;
}

static BOOL UnpackDataInt32(MSEED_RECORD *dest, UINT8 *src, BOOL swap)
{
    dest->hdr.format = mseedCopyINT32(dest->dat.int32, (INT32 *) src, dest->hdr.nsamp);
    if (swap) utilSwapINT32(dest->dat.int32, dest->hdr.nsamp);
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return TRUE;
}

static BOOL UnpackDataReal32(MSEED_RECORD *dest, UINT8 *src, BOOL swap)
{
    dest->hdr.format = mseedCopyREAL32(dest->dat.real32, (REAL32 *) src, dest->hdr.nsamp);
    if (swap) utilSwapREAL32(dest->dat.real32, dest->hdr.nsamp);
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return TRUE;
}

static BOOL UnpackDataReal64(MSEED_RECORD *dest, UINT8 *src, BOOL swap)
{
    dest->hdr.format = mseedCopyREAL64(dest->dat.real64, (REAL64 *) src, dest->hdr.nsamp);
    if (swap) utilSwapREAL64(dest->dat.real64, dest->hdr.nsamp);
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return TRUE;
}

static BOOL UnpackDataSteim1(MSEED_RECORD *dest, UINT8 *src, INT32 srclen)
{
int retval, swapflag;
INT32 unused1[MSEED_MAX_BUFLEN/sizeof(INT32)], unused2, unused3;

    swapflag = (dest->hdr.order == NATIVE_BYTE_ORDER) ? 0 : 1;

    retval = msr_unpack_steim1(
        src,             /* ptr to Steim1 data frames */
        srclen,          /* number of bytes in all data frames */
        dest->hdr.nsamp, /* number of samples in all data frames */
        dest->hdr.nsamp, /* number of samples requested by caller */
        dest->dat.int32, /* ptr to unpacked data array */
        unused1,         /* ptr to unpacked diff array */
        &unused2,        /* ptr to hold X0, first sample in frame */
        &unused3,        /* ptr to hold XN, last sample in frame */
        swapflag,        /* non-zero if input should be byte-swapped */
        0                /* verbose mode suppressed */
    );
    dest->hdr.format = MSEED_FORMAT_INT_32;
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return (retval == dest->hdr.nsamp) ? TRUE : FALSE;
}

static BOOL UnpackDataSteim2(MSEED_RECORD *dest, UINT8 *src, INT32 srclen)
{
int retval, swapflag;
INT32 unused1[MSEED_MAX_BUFLEN/sizeof(INT32)], unused2, unused3;

    swapflag = (dest->hdr.order == NATIVE_BYTE_ORDER) ? 0 : 1;

    retval = msr_unpack_steim2(
        src,             /* ptr to Steim1 data frames */
        srclen,          /* number of bytes in all data frames */
        dest->hdr.nsamp, /* number of samples in all data frames */
        dest->hdr.nsamp, /* number of samples requested by caller */
        dest->dat.int32, /* ptr to unpacked data array */
        unused1,         /* ptr to unpacked diff array */
        &unused2,        /* ptr to hold X0, first sample in frame */
        &unused3,        /* ptr to hold XN, last sample in frame */
        swapflag,        /* non-zero if input should be byte-swapped */
        0                /* verbose mode suppressed */
    );
    dest->hdr.format = MSEED_FORMAT_INT_32;
    dest->hdr.order = NATIVE_BYTE_ORDER;

    return (retval == dest->hdr.nsamp) ? TRUE : FALSE;
}

BOOL mseedUnpackRecord(MSEED_RECORD *dest, UINT8 *src)
{
int dlen;
BOOL swap;
UINT8 *data;

    if (dest == NULL || src == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    memset(dest, 0, sizeof(MSEED_RECORD));

    if (!mseedUnpackHeader(&dest->hdr, src)) return FALSE;

    data = &src[dest->hdr.bod];
    dlen = dest->hdr.reclen - dest->hdr.bod;
    swap = (dest->hdr.order == NATIVE_BYTE_ORDER) ? FALSE : TRUE;

    switch (dest->hdr.format) {
      case MSEED_FORMAT_INT_16: return UnpackDataInt16 (dest, data, swap);
      case MSEED_FORMAT_INT_32: return UnpackDataInt32 (dest, data, swap);
      case MSEED_FORMAT_IEEE_F: return UnpackDataReal32(dest, data, swap);
      case MSEED_FORMAT_IEEE_D: return UnpackDataReal64(dest, data, swap);
      case MSEED_FORMAT_STEIM1: return UnpackDataSteim1(dest, data, dlen);
      case MSEED_FORMAT_STEIM2: return UnpackDataSteim2(dest, data, dlen);
      default:
        errno = ENOTSUP;
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: unpack.c,v $
 * Revision 1.10  2017/06/20 21:55:38  dechavez
 * fixed sample interval vs sample rate bug in mseedUnpackB100()
 *
 * Revision 1.9  2015/12/04 23:29:16  dechavez
 * fixed msr_unpack_steimX prototypes to calm OS X compiles
 *
 * Revision 1.8  2015/11/13 20:48:32  dechavez
 * cast some UNIT8 ptrs to const char ptrs to calm Darwin compiles
 *
 * Revision 1.7  2015/10/02 15:35:50  dechavez
 * moved mseedSetIdent() call to the end of mseedUnpackHeader() in order to
 * capture the format and sint needed by sig
 *
 * Revision 1.6  2015/09/24 22:17:13  dechavez
 * accomodate change of nsamp to INT32 in mseedUnpackFSDH()
 *
 * Revision 1.5  2015/09/15 23:23:58  dechavez
 * split FSDH decoding out of mseedUnpackHeader() into mseedUnpackFSDH(), made the
 * previously static "internal" decoders public so that they could be used in read.c
 *
 * Revision 1.4  2014/08/11 18:18:02  dechavez
 * initial release
 *
 */
