#pragma ident "$Id: stage.c,v 1.4 2015/12/01 22:18:29 dechavez Exp $"
/*======================================================================
 *
 *  Stage record processing.
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_STAGE

static void Print(int type, DCCDB_STAGE *stage, REAL64 freq, REAL64 srate)
{
    if (strcasecmp(stage->sta, "AAK") != 0) return;
    if (strcasecmp(stage->chn, "BHZ") != 0 && strcasecmp(stage->chn, "LHZ") != 0 && strcasecmp(stage->chn, "VHZ") != 0) return;
    if (strcasecmp(stage->loc,  "00") != 0) return;
    if ((UINT32) stage->begt != (UINT32) 655689600) return;
    if (srate < 0.0) {
        printf("   ProcessStage: stageid=%d empty B%d\n", stage->stageid, type);
    } else {
        printf("   ProcessStage: stageid=%d B%d freq=%lf, srate=%lf filter=%s/%s\n", stage->stageid, type, freq, srate, stage->dir, stage->dfile);
    }
}

/* A digital stage is one that emits counts */

static BOOL IsDigitalStage(DCCDB_STAGE *stage)
{
    return (filterUnitsCode(stage->ounits) == FILTER_UNITS_COUNTS) ? TRUE : FALSE;
}

void ProcessStage(DCCDB *db, FILE *fp, char *fpath, DCCDB_SITE *site, DCCDB_CHAN *chan, DCCDB_STAGE *stage, REAL64 freq)
{
REAL64 a0 = 1.0;    /* will get changed below if appropriate */
REAL64 delay = 0.0; /* will get changed below if appropriate */
static FILTER_PANDZ EmptyPandZ = FILTER_EMPTY_PANDZ;
static FILTER_COEFF EmptyCoeff = FILTER_EMPTY_COEFF;
static char *fid = "ProcessStage";

    if (stage->filter.type & FILTER_TYPE_PANDZ) {

        if (!filterA0(&stage->filter, freq, stage->srate, &a0)) {
            LogErr("%s: filterA0: %s\n", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 1);
        }
        WriteBlockette53(fp, fpath, db, stage, freq, a0, &stage->filter.data.pz);

    } else if (stage->filter.type & FILTER_TYPE_COEFF) {

        delay = stage->filter.data.cf.delay / (stage->srate / stage->decifac);
        WriteBlockette54(fp, fpath, db, stage, &stage->filter.data.cf);

    } else if (stage->filter.type == FILTER_TYPE_NULL) {

        if (IsDigitalStage(stage)) {
            WriteBlockette54(fp, fpath, db, stage, &EmptyCoeff);
        } else {
            WriteBlockette53(fp, fpath, db, stage, freq, a0, &EmptyPandZ);
        }

    } else {

        LogErr("%s: ERROR: unexpected FILTER type '%d'\n", fid, stage->filter.type);
        GracefulExit(MY_MOD_ID + 2);

    }

    if (IsDigitalStage(stage)) {
        WriteBlockette57(fp, fpath, stage, delay);
        WriteBlockette58(fp, fpath, stage->stageid, stage->gnom * stage->gcalib, 0.0);
    } else {
        WriteBlockette58(fp, fpath, stage->stageid, stage->gnom * stage->gcalib, freq);
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
 * $Log: stage.c,v $
 * Revision 1.4  2015/12/01 22:18:29  dechavez
 * added srate to filterA0() call
 *
 * Revision 1.3  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
