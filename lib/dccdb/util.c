#pragma ident "$Id: util.c,v 1.2 2015/07/10 18:18:59 dechavez Exp $"
/*======================================================================
 *
 *  Misc utilities
 *
 *====================================================================*/
#include "dccdb.h"

#ifndef IsLeapYear
#define IsLeapYear(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif
 
#ifndef DaysInYear 
#define DaysInYear(i) (365 + IsLeapYear(i))
#endif

REAL64 dccdbOndateToBegt(INT32 yearday)
{
REAL64 rsec;
int year, doy;

    if (yearday == DCCDB_NULL_YEARDAY) return DCCDB_NULL_BEGT;

    year = yearday / 1000; 
    doy  = yearday - (year * 1000);

    rsec = utilYdhmsmtod(year, doy, 0, 0, 0, 0);

    return rsec;
}

REAL64 dccdbOffdateToEndt(INT32 yearday)
{
REAL64 rsec;
int year, doy;

    if (yearday == DCCDB_NULL_YEARDAY) return DCCDB_NULL_ENDT;

    year = yearday / 1000; 
    doy  = yearday - (year * 1000);

    if (doy != DaysInYear(year)) {
        --doy;
    } else {
        doy = 1;
        --year;
    }

    rsec = utilYdhmsmtod(year, doy, 23, 59, 59, 900);

    return rsec;
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
 * Revision 1.2  2015/07/10 18:18:59  dechavez
 * replaced long with INT32
 *
 * Revision 1.1  2015/03/06 23:16:41  dechavez
 * initial release
 *
 */
