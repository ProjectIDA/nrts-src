#pragma ident "$Id: seedloc.c,v 1.1 2015/07/10 18:23:26 dechavez Exp $"
/*======================================================================
 *
 *  DCCDB_SEEDLOC stuff
 *
 *====================================================================*/
#include "dccdb.h"

int SeedlocCompareFunc(const void *aptr, const void *bptr)
{
int result;
DCCDB_SEEDLOC *a, *b;

    a = (DCCDB_SEEDLOC *) aptr;
    b = (DCCDB_SEEDLOC *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if ((result = strcmp(a->chn, b->chn)) != 0) return result;
    if ((result = strcmp(a->loc, b->loc)) != 0) return result;
    if (a->begt > b->begt) return  1;
    if (a->begt < b->begt) return -1;
    if (a->endt > b->endt) return  1;
    if (a->endt < b->endt) return -1;

    return 0;
}
void dccdbPrintSeedlocRecord(FILE *fp, DCCDB_SEEDLOC *seedloc)
{
    fprintf(fp, "%6s", seedloc->sta);
    fprintf(fp, " %-8s", seedloc->chn);
    fprintf(fp, " %17.5f", seedloc->begt);
    fprintf(fp, " %17.5f", seedloc->endt);
    fprintf(fp, " %-6s", seedloc->seedchn);
    fprintf(fp, " %-2s", seedloc->loc);
    fprintf(fp, " %17.5f", seedloc->lddate);
    fprintf(fp, "\n");
}

BOOL dccdbReadSeedloc(DCCDB *db)
{
int i;
static char *fid = "dccdbReadSeedloc";

    if (dbquery(db->table.seedloc, dbRECORD_COUNT, &db->nseedloc) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->seedloc = (DCCDB_SEEDLOC *) malloc(sizeof(DCCDB_SEEDLOC) * db->nseedloc)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d seedloc records\n", db->nseedloc);
    for (i = 0; i < db->nseedloc; i++) {
        db->table.seedloc.record = i;
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_STA, db->seedloc[i].sta, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_STA) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_CHN, db->seedloc[i].chn, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_CHN) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_LOC, db->seedloc[i].loc, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_LOC) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_SEEDCHN, db->seedloc[i].seedchn, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_LOC) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_BEGT, &db->seedloc[i].begt, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_BEGT) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_ENDT, &db->seedloc[i].endt, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_ENDT) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.seedloc, 0, DCCDB_SEEDLOC_LDDATE, &db->seedloc[i].lddate, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SEEDLOC_LDDATE) failed\n", fid);
            return FALSE;
        }
    }

    qsort(db->seedloc, db->nseedloc, sizeof(DCCDB_SEEDLOC), SeedlocCompareFunc);

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
 * $Log: seedloc.c,v $
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
