#pragma ident "$Id: decode.c,v 1.6 2015/12/04 23:05:17 dechavez Exp $"
/*======================================================================
 *
 *  Decode various data structures
 *
 *====================================================================*/
#include "qdplus.h"

int qdplusUnpackWrappedQDP(UINT8 *start, QDPLUS_PKT *dest)
{
UINT8 *ptr;
int dlen;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    ptr += utilUnpackUINT32(ptr, &dest->seqno.signature);
    ptr += utilUnpackUINT64(ptr, &dest->seqno.counter);
    ptr += utilUnpackBytes(ptr, dest->qdp.raw, QDP_CMNHDR_LEN);
    qdpNetToHost(&dest->qdp);
    dlen = dest->qdp.hdr.dlen < QDP_MAX_PAYLOAD ? dest->qdp.hdr.dlen : QDP_MAX_PAYLOAD;
    ptr += utilUnpackBytes(ptr, dest->qdp.raw + QDP_CMNHDR_LEN, dlen);

    return (int) (ptr - start);
}

/* decode a QDPLUS_PAROSCI, leaving differences in place */

static void DecodeUserParosci(UINT8 *start, QDPLUS_PAROSCI *dest)
{
int i;
INT16 tmp;
UINT8 *ptr;

    ptr = start;
    ++ptr; /* skip over identifier */
    ++ptr; /* skip over reserved */
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->chn, ISI_CHNLEN); dest->chn[ISI_CHNLEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->loc, ISI_LOCLEN); dest->loc[ISI_LOCLEN] = 0;
    ++ptr; /* skip over reserved */
    ptr += utilUnpackUINT64(ptr, &dest->sint);
    ptr += utilUnpackUINT64(ptr, &dest->tofs.tstamp);
    dest->tofs.status.receiver = *ptr++;
    dest->tofs.status.generic  = *ptr++;
    ptr += utilUnpackINT32(ptr, &dest->first);
    ptr += 10; /* skip over reserved */
    ptr += utilUnpackUINT16(ptr, &dest->ndiff);
    dest->diff = (INT16 *) ptr;

    for (i = 0; i < dest->ndiff; i++) {
        tmp = dest->diff[i];
        dest->diff[i] = ntohs(tmp);
    }
}

/* decode an arbitrary DT_USER packet */

void qdplusDecodeUser(UINT8 *start, QDPLUS_USERPKT *dest)
{
UINT8 *ptr = start;

    switch (dest->type = (int) *ptr++) {
      case QDPLUS_DT_USER_PAROSCI: DecodeUserParosci(start, &dest->data.parosci); break;
      default: return;
    }
}

/* Revision History
 *
 * $Log: decode.c,v $
 * Revision 1.6  2015/12/04 23:05:17  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.5  2011/04/07 22:42:39  dechavez
 * replaced qdplusDecodeUserAux with qdplusDecodeUser()+DecodeUserParosci()
 *
 * Revision 1.4  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
