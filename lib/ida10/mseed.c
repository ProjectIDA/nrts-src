#pragma ident "$Id: mseed.c,v 1.4 2011/11/09 23:10:52 dechavez Exp $"
/*======================================================================
 *
 *  Generate equivalent MiniSEED header
 *
 *====================================================================*/
#include "ida10.h"

BOOL ida10ToMseed(UINT8 *src, int srclen, UINT32 seqno, char qcode, char *sta, char *net, LISS_MSEED_HDR *dest)
{
IDA10_TSHDR hdr;
char tmpsta[LISS_SNAMLEN+1];
char tmpnet[LISS_NNAMLEN+1];

    if (src == NULL || net == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Ingore packets that are not a power of two in length */

    if ((dest->b1000.length = util_powerof(srclen, 2)) < 0) {
        errno = ENOTSUP;
        return FALSE;
    }

/* Only support waveform records */

    if (ida10Type(src) != IDA10_TYPE_TS) {
        errno = 0;
        return FALSE;
    }

    ida10UnpackTSHdr(src, &hdr);

/* Ignore unsupported data types */

    switch (hdr.datatype) {
      case IDA10_DATA_INT32:  dest->b1000.format = LISS_INT_32; break;
      case IDA10_DATA_INT16:  dest->b1000.format = LISS_INT_16; break;
      case IDA10_DATA_REAL32: dest->b1000.format = LISS_IEEE_F; break;
      case IDA10_DATA_REAL64: dest->b1000.format = LISS_IEEE_D; break;
      default:
        errno = ENOTSUP;
        return FALSE;
    }

/* Long station names (more than 5 chars) get split into sta and net */

    if (sta == NULL) sta = hdr.sname;
    if (strlen(sta) > LISS_SNAMLEN) {
        memcpy(tmpsta, sta, 4); tmpsta[4] = 0;
        memcpy(tmpnet, sta+4, 2); tmpnet[2] = 0;
        sta = tmpsta;
        net = tmpnet;
    }

/* Set the caller supplied values and possibly massaged station and net names */

    dest->fsdh.seqno = seqno;
    dest->fsdh.qcode = qcode;
    memcpy(dest->fsdh.netid, net, LISS_NNAMLEN); dest->fsdh.netid[LISS_NNAMLEN] = 0;
    memcpy(dest->fsdh.staid, sta, LISS_SNAMLEN);
    dest->fsdh.staid[LISS_SNAMLEN] = 0;

/* The rest comes from the TS header */

    dest->fsdh.bod = IDA10_TSHEADLEN;

    dest->fsdh.start  = hdr.tofs;
    dest->fsdh.nsamp  = hdr.dl.nsamp;
    dest->fsdh.srfact = hdr.dl.srate.factor;
    dest->fsdh.srmult = hdr.dl.srate.multiplier;

    if (strlen(hdr.cname) > LISS_CNAMLEN) {
        memcpy(dest->fsdh.chnid, hdr.cname, LISS_CNAMLEN);
        memcpy(dest->fsdh.locid, &hdr.cname[LISS_CNAMLEN], LISS_LNAMLEN);
    } else {
        strcpy(dest->fsdh.chnid, hdr.cname);
        sprintf(dest->fsdh.locid, "  ");
    }
    dest->fsdh.chnid[LISS_CNAMLEN] = 0; util_ucase(dest->fsdh.chnid);
    dest->fsdh.locid[LISS_LNAMLEN] = 0; util_ucase(dest->fsdh.locid);

    dest->fsdh.active = dest->fsdh.qual = 0;
    dest->fsdh.ioclck = (hdr.cmn.ttag.beg.status.locked) ? LISS_IOC_CLOCK_LOCKED : 0;
    if (hdr.unused) dest->fsdh.ioclck |= LISS_IOC_SHORT_READ;

    dest->b1000.order = LISS_BIG_ENDIAN;

    return TRUE;
}

/* Revision History
 *
 * $Log: mseed.c,v $
 * Revision 1.4  2011/11/09 23:10:52  dechavez
 * 2.14.3
 *
 * Revision 1.3  2011/11/09 20:06:02  dechavez
 * split illegal (too long) station names into sta+net
 *
 * Revision 1.2  2011/10/28 21:53:19  dechavez
 * changed ida10ToMseed() to populate a LISS_MSEED_HDR structure
 *
 * Revision 1.1  2011/10/24 19:37:27  dechavez
 * initial release
 *
 */
