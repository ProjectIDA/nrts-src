#pragma ident "$Id: stage.c,v 1.3 2015/09/30 20:09:36 dechavez Exp $"
/*======================================================================
 *
 *  DCCDB_STAGE stuff
 *
 *====================================================================*/
#include "dccdb.h"

static int StageCompareFunc(const void *aptr, const void *bptr)
{
int result;
DCCDB_STAGE *a, *b;

    a = (DCCDB_STAGE *) aptr;
    b = (DCCDB_STAGE *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if ((result = strcmp(a->chn, b->chn)) != 0) return result;
    if ((result = strcmp(a->loc, b->loc)) != 0) return result;
    if (a->begt > b->begt) return  1;
    if (a->begt < b->begt) return -1;
    if (a->endt > b->endt) return  1;
    if (a->endt < b->endt) return -1;

    return (a->stageid - b->stageid);
}

void dccdbPrintStageRecord(FILE *fp, DCCDB_STAGE *stage)
{
    fprintf(fp, "%-6s", stage->sta);
    fprintf(fp, " %-8s", stage->chn);
    fprintf(fp, " %-2s", stage->loc);
    fprintf(fp, " %17.5f", stage->begt);
    fprintf(fp, " %17.5f", stage->endt);
    fprintf(fp, " %8ld", stage->stageid);
    fprintf(fp, " %-16s", stage->ssident);
    fprintf(fp, " %11.5g", stage->gnom);
    fprintf(fp, " %10.6f", stage->gcalib);
    fprintf(fp, " %-16s", stage->iunits);
    fprintf(fp, " %-16s", stage->ounits);
    fprintf(fp, " %8ld", stage->unused1);
    fprintf(fp, " %8.d", stage->decifac);
    fprintf(fp, " %11.7f", stage->srate);
    fprintf(fp, " %11.7g", stage->unused2);
    fprintf(fp, " %-64s", stage->dir);
    fprintf(fp, " %-32s", stage->dfile);
    fprintf(fp, " %17.5f", stage->lddate);
    fprintf(fp, "\n");
}

static BOOL ReadFilterFile(FILTER *dest, char *dir, char *dfile)
{
FILE *fp;
BOOL retval;
char path[MAXPATHLEN+1];

    if (strcmp(dir, DCCDB_NULL_STRING) == 0 || strcmp(dfile, DCCDB_NULL_STRING) == 0) {
        dest->type = FILTER_TYPE_NULL;
        return TRUE;
    }

    sprintf(path, "%s/%s", dir, dfile);
    if ((fp = fopen(path, "r")) == NULL) return FALSE;
    retval = filterRead(fp, dest);
    fclose(fp);

    return retval;
}

BOOL dccdbReadStage(DCCDB *db)
{
int i;
static char *fid = "dccdbReadStage";

    if (dbquery(db->table.stage, dbRECORD_COUNT, &db->nstage) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->stage = (DCCDB_STAGE *) malloc(sizeof(DCCDB_STAGE) * db->nstage)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d stage records\n", db->nstage);
    for (i = 0; i < db->nstage; i++) {
        db->table.stage.record = i;
        if (dbgetv(db->table.stage, 0,
            DCCDB_STAGE_STA,      db->stage[i].sta,
            DCCDB_STAGE_CHN,      db->stage[i].chn,
            DCCDB_STAGE_LOC,      db->stage[i].loc,
            DCCDB_STAGE_SSIDENT,  db->stage[i].ssident,
            DCCDB_STAGE_IUNITS,   db->stage[i].iunits,
            DCCDB_STAGE_OUNITS,   db->stage[i].ounits,
            DCCDB_STAGE_DIR,      db->stage[i].dir,
            DCCDB_STAGE_DFILE,    db->stage[i].dfile,
            DCCDB_STAGE_BEGT,    &db->stage[i].begt,
            DCCDB_STAGE_ENDT,    &db->stage[i].endt,
            DCCDB_STAGE_STAGEID, &db->stage[i].stageid,
            DCCDB_STAGE_GNOM,    &db->stage[i].gnom,
            DCCDB_STAGE_GCALIB,  &db->stage[i].gcalib,
            DCCDB_STAGE_UNUSED1, &db->stage[i].unused1,
            DCCDB_STAGE_DECIFAC, &db->stage[i].decifac,
            DCCDB_STAGE_SRATE,   &db->stage[i].srate,
            DCCDB_STAGE_UNUSED2, &db->stage[i].unused2,
            DCCDB_STAGE_LDDATE,  &db->stage[i].lddate,
            0
        ) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv failed", fid);
            return FALSE;
        }
        if (strcmp(db->stage[i].ssident, DCCDB_NULL_STRING) == 0) db->stage[i].ssident[0] = 0;
        if (!ReadFilterFile(&db->stage[i].filter, db->stage[i].dir, db->stage[i].dfile)) {
            logioMsg(db->lp, LOG_ERR, "%s: ReadFilterFile(path=%s/%s) failed: %s\n",
                fid, db->stage[i].dir, db->stage[i].dfile, &db->stage[i].filter, strerror(errno)
            );
            return FALSE;
        }
    }

    qsort(db->stage, db->nstage, sizeof(DCCDB_STAGE), StageCompareFunc);

    return TRUE;
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
 * Revision 1.3  2015/09/30 20:09:36  dechavez
 * removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
 * set ssident to an empty string if stored as a null value ("-") in the database
 *
 * Revision 1.2  2015/08/24 18:26:06  dechavez
 * replaced izero and leadfac with unused1 and unused2
 *
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
