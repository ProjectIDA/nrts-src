#pragma ident "$Id: read.c,v 1.3 2016/06/06 21:47:12 dechavez Exp $"
/*======================================================================
 *
 *  Read one item
 *
 *====================================================================*/
#include "tristar.h"

int tristarReadItem(TS *handle, TS_DESC *desc, TS_ITEM *item)
{
UINT16 lo, hi;
static char *fid = "tristarReadItem";

    if (handle == NULL || desc == NULL || item == NULL) {
        errno = EINVAL;
        return TS_READ_EINVAL;
    }

    item->valid = FALSE;

/* All items have a low order address */

    if (modbus_read_registers(handle->ctx, desc->addr.lo, 1, &lo) == -1) return TS_READ_FAIL;

/* 32-bit items need a second read to get the high order word */

    if (desc->addr.hi != TS_XXX) {
        if (modbus_read_registers(handle->ctx, desc->addr.hi, 1, &hi) == -1) return TS_READ_FAIL;
        item->raw = (hi << 16) | lo;
    } else {
        item->raw = (lo << 16) >> 16;
    }

/* Apply the scale factor to convert to real units */

    item->value = (REAL64) item->raw * desc->scale;

/* Apply the current and voltage scale factors as well (if applicable) */

    switch (desc->pu) {
      case TS_PU_V: item->value *= handle->V_PU; break;
      case TS_PU_I: item->value *= handle->I_PU; break;
      case TS_PU_B: item->value *= handle->V_PU * handle->I_PU; break;
    }

/* All done */

    item->valid = TRUE;
    return TS_READ_OK;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.3  2016/06/06 21:47:12  dechavez
 * completely rewritten, now with just tristarReadItem()
 *
 * Revision 1.2  2012/05/02 18:22:04  dechavez
 * set valid field in tristarReadRegister(), changed tristarReadRam() to void
 * now that valid field is used, added tristarReadRamLogset()
 *
 * Revision 1.1  2012/04/25 21:04:13  dechavez
 * initial release
 *
 */
