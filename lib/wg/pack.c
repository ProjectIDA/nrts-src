#pragma ident "$Id: pack.c,v 1.2 2015/12/04 22:31:53 dechavez Exp $"
/*======================================================================
 *
 *  Pack/Unpack WG data objects into/out of NBO
 *
 *====================================================================*/
#include "wg.h"
#include "util.h"

int wgPackSeqno(UINT8 *start, WG_OBSPKT_SEQNO *seqno)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT16(ptr, seqno->valid);
    ptr += utilPackUINT16(ptr, seqno->value);

    return (int) (ptr - start);

}

int wgUnpackSeqno(UINT8 *start, WG_OBSPKT_SEQNO *seqno)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &seqno->valid);
    ptr += utilUnpackUINT16(ptr, &seqno->value);

    return (int) (ptr - start);

}

int wgUnpackWGID(UINT8 *start, char *dest, int len)
{
UINT8 *ptr;

    if (len > WG_STALEN) len = WG_STALEN;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest, len);
    dest[len] = 0;

    return (int) (ptr - start);
}

int wgPackWGOBS1(UINT8 *start, WG_WGOBS1 *src)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT16(ptr, src->seqno);
    ptr += utilPackUINT16(ptr, src->len);
    ptr += utilPackBytes(ptr, src->payload, src->len);

    return (int) (ptr - start);
}

int wgUnpackWGOBS1(UINT8 *start, WG_WGOBS1 *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->seqno);
    ptr += utilUnpackUINT16(ptr, &dest->len);
    dest->payload = ptr;

    return (int) (ptr - start);
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.2  2015/12/04 22:31:53  dechavez
 * casts, format fixes and benign bug fixes to calm OS X compiles
 *
 * Revision 1.1  2012/07/03 16:07:22  dechavez
 * initial release
 *
 */
