#pragma ident "$Id: cascade.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Cascade Loop
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_CASCADE

void ProcessCascade(FILE *fp, char *fpath, DCCDB *db, DCCDB_SITE *site, DCCDB_CHAN *chan, DCCDB_CASCADE *cascade)
{
int i;
DCCDB_STAGE *stage;
static char *fid = "ProcessCascade";
char str1[1024], str2[1024]; utilDttostr(cascade->begt, 0, str1); utilDttostr(cascade->endt, 0, str2);

/* Write out blockette 52 and update POD request file */

    stage = cascade->nentry > 0 ? &cascade->entry[0] : NULL;

    WriteBlockette52(fp, fpath, db, site, chan, stage, cascade->srate);
    UpdateReq(cascade);
    if (!cascade->nentry) return; /* LOG channel doesn't have any stages, for example */

/* Write out each stage of the calibration epoch */

    for (i = 0; i < cascade->nentry; i++) {
        stage = &cascade->entry[i];
        ProcessStage(db, fp, fpath, site, chan, stage, cascade->freq);
    }

/* And end with the stage 0 total sensitivity */

    WriteBlockette58(fp, fpath, 0, cascade->a0, cascade->freq);
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
 * $Log: cascade.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
