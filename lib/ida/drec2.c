#pragma ident "$Id: drec2.c,v 1.2 2012/02/14 20:15:22 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 2 data record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define DATA_OFFSET 64

int drec_rev2(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab)
{
int retval;

/*  Load the header  */

    if ((retval = dhead_rev2(ida, &dest->head, src)) != 0) return retval;

/*  Load the data  */

    if (dest->head.nbytes > IDA_MAXDLEN) return -201;
    memcpy(dest->data, src + DATA_OFFSET, dest->head.nbytes);

    if (swab) {
        if (dest->head.form == S_UNCOMP) {
            SSWAB((INT16 *) dest->data, dest->head.nsamp);
        } else if (dest->head.form == L_UNCOMP) {
            LSWAB((INT32 *) dest->data, dest->head.nsamp);
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: drec2.c,v $
 * Revision 1.2  2012/02/14 20:15:22  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
