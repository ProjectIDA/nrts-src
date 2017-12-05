#pragma ident "$Id: drec.c,v 1.2 2012/02/14 20:15:49 dechavez Exp $"
/*======================================================================
 *
 *  Copy from raw buffer to data record structure.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int ida_drec(IDA *ida, IDA_DREC *dest, UINT8 *src, int shift, int swab)
{
int i, status;
INT16 *sdata;
INT32 *ldata;

    dest->head.rev = ida->rev.value;
    dest->head.ffu_shift = 0; /* rev specific routines modify as required */

    if (ida->rev.value == 9) {
        status = drec_rev9(ida, dest, src, swab);
    } else if (ida->rev.value == 8) {
        status = drec_rev8(ida, dest, src, swab);
    } else if (ida->rev.value == 7) {
        status = drec_rev7(ida, dest, src, swab);
    } else if (ida->rev.value == 6) {
        status = drec_rev6(ida, dest, src, swab);
    } else if (ida->rev.value == 5) {
        status = drec_rev5(ida, dest, src, swab);
    } else if (ida->rev.value == 4) {
        status = drec_rev4(ida, dest, src, swab);
    } else if (ida->rev.value == 3) {
        status = drec_rev3(ida, dest, src, swab);
    } else if (ida->rev.value == 2) {
        status = drec_rev2(ida, dest, src, swab);
    } else if (ida->rev.value == 1) {
        status = drec_rev1(ida, dest, src, swab);
    } else {
        errno = ENOTSUP;
        status = -401;
    }

    if (status != 0) return status;

/*  If requested, apply that stupid Fels' induced shift  */

    if (shift && dest->head.ffu_shift) {
        ldata = (INT32 *) dest->data;
        for (i = 0; i < dest->head.nsamp; i++) {
            ldata[i] >>= dest->head.ffu_shift;
        }
    }

/*  Set last word for uncompressed records  */

    if (ida_sampok(&dest->head)) {
        if (dest->head.form == S_UNCOMP) {
            sdata = (INT16 *) dest->data;
            dest->head.last_word = (INT32) sdata[dest->head.nsamp - 1];
        } else if (dest->head.form == L_UNCOMP) {
            ldata = (INT32 *) dest->data;
            dest->head.last_word = ldata[dest->head.nsamp - 1];
        }
        LSWAB(&dest->head.last_word, 1);
    }

    return status;
}

/* Revision History
 *
 * $Log: drec.c,v $
 * Revision 1.2  2012/02/14 20:15:49  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
