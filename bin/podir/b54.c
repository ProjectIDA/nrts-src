#pragma ident "$Id: b54.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 54 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B54

void WriteBlockette54(FILE *fp, char *fpath, DCCDB *db, DCCDB_STAGE *stage, FILTER_COEFF *filter)
{
int i;
MSEED_B54 b54;
static char *fid = "WriteBlockette54";

    b54.stageid  = stage->stageid;
    b54.type    = mseedStageType(&stage->filter);
    b54.iunits  = dccdbLookupUnitCode(db, stage->iunits);
    b54.ounits  = dccdbLookupUnitCode(db, stage->ounits);
    b54.nnum    = filter->ncoef; for (i = 0; i < b54.nnum; i++) { b54.num[i].value = filter->coef[i]; b54.num[i].error = 0; }
    b54.nden    = 0;

    if (!mseedWriteBlockette54(fp, &b54)) {
        LogErr("%s: mseedWriteBlockette54: %s: %s\n", fid, fpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
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
 * $Log: b54.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
