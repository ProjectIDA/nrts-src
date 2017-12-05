#pragma ident "$Id: extract.c,v 1.2 2014/04/28 21:39:34 dechavez Exp $"
/*======================================================================
 * 
 * Unpack all the sensor data in a single SBD message into a linked list
 * of ADDOSS_SENSOR_DATA_INT24 structures, sorted by time.  This is to
 * try to reduce the false time tears that occur when previously missed
 * frames are received.
 *
 * Returns a pointer to a LNKLST linked list, or NULL if problems.
 *
 * ALLOCATES MEMORY!  The calling process must listDestroy() the return
 * value when done with the data in order to avoid memory leaks.
 *
 *====================================================================*/
#include "addoss.h"

static BOOL AddFrame(LNKLST *list, UINT8 *frame)
{
LNKLST_NODE *crnt;
ADDOSS_SENSOR_DATA_INT24 new, *this;

/* Unpack the frame */

    addossUnpackSensorDataInt24(frame, &new);

/* Insert it into the list before any older frames */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        this = (ADDOSS_SENSOR_DATA_INT24 *) crnt->payload;
        if (new.tstamp < this->tstamp) {
            if (listInsertBefore(crnt, &new, sizeof(ADDOSS_SENSOR_DATA_INT24)) == NULL) {
                return FALSE;
            } else {
                return TRUE;
            }
        }
        crnt = listNextNode(crnt);
    }

/* If we got here, then that means there were no older frames
 * in the list so we just tack the new one onto the end.
 */

    return listAppend(list, &new, sizeof(ADDOSS_SENSOR_DATA_INT24));
}

LNKLST *addossExtractSensorDataInt24(SBD_MESSAGE *message)
{
UINT8 *ptr;
UINT16 type;
LNKLST *list;

/* verify input */

    if (message == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!message->mo.header.valid || !message->mo.payload.valid) {
        errno = EINVAL;
        return NULL;
    }

    if (message->mo.payload.len % ADDOSS_FRAME_LEN) {
        errno = EINVAL;
        return NULL;
    }

/* At this point the input message is verified valid for ADDOSS */

    if ((list = listCreate()) == NULL) return NULL;

/* Empty payloads are OK (WG mailbox checks) */

    if (message->mo.payload.len == 0) return list;

/* Process each ADDOSS frame in the message */

    ptr = message->mo.payload.data;
    while (ptr < message->mo.payload.data + message->mo.payload.len) {
        ptr += utilUnkcapUINT16(ptr, &type);
        if (type == ADDOSS_ID_SENSOR_DATA_INT24 && !AddFrame(list, ptr)) {
            listDestroy(list);
            return NULL;
        }
        ptr += ADDOSS_FRAME_PAYLOAD_LEN; /* all frames have the same payload length */
    }

/* All done, return the list */

    return list;
}

/* Revision History
 *
 * $Log: extract.c,v $
 * Revision 1.2  2014/04/28 21:39:34  dechavez
 * fixed return typo in addossExtractSensorDataInt24() when failing on listCreate()
 *
 * Revision 1.1  2014/01/27 17:24:28  dechavez
 * created
 *
 */
