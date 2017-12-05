#pragma ident "$Id: b53.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 53 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B53

void WriteBlockette53(FILE *fp, char *fpath, DCCDB *db, DCCDB_STAGE *stage, REAL64 freq, REAL64 a0, FILTER_PANDZ *filter)
{
int i;
MSEED_B53 b53;
static UTIL_COMPLEX ComplexZero = {0.0, 0.0};
static char *fid = "WriteBlockette53";

/* The filter type is forced to be analog if this is one of those mysterious empty blockette 52's that we need */

    b53.stageid = stage->stageid;
    b53.type = (filter->npole || filter->nzero) ? mseedStageType(&stage->filter) : MSEED_STAGE_TYPE_HERTZ;
    b53.iunits = dccdbLookupUnitCode(db, stage->iunits);
    b53.ounits = dccdbLookupUnitCode(db, stage->ounits);
    b53.a0 = a0;
    b53.freq = freq;
    b53.nzero = filter->nzero;
    for (i = 0; i < filter->nzero; i++) {
        b53.zero[i].value = filter->zero[i];
        b53.zero[i].error = ComplexZero;
    }
    b53.npole = filter->npole;
    for (i = 0; i < filter->npole; i++) {
        b53.pole[i].value = filter->pole[i];
        b53.pole[i].error = ComplexZero;
    }

    if (!mseedWriteBlockette53(fp, &b53)) {
        LogErr("%s: mseedWriteBlockette53: %s: %s\n", fid, fpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }
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
 * $Log: b53.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
