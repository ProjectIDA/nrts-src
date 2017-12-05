#pragma ident "$Id: pack.c,v 1.9 2015/12/04 22:18:12 dechavez Exp $"
/*======================================================================
 *
 *  Pack and unpack various SEED things
 *
 *====================================================================*/
#include <math.h>
#include "liss.h"
#include "util.h"

BOOL lissUnpackB1000(LISS_B1000 *dest, UINT8 *src)
{
BOOL swap;
INT16 type;

    memcpy(&type, src, 2);
    if (type == 1000) {
        swap = 0;
    } else {
        utilSwapINT16((UINT16 *) &type, 1);
        if (type == 1000) {
            swap = 1;
        } else {
            errno = EINVAL;
            return FALSE;
        }
    }

    memcpy(&dest->next, src + 2, 2);
    if (swap) utilSwapINT16((UINT16 *) &dest->next, 1);

    memcpy(&dest->format, src + 4, 1);
    memcpy(&dest->order,  src + 5, 1);
    memcpy(&dest->length, src + 6, 1);

    return TRUE;
}

void lissUnpackFSDH(LISS_FSDH *dest, UINT8 *input)
{
char *src;

    src = (char *) input; /* to shut up Darwin builds */

    sscanf(src, "%06ldD             ", &dest->seqno);

    dest->qcode = src[6];
    strlcpy(dest->staid, src+ 8, LISS_SNAMLEN+1);
    strlcpy(dest->locid, src+13, LISS_LNAMLEN+1);
    strlcpy(dest->chnid, src+15, LISS_CNAMLEN+1);
    strlcpy(dest->netid, src+18, LISS_NNAMLEN+1);

    utilTrimString(dest->staid);
    utilTrimString(dest->chnid);
    utilTrimString(dest->netid);

    dest->start = lissSeedTimeToEpoch((UINT8 *) src+20, &dest->order);
    memcpy(&dest->nsamp,  src+30, 2);
    memcpy(&dest->srfact, src+32, 2);
    memcpy(&dest->srmult, src+34, 2);
    memcpy(&dest->active, src+36, 1);
    memcpy(&dest->ioclck, src+37, 1);
    memcpy(&dest->qual,   src+38, 1);
    memcpy(&dest->more,   src+39, 1);
    memcpy(&dest->tcorr,  src+40, 4);
    memcpy(&dest->bod,    src+44, 2);
    memcpy(&dest->first,  src+46, 2);

    if (dest->order != NATIVE_BYTE_ORDER) {
        dest->swap = TRUE;
        utilSwapINT16((UINT16 *) &dest->nsamp,  1);
        utilSwapINT16((UINT16 *) &dest->srfact, 1);
        utilSwapINT16((UINT16 *) &dest->srmult, 1);
        utilSwapINT32((UINT32 *) &dest->tcorr,  1);
        utilSwapINT16((UINT16 *) &dest->bod,    1);
        utilSwapINT16((UINT16 *) &dest->first,  1);
    } else {
        dest->swap = FALSE;
    }
}

int lissUnpackMiniSeed(LISS_PKT *pkt, UINT8 *src, UINT32 flags)
{
int i;
UINT8 *ptr;
INT32 next;
INT16 type, *sdata;
static UINT8 heartbeat_signature[8] = { 48, 48, 48, 48, 48, 48, 32}  ;

    pkt->srclen = 0;
    pkt->bod = NULL;

    if(memcmp(heartbeat_signature, src, 7) ==0 ) return pkt->status = LISS_HEARTBEAT;

/* Decode it and detect non-data packets */

    lissUnpackFSDH(&pkt->hdr.fsdh, src);

    if (strcasecmp(pkt->hdr.fsdh.chnid, "LOG") == 0) return pkt->status = LISS_NOT_DATA;

    if (pkt->hdr.fsdh.srfact == 0) return pkt->status = LISS_NOT_DATA;
    if (pkt->hdr.fsdh.srmult == 0) return pkt->status = LISS_NOT_DATA;
    if (pkt->hdr.fsdh.nsamp   < 1) return pkt->status = LISS_NOT_DATA;

/* Look for blockette 1000 */

    next = pkt->hdr.fsdh.first;
    do {
        ptr  = src + next;
        type = lissTypeAndOffset(ptr, &next, pkt->hdr.fsdh.swap);
    } while (next > 0 && type != 1000);

    if (type != 1000) return pkt->status = LISS_NOT_MINISEED;

/* Decode blockette 1000 */

    if (!lissUnpackB1000(&pkt->hdr.b1000, ptr)) return pkt->status = LISS_NONSENSE_PKT;

/* Fill in the derived fields */

    pkt->srclen = (INT32) (pow(2.0, (double) pkt->hdr.b1000.length)) - pkt->hdr.fsdh.bod;
    pkt->order = pkt->hdr.b1000.order ? BIG_ENDIAN_BYTE_ORDER : LTL_ENDIAN_BYTE_ORDER;
    pkt->bod = src + pkt->hdr.fsdh.bod;
    pkt->flags = flags;

/* Ignore nonsense packets */

    if (pkt->hdr.b1000.format == 0) return pkt->status = LISS_NONSENSE_PKT;
    
/* Decompress/reorder if asked */

    if (pkt->flags & LISS_OPTION_DECODE) {

        switch (pkt->hdr.b1000.format) {

          case LISS_STEIM1:
            pkt->status = lissDecompressSteim1(pkt->data, LISS_MAX_BLKSIZ, pkt->bod, pkt->srclen, pkt->order, pkt->hdr.fsdh.nsamp);
            break;

          case LISS_STEIM2:
            pkt->status = lissDecompressSteim2(pkt->data, LISS_MAX_BLKSIZ, pkt->bod, pkt->srclen, pkt->order, pkt->hdr.fsdh.nsamp);
            break;

          case LISS_INT_32:
            memcpy(pkt->data, (INT32 *) pkt->bod, pkt->hdr.fsdh.nsamp);
            if (pkt->hdr.fsdh.swap) utilSwapINT32((UINT32 *) pkt->data, pkt->hdr.fsdh.nsamp);
            pkt->status = LISS_PKT_OK;
            break;

          case LISS_INT_16:
            sdata = (INT16 *) pkt->bod;
            for (i = 0; i < pkt->hdr.fsdh.nsamp; i++) pkt->data[i] = (INT32) sdata[i];
            if (pkt->hdr.fsdh.swap) utilSwapINT32((UINT32 *) pkt->data, pkt->hdr.fsdh.nsamp);
            pkt->status = LISS_PKT_OK;
            break;

          default:
            pkt->status = LISS_UNSUPPORTED;
        }
    } else {
        pkt->status = LISS_PKT_OK;
    }

    return pkt->status;
}

int lissPackMseedHdr(UINT8 *dest, LISS_MSEED_HDR *mseed)
{
static char space = ' ';
int intYR, intDA, intHR, intMN, intSC, intMS;
char tmptxt[LISS_SNAMLEN+LISS_CNAMLEN+LISS_LNAMLEN+1];
UINT8 *ptr;
    
/* Pack Fixed Section of Data Header */

    ptr = dest;
    sprintf(tmptxt, "%06lu", mseed->fsdh.seqno % LISS_FSDH_SEQNO_MODULO);
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt)); /* Sequence number */
    *ptr++ = mseed->fsdh.qcode;                                  /* Data header/quality indicator */
    *ptr++ = space;                                              /* Reserved byte */
    sprintf(tmptxt, "%-5s", mseed->fsdh.staid); utilPadString(tmptxt, LISS_SNAMLEN, ' ');
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt)); /* Station identifier code */
    sprintf(tmptxt, "%-2s", mseed->fsdh.locid); utilPadString(tmptxt, LISS_LNAMLEN, ' ');
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt)); /* Location identifier */
    sprintf(tmptxt, "%-3s", mseed->fsdh.chnid); utilPadString(tmptxt, LISS_CNAMLEN, ' ');
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt)); /* Channel identifier */
    sprintf(tmptxt, "%-2s", mseed->fsdh.netid); utilPadString(tmptxt, LISS_NNAMLEN, ' ');
    ptr += utilPackBytes(ptr, (UINT8 *) tmptxt, strlen(tmptxt)); /* Network code */
    util_tsplit(mseed->fsdh.start, &intYR, &intDA, &intHR, &intMN, &intSC, &intMS);
    ptr += utilPackUINT16(ptr, (UINT16) intYR);                  /* Record start time */
    ptr += utilPackUINT16(ptr, (UINT16) intDA);
    *ptr++ = (UINT8) intHR;
    *ptr++ = (UINT8) intMN;
    *ptr++ = (UINT8) intSC;
    *ptr++ = 0;
    ptr += utilPackUINT16(ptr, (UINT16) intMS);
    ptr += utilPackUINT16(ptr, (UINT16) mseed->fsdh.nsamp);      /* Number of samples */
    ptr += utilPackUINT16(ptr, (UINT16) mseed->fsdh.srfact);     /* Sample rate factor */
    ptr += utilPackUINT16(ptr, (UINT16) mseed->fsdh.srmult);     /* Sample rate multiplier */
    *ptr++ = mseed->fsdh.active;                                 /* Activity flags */
    *ptr++ = mseed->fsdh.ioclck;                                 /* I/O and clock flags */
    *ptr++ = mseed->fsdh.qual;                                   /* Data quality flags */
    *ptr++ = 1;                                                  /* Number of blockettes that follow */
    ptr += utilPackUINT32(ptr, mseed->fsdh.tcorr);               /* Time correction */
    ptr += utilPackINT16(ptr, mseed->fsdh.bod);                  /* Beginning of data */
    ptr += utilPackUINT16(ptr, (UINT16) (ptr - dest) + 2);       /* First blockette */

/* Pack Blockette 1000 */

    ptr += utilPackUINT16(ptr, 1000);                            /* Blockette type - 1000 */
    ptr += utilPackUINT16(ptr, 0);                               /* Next blockette's byte number */
    *ptr++ = mseed->b1000.format;                                /* Encoding Format */
    *ptr++ = mseed->b1000.order;                                 /* Word order */
    *ptr++ = mseed->b1000.length;                                /* Data Record length */
    *ptr++ = 0;                                                  /* reserved */

/* return length of packed header */

    return (int) (ptr - dest);
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.9  2015/12/04 22:18:12  dechavez
 * fixed format strings to calm OS X compiles
 *
 * Revision 1.8  2011/11/09 23:10:32  dechavez
 * pad short strings with spaces in lissPackMseedHdr()
 *
 * Revision 1.7  2011/11/03 17:43:10  dechavez
 * adjust for fsdh and b1000 moving into LISS_MSEED_HDR hdr field
 *
 * Revision 1.6  2011/10/28 21:46:03  dechavez
 * added lissPackMseedHdr()
 *
 * Revision 1.5  2009/11/19 19:24:47  dechavez
 * LISS_HEARTBEAT support (aap)
 *
 * Revision 1.4  2008/02/03 21:13:36  dechavez
 * lissUnpackMiniSeed() made smarter about decoding non-data packets, return
 * value switched from BOOL to decode status
 *
 * Revision 1.3  2008/01/07 21:52:31  dechavez
 * added selection tests and optional data decoding (brought over from lissRead())
 *
 * Revision 1.2  2007/10/31 17:08:28  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
