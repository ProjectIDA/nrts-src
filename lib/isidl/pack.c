#pragma ident "$Id: pack.c,v 1.2 2015/12/04 22:14:50 dechavez Exp $"
/*======================================================================
 *
 *  Pack/Unpack ISI DL data objects into/out of NBO
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"

static int PackIndex(UINT8 *start, ISI_INDEX *src)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackUINT32(ptr, src->active);
    ptr += utilPackUINT32(ptr, src->oldest);
    ptr += utilPackUINT32(ptr, src->yngest);
    ptr += utilPackUINT32(ptr, src->lend);

    return (int) (ptr - start);
}

static int UnpackIndex(UINT8 *start, ISI_INDEX *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->active);
    ptr += utilUnpackUINT32(ptr, &dest->oldest);
    ptr += utilUnpackUINT32(ptr, &dest->yngest);
    ptr += utilUnpackUINT32(ptr, &dest->lend);

    return (int) (ptr - start);
}

int isidlPackDLSys(UINT8 *start, ISI_DL_SYS *src)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackBytes(ptr, (UINT8 *) src->site, ISI_SITELEN);
    ptr += isiPackSeqno(ptr, &src->seqno);
    ptr += utilPackUINT32(ptr, src->numpkt);
    ptr += utilPackUINT32(ptr, src->nhide);
    ptr += utilPackUINT32(ptr, src->maxlen);
    ptr += PackIndex(ptr, &src->index);
    ptr += PackIndex(ptr, &src->backup);
    ptr += utilPackUINT32(ptr, (UINT32) src->parent);
    ptr += utilPackUINT16(ptr, (UINT16) src->state);
    ptr += utilPackUINT64(ptr, src->count);
    ptr += utilPackUINT64(ptr, src->tstamp.start);
    ptr += utilPackUINT64(ptr, src->tstamp.write);

    return (int) (ptr - start);
}

int isidlUnpackDLSys(UINT8 *start, ISI_DL_SYS *dest)
{
UINT32 tmp32;
UINT16 tmp16;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->site, ISI_SITELEN); dest->site[ISI_SITELEN] = 0;
    ptr += isiUnpackSeqno(ptr, &dest->seqno);
    ptr += utilUnpackUINT32(ptr, &dest->numpkt);
    ptr += utilUnpackUINT32(ptr, &dest->nhide);
    ptr += utilUnpackUINT32(ptr, &dest->maxlen);
    ptr += UnpackIndex(ptr, &dest->index);
    ptr += UnpackIndex(ptr, &dest->backup);
    ptr += utilUnpackUINT32(ptr, &tmp32); dest->parent = (pid_t) tmp32;
    ptr += utilUnpackUINT16(ptr, &tmp16); dest->state = (int) tmp16;
    ptr += utilUnpackUINT64(ptr, &dest->count);
    ptr += utilUnpackINT64(ptr, &dest->tstamp.start);
    ptr += utilUnpackINT64(ptr, &dest->tstamp.write);

    return (int) (ptr - start);
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.2  2015/12/04 22:14:50  dechavez
 * utilUnpackINT64() instead of utilUnpackUINT64() where needed (benign, calms OS X)
 *
 * Revision 1.1  2010/08/27 18:48:35  dechavez
 * initial release
 *
 */
