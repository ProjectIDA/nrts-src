#pragma ident "$Id: util.c,v 1.3 2015/09/30 20:25:46 dechavez Exp $"
/*======================================================================
 *
 *  miscellaneous utilities
 *
 *====================================================================*/
#include "podir.h"

INT64 ConvertDccdbBegtTo1999Nsec(REAL64 tstamp)
{
INT64 result;

    if (tstamp == DCCDB_NULL_BEGT) {
        result = (INT64) 0;
    } else {
        result = utilConvertFrom1970SecsTo1999Nsec(tstamp);
    }

    return result;
}

INT64 ConvertDccdbEndtTo1999Nsec(REAL64 tstamp)
{
INT64 result;

    if (tstamp == DCCDB_NULL_ENDT) {
        result = MSEED_NEVER;
    } else {
        result = utilConvertFrom1970SecsTo1999Nsec(tstamp);
    }

    return result;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: util.c,v $
 * Revision 1.3  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
