#pragma ident "$Id: util.c,v 1.1 2016/04/28 23:01:50 dechavez Exp $"
/*======================================================================
 *
 *  Various common convenience functions
 *
 *====================================================================*/
#include "spm10.h"

void BuildIdentString(char *dest, char *nname, char *sname, char *cname)
{
static char *hash = "#";

    if (nname == NULL) nname = hash;
    if (sname == NULL) sname = hash;
    if (cname == NULL) cname = hash;

    sprintf(dest, "%s-%s-%s", nname, sname, cname);
}

void BuildGentag(IDA10_GENTAG *dest, TIMESTAMP *tstamp)
{
int doy, sec, msec;
REAL64 dtime;

    if (tstamp->month == -1 && tstamp->day == -1) {
        doy = tstamp->jday;
    } else {
        doy = utilYmdtojd(tstamp->year, tstamp->month, tstamp->day);
    }
    sec = (int) tstamp->second;
    msec = (int) ((tstamp->second - (float) sec) * 1000.0);
    dtime = utilYdhmsmtod(tstamp->year, doy, tstamp->hour, tstamp->minute, sec, msec);
    dtime -= SAN_EPOCH_TO_1970_EPOCH; /* convert secs since 1/1/1970 to secs since 1/1/1999 */
    dest->tstamp = (UINT64) (dtime * (REAL64) NANOSEC_PER_SEC);
    dest->status.receiver = dest->status.generic = tstamp->status;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * Revision 1.1  2016/04/28 23:01:50  dechavez
 * initial release
 *
 */
