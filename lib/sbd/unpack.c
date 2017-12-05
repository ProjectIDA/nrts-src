#pragma ident "$Id: unpack.c,v 1.6 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 * 
 * Unpack SBD Information Elements
 *
 *====================================================================*/
#include "sbd.h"

int sbdUnpackMO_HEADER(UINT8 *start, SBD_MO_HEADER *dest)
{
UINT8 *ptr, *tmp;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 28) {
        ptr += len;
    } else {
        ptr += utilUnpackUINT32(ptr, &dest->cdr);
        ptr += utilUnpackBytes(ptr, (UINT8 *) dest->imei, 15); dest->imei[15] = 0;
        dest->status = *ptr++;
        ptr += utilUnpackUINT16(ptr, &dest->momsn);
        ptr += utilUnpackUINT16(ptr, &dest->mtmsn);
        ptr += utilUnpackUINT32(ptr, &dest->tstamp);
        dest->valid = TRUE;
    }
    
    return (int) (ptr - start);
}

int sbdUnpackMO_LOCATION(UINT8 *start, SBD_MO_LOCATION *dest)
{
int skiplen;
REAL32 sign;
UINT8 *ptr, format, latlon;
UINT16 len, minutes;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 11) {
        ptr += len;
        return (int) (ptr - start);
    }
    format = *ptr++;
    if ((format & SBD_MO_LOCATION_MASK_FORMAT) != 0) { /* unrecognized format */
        ptr += 10;
        return (int) (ptr - start);
    }

    /* read/compute latitude */

    sign = (format & SBD_MO_LOCATION_MASK_NSI) ? -1.0 : 1.0;
    latlon = *ptr++;
    ptr += utilUnpackUINT16(ptr, &minutes); /* in thousandths */
    dest->lat = sign * ((REAL32) latlon + (REAL32) minutes / 60000);

    /* read/compute longitude */

    sign = (format & SBD_MO_LOCATION_MASK_EWI) ? -1.0 : 1.0;
    latlon = *ptr++;
    ptr += utilUnpackUINT16(ptr, &minutes); /* in thousandths */
    dest->lon = sign * ((REAL32) latlon + (REAL32) minutes / 60000);
    
    /* read circular error probablility */

    ptr += utilUnpackUINT32(ptr, &dest->cep);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

int sbdUnpackMO_CONFIRM(UINT8 *start, SBD_MO_CONFIRM *dest)
{
UINT8 *ptr, status;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 1) {
        ptr += len;
        return (int) (ptr - start);
    }
    dest->status = *ptr++ ? TRUE : FALSE;
    
    dest->valid = TRUE;
    return (int) (ptr - start);
}


int sbdUnpackMT_HEADER(UINT8 *start, SBD_MT_HEADER *dest)
{
UINT8 *ptr;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 21) {
        ptr += len;
        return (int) (ptr - start);
    }
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->myid,  4); dest->myid[ 4] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->imei, 15); dest->imei[15] = 0;
    ptr += utilUnpackUINT16(ptr, &dest->flags);
    
    dest->valid = TRUE;
    return (int) (ptr - start);
}

int sbdUnpackMT_CONFIRM(UINT8 *start, SBD_MT_CONFIRM *dest)
{
UINT8 *ptr;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 25) {
        ptr += len;
        return (int) (ptr - start);
    }
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->myid,  4); dest->myid[ 4] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->imei, 15); dest->imei[15] = 0;
    ptr += utilUnpackUINT32(ptr, &dest->cdr);
    ptr += utilUnpackUINT16(ptr, &dest->status);
    
    dest->valid = TRUE;
    return (int) (ptr - start);
}

int sbdUnpackMT_PRIORITY(UINT8 *start, SBD_MT_PRIORITY *dest)
{
UINT8 *ptr;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &len);
    if (len != 2) {
        ptr += len;
        return (int) (ptr - start);
    }
    ptr += utilUnpackUINT16(ptr, &dest->level);
    
    dest->valid = TRUE;
    return (int) (ptr - start);
}

int sbdUnpackPayload(UINT8 *start, SBD_PAYLOAD *dest)
{
UINT8 *ptr;
UINT16 len;

    dest->valid = FALSE; /* assume failure */

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->len);
    if (dest->len > SBD_MAX_MSGLEN) {
        ptr += len;
        return (int) (ptr - start);
    }
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->data, dest->len);
    
    dest->valid = TRUE;
    return (int) (ptr - start);
}

/* Revision History
 *
 * $Log: unpack.c,v $
 * Revision 1.6  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.5  2014/01/29 23:54:30  dechavez
 * rework handling of unexpected lengths (to avoid Linux build complaints)
 *
 * Revision 1.4  2013/03/15 21:35:43  dechavez
 * usee SBD_MO_LOCATION_MASK_x from sbd.h, changed length in sbdUnpackMO_LOCATION()
 * from 20 (what the manual says) to 11 (what it really is)
 *
 * Revision 1.3  2013/03/13 21:29:03  dechavez
 * added sbdUnpackMO_LOCATION(), sbdUnpackMO_CONFIRM(), sbdUnpackMT_PRIORITY()
 *
 * Revision 1.2  2013/03/11 22:57:43  dechavez
 * added sbdUnpackMT_CONFIRM()
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
