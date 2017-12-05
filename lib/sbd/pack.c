#pragma ident "$Id: pack.c,v 1.5 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 * 
 * Pack SBD Information Elements
 *
 *====================================================================*/
#include "sbd.h"

int sbdPackMO_HEADER(UINT8 *start, SBD_MO_HEADER *src)
{
UINT8 *ptr;
UINT16 len = 28;

    if (start == NULL || src == NULL || !src->valid) return 0;

    ptr = start;
    *ptr++ = SBD_IEI_MO_HEADER;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackUINT32(ptr, src->cdr);
    ptr += utilPackBytes(ptr, (UINT8 *) src->imei, 15);
    *ptr++ = src->status;
    ptr += utilPackUINT16(ptr, src->momsn);
    ptr += utilPackUINT16(ptr, src->mtmsn);
    ptr += utilPackUINT32(ptr, src->tstamp);

    return (int) (ptr - start);

}

static void SplitCoordinate(REAL32 coord, INT8 *deg, INT16 *min)
{
REAL32 degrees, fraction;
       
    if (coord < 0.0) coord = -coord;
    degrees = (REAL32) ((int) coord);
    fraction = coord - degrees;

    *deg = (UINT8) degrees;
    *min = (UINT16) (fraction * 60000.0);
}

int sbdPackMO_LOCATION(UINT8 *start, SBD_MO_LOCATION *src)
{
UINT8 *ptr, format;
INT8 deg;
INT16 min;
UINT16 len = 11;

    if (start == NULL || src == NULL || !src->valid) return 0;

    format = 0;
    if (src->lat < 0.0) format |= SBD_MO_LOCATION_MASK_NSI;
    if (src->lon < 0.0) format |= SBD_MO_LOCATION_MASK_EWI;

    ptr = start;
    *ptr++ = SBD_IEI_MO_LOCATION;
    ptr += utilPackUINT16(ptr, len);
    *ptr++ = format;

    SplitCoordinate(src->lat, &deg, &min);
    *ptr++ = deg;
    ptr += utilPackUINT16(ptr, min);

    SplitCoordinate(src->lon, &deg, &min);
    *ptr++ = deg;
    ptr += utilPackUINT16(ptr, min);

    ptr += utilPackUINT32(ptr, src->cep);

    return (int) (ptr - start);

}

int sbdPackMO_CONFIRM(UINT8 *start, SBD_MO_CONFIRM *src)
{
UINT8 *ptr;
UINT16 len = 1;

    if (start == NULL || src == NULL || !src->valid) return 0;
    ptr = start;
    *ptr++ = SBD_IEI_MO_CONFIRM;
    ptr += utilPackUINT16(ptr, len);
    *ptr++ = src->status ? 1 : 0;

    return (int) (ptr - start);

}

int sbdPackMT_HEADER(UINT8 *start, SBD_MT_HEADER *src)
{
UINT8 *ptr;
UINT16 len = 21;

    if (start == NULL || src == NULL || !src->valid) return 0;

    ptr = start;
    *ptr++ = SBD_IEI_MT_HEADER;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackBytes(ptr, (UINT8 *) src->myid, 4);
    ptr += utilPackBytes(ptr, (UINT8 *) src->imei, 15);
    ptr += utilPackUINT16(ptr, src->flags);

    return (int) (ptr - start);

}

int sbdPackMT_CONFIRM(UINT8 *start, SBD_MT_CONFIRM *src)
{
UINT8 *ptr;
UINT16 len = 25;

    if (start == NULL || src == NULL || !src->valid) return 0;

    ptr = start;
    *ptr++ = SBD_IEI_MT_CONFIRM;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackBytes(ptr, (UINT8 *) src->myid, 4);
    ptr += utilPackBytes(ptr, (UINT8 *) src->imei, 15);
    ptr += utilPackUINT32(ptr, src->cdr);
    ptr += utilPackUINT16(ptr, src->status);

    return (int) (ptr - start);

}

int sbdPackMT_PRIORITY(UINT8 *start, SBD_MT_PRIORITY *src)
{
UINT8 *ptr;
UINT16 len = 2;

    if (start == NULL || src == NULL || !src->valid) return 0;
    ptr = start;
    *ptr++ = SBD_IEI_MT_PRIORITY;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackUINT16(ptr, src->level);

    return (int) (ptr - start);

}

int sbdPackPayload(UINT8 *start, SBD_PAYLOAD *src, int which)
{
UINT8 *ptr;
UINT16 len = 21;

    if (start == NULL || src == NULL || !src->valid) return 0;

    ptr = start;
    *ptr++ = which;
    len = (src->len <= SBD_MAX_MSGLEN) ? src->len : SBD_MAX_MSGLEN;
    ptr += utilPackUINT16(ptr, len);
    ptr += utilPackBytes(ptr, src->data, len);
    
    return (int) (ptr - start);
}

int sbdPackMessage(SBD_MESSAGE *message)
{
UINT8 *ptr, *lenptr;

    if (message == NULL) {
        errno = EINVAL;
        return -1;
    }

/* Pack the approriate header, and payload if applicable */

    ptr = message->body;
    *ptr++ = 1; /* always protocol 1 */
    lenptr = ptr; ptr += 2; /* skip over length field */
    if (message->mo.header.valid) {
        ptr += sbdPackMO_HEADER(ptr, &message->mo.header);
        if (message->mo.location.valid)  ptr += sbdPackMO_LOCATION(ptr, &message->mo.location);
        if (message->mo.confirm.valid)   ptr += sbdPackMO_CONFIRM(ptr, &message->mo.confirm);
        if (message->mo.payload.valid)   ptr += sbdPackPayload(ptr, &message->mo.payload, SBD_IEI_MO_PAYLOAD);
    }
    if (message->mt.header.valid) {
        ptr += sbdPackMT_HEADER(ptr, &message->mt.header);
        if (message->mt.payload.valid)   ptr += sbdPackPayload(ptr, &message->mt.payload, SBD_IEI_MT_PAYLOAD);
    }
    if (message->mt.confirm.valid) ptr += sbdPackMT_CONFIRM(ptr, &message->mt.confirm);

    message->len = (UINT16) (ptr - message->body); /* full body, including preamble */
    utilPackUINT16(lenptr, message->len - SBD_PROTOCOL_1_PREAMBLE_LEN); /* length that is sent does not include preamble */

    return (int) message->len; /* total length, including preamble */
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.5  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.4  2013/03/15 21:39:20  dechavez
 * added sbdPackMO_LOCATION(), sbdPackMO_CONFIRM(), sbdPackMT_PRIORITY()
 *
 * Revision 1.3  2013/03/13 21:26:11  dechavez
 * accomodate separate MO and MT payloads
 *
 * Revision 1.2  2013/03/11 23:00:11  dechavez
 * added SBD_MT_CONFIRM suppor
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
