#pragma ident "$Id: cascade.c,v 1.4 2015/09/30 20:04:25 dechavez Exp $"
/*======================================================================
 *
 *  Build up DCCDB_CASCADEs from sorted stage table entries
 *
 *====================================================================*/
#include "dccdb.h"

void dccdbPrintCascade(FILE *fp, DCCDB_CASCADE *cascade, BOOL full)
{
int i;

    if (fp == NULL || cascade == NULL) return;

    fprintf(fp, "%-6s", cascade->sta);
    fprintf(fp, " %-8s", cascade->chn);
    fprintf(fp, " %-2s", cascade->loc);
    fprintf(fp, " %17.5f", cascade->begt);
    fprintf(fp, " %17.5f", cascade->endt);
    fprintf(fp, " %12.5E", cascade->srate);
    fprintf(fp, " %12.5E", cascade->freq);
    fprintf(fp, " %12.5E", cascade->a0);
    fprintf(fp, "\n");

    if (full) for (i = 0; i < cascade->nentry; i++) {
        fprintf(fp, "    ");
        fprintf(fp, "%8ld",    cascade->entry[i].stageid);
        fprintf(fp, " %-16s",  cascade->entry[i].ssident);
        fprintf(fp, " %11.5g", cascade->entry[i].gnom);
        fprintf(fp, " %10.6f", cascade->entry[i].gcalib);
        fprintf(fp, " %-16s",  cascade->entry[i].iunits);
        fprintf(fp, " %-16s",  cascade->entry[i].ounits);
        fprintf(fp, " %8.d",   cascade->entry[i].decifac);
        fprintf(fp, " %11.7f", cascade->entry[i].srate);
        fprintf(fp, " %s/%s:%s",  cascade->entry[i].dir, cascade->entry[i].dfile, filterString(&cascade->entry[i].filter, NULL));
        fprintf(fp, "\n");
    }
}

void dccdbPrintCascadeList(FILE *fp, LNKLST *list, BOOL full)
{
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        dccdbPrintCascade(fp, (DCCDB_CASCADE *) crnt->payload, full);
        crnt = listNextNode(crnt);
    }
}

static REAL64 CascadeSampleRate(DCCDB_CASCADE *cascade)
{
int lastindex;
REAL64 srate;

    lastindex = cascade->nentry - 1;
    srate = cascade->entry[lastindex].srate / cascade->entry[lastindex].decifac;

    return srate;
}

BOOL dccdbBuildCascades(DCCDB *db)
{
int i, count, index;
static char *fid = "dccdbBuildCascades";

    if (db == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (db->nstage == 0) {
        db->ncascade = 0;
        db->cascade = NULL;
        return TRUE;
    }

    if (db->stage[0].stageid != 1) {
        errno = EINVAL;
        logioMsg(db->lp, LOG_ERR, "%s: db->stage[0].stageid != 1\n", fid);
        return FALSE;
    } else {
        count = 1;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "searching stage table for distinct calibration epochs\n");
    for (i = 1; i < db->nstage; i++) {
        if (db->stage[i].stageid == 1) {
            ++count;
        } else if (db->stage[i].stageid != db->stage[i-1].stageid + 1) {
            logioMsg(db->lp, LOG_ERR, "%s: UNEXPECTED STAGEID INCREMENT found between stage records %d and %d\n", fid, i-1, i);
            return FALSE;FALSE;
        }
    }
    db->ncascade = count;

    if ((db->cascade = (DCCDB_CASCADE *) malloc(sizeof(DCCDB_CASCADE) * db->ncascade)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "generating %d cascades\n", db->ncascade);

    for (i = 0, index = -1; i < db->nstage; i++) {
        if (db->stage[i].stageid == 1) {
            if (++index >= db->ncascade) {
                logioMsg(db->lp, LOG_ERR, "%s: UNEXPECTED LOGIC ERROR #1 GENERATING CASCADES!", fid);
                return FALSE;
            }
            strncpy(db->cascade[index].sta, db->stage[i].sta, DCCDB_STAGE_STA_LEN+1);
            strncpy(db->cascade[index].chn, db->stage[i].chn, DCCDB_STAGE_CHN_LEN+1);
            strncpy(db->cascade[index].loc, db->stage[i].loc, DCCDB_STAGE_LOC_LEN+1);
            db->cascade[index].begt =  db->stage[i].begt;
            db->cascade[index].endt =  db->stage[i].endt;
            db->cascade[index].freq = -1.0;
            db->cascade[index].a0 = 0.0;
            db->cascade[index].nentry = 0;
            db->cascade[index].chan = NULL;
        }
        if (db->cascade[index].nentry >= DCCDB_MAX_CASCADE_ENTRIES) {
            logioMsg(db->lp, LOG_ERR, "%s: UNEXPECTED LOGIC ERROR #2 GENERATING CASCADES!", fid);
            return FALSE;
        }
        db->cascade[index].entry[db->cascade[index].nentry++] = db->stage[i];
    }

    if (index != db->ncascade-1) {
        logioMsg(db->lp, LOG_ERR, "%s: UNEXPECTED LOGIC ERROR #3 GENERATING CASCADES! index=%d != ncascade=%d", fid, index, db->ncascade);
        return FALSE;
    }

    for (i = 0; i < db->ncascade; i++) db->cascade[i].srate = CascadeSampleRate(&db->cascade[i]);

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
 * $Log: cascade.c,v $
 * Revision 1.4  2015/09/30 20:04:25  dechavez
 * removed isduplicate field
 *
 * Revision 1.3  2015/09/04 00:41:29  dechavez
 * added "full" arg to dccdbPrintCascade(),
 * initialize new chan and isduplicate fields in dccdbBuildCascades()
 *
 * Revision 1.2  2015/08/24 18:31:53  dechavez
 * added a0 and removed izero and leadfac from dccdbPrintCascade(), added a0 to dccdbBuildCascades()
 *
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
