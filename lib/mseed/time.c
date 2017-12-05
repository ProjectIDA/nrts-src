#pragma ident "$Id: time.c,v 1.2 2015/06/30 19:14:08 dechavez Exp $"
/*======================================================================
 * 
 * TIME related utilities
 *
 *====================================================================*/
#include "mseed.h"
#include "sanio.h"

char *mseedSetTIMEstring(char *dest, INT64 tstamp, BOOL full)
{
REAL64 epochtime;
int year, day, hr, mn, sc, msc;

/* Look for special "never" case */

    if (tstamp == MSEED_NEVER) {
        dest[0] = 0;
        return dest;
    }

/* Convert from nanoseconds since 1999 to seconds since 1970 */

    epochtime = (tstamp + (SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC)) / (REAL64) NANOSEC_PER_SEC;

/* Split into components */

    utilTsplit(epochtime, &year, &day, &hr, &mn, &sc, &msc);

/* Build the time string (we assume it is beg enough (MSEED_TIMELEN)) */

    if (full || msc != 0) {
        sprintf(dest, "%04d,%03d,%02d:%02d:%02d.%03d", year, day, hr, mn, sc, msc);
    } else if (sc != 0) {
        sprintf(dest, "%04d,%03d,%02d:%02d:%02d", year, day, hr, mn, sc);
    } else if (mn != 0) {
        sprintf(dest, "%04d,%03d,%02d:%02d", year, day, hr, mn);
    } else if (hr != 0) {
        sprintf(dest, "%04d,%03d,%02d", year, day, hr);
    } else if (day != 0) {
        sprintf(dest, "%04d,%03d", year, day);
    } else {
        sprintf(dest, "%04d", year);
    }

    return dest;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: time.c,v $
 * Revision 1.2  2015/06/30 19:14:08  dechavez
 * added "BOOL full" arg to mseedSetTIMEstring
 *
 * Revision 1.1  2014/10/29 21:27:12  dechavez
 * created
 *
 */
