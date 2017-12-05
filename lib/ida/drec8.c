#pragma ident "$Id: drec8.c,v 1.2 2012/02/14 20:15:23 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 8 data record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define DATA_OFFSET 60

int drec_rev8(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab)
{
int retval;

/*  Load the header  */

    if ((retval = dhead_rev8(ida, &dest->head, src)) != 0) return retval;

/*  Load the data  */

    if (dest->head.nbytes > IDA_MAXDLEN) return -201;
    memcpy(dest->data, src + DATA_OFFSET, dest->head.nbytes);

    if (swab) {
        if (dest->head.form == S_UNCOMP) {
            SSWAB((INT16 *) dest->data, (INT16) dest->head.nbytes/2);
        } else if (dest->head.form == L_UNCOMP) {
            LSWAB((INT32 *) dest->data, (INT32) dest->head.nbytes/4);
        } else if (dest->head.form == FP32_UNCOMP) {
            LSWAB((INT32 *) dest->data, (INT32) dest->head.nbytes/4);
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: drec8.c,v $
 * Revision 1.2  2012/02/14 20:15:23  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
