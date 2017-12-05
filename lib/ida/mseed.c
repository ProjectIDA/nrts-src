#pragma ident "$Id: mseed.c,v 1.4 2011/11/09 23:11:09 dechavez Exp $"
/*======================================================================
 *
 *  Generate equivalent MiniSEED header
 *
 *====================================================================*/
#include "ida.h"

BOOL idaToMseed(IDA *ida, UINT8 *src, UINT32 seqno, char qcode, char *net, LISS_MSEED_HDR *dest)
{
IDA_DHDR hdr;
ISI_STREAM_NAME name;

    if (ida == NULL || src == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Only support waveform records */

    if (ida_rtype(src, ida->rev.value) != IDA_DATA) {
        errno = 0;
        return FALSE;
    }

/* Set the caller supplied values */

    dest->fsdh.seqno = seqno;
    dest->fsdh.qcode = qcode;
    memcpy(dest->fsdh.netid, net, LISS_NNAMLEN); dest->fsdh.netid[LISS_NNAMLEN] = 0;

/* Unpack the IDA header to get the rest */

    ida_dhead(ida, &hdr, src);

    idaBuildStreamName(ida, &hdr, &name);
    memcpy(dest->fsdh.staid, name.sta, LISS_SNAMLEN); dest->fsdh.staid[LISS_SNAMLEN] = 0; util_ucase(dest->fsdh.staid);
    memcpy(dest->fsdh.chnid, name.chn, LISS_CNAMLEN); dest->fsdh.chnid[LISS_CNAMLEN] = 0; util_ucase(dest->fsdh.chnid);
    memcpy(dest->fsdh.locid, name.loc, LISS_LNAMLEN); dest->fsdh.locid[LISS_LNAMLEN] = 0; util_ucase(dest->fsdh.locid);

    dest->fsdh.start = hdr.beg.tru;
    dest->fsdh.nsamp = hdr.nsamp;

    switch (ida->rev.value) {
      case 5:
      case 6:
      case 7:
      case 8:
        dest->fsdh.bod = 60;
      default:
        dest->fsdh.bod = 64;
    }

    if (hdr.sint >= 1.0) {
        dest->fsdh.srfact = (int) -hdr.sint;
        dest->fsdh.srmult = 1;
    } else {
        dest->fsdh.srfact = (int) rint((1.0 / hdr.sint));
        dest->fsdh.srmult = 1;
    }

    dest->fsdh.active = dest->fsdh.qual  = 0;
    dest->fsdh.ioclck = (hdr.beg.qual == 1) ? LISS_IOC_CLOCK_LOCKED : 0;
    if (dest->fsdh.bod < 64) dest->fsdh.ioclck |= LISS_IOC_SHORT_READ;

    dest->b1000.length = 10; /* all pre-IDA10 packets are 1024 bytes */
    switch (hdr.wrdsiz) {
      case 2: dest->b1000.format = LISS_INT_16; break;
      case 4: dest->b1000.format = LISS_INT_32; break;
      default:
        errno = EINVAL;
        return 0;
    }
    dest->b1000.order = LISS_BIG_ENDIAN;

    return TRUE;
}

/* Revision History
 *
 * $Log: mseed.c,v $
 * Revision 1.4  2011/11/09 23:11:09  dechavez
 * back out 4.4.2 change (done instead in libliss 1.3.6)
 *
 * Revision 1.3  2011/11/09 22:52:47  dechavez
 * pad short names with spaces in FSDH
 *
 * Revision 1.2  2011/10/28 21:53:37  dechavez
 * changed idaToMseed() to populate a LISS_MSEED_HDR structure
 *
 * Revision 1.1  2011/10/24 19:38:24  dechavez
 * initial release
 *
 */
