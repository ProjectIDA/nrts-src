#pragma ident "$Id: chan.c,v 1.4 2015/09/30 20:06:22 dechavez Exp $"
/*======================================================================
 *
 *  DCCDB_CHAN stuff
 *
 *====================================================================*/
#include "dccdb.h"

int ChanCompareFunc(const void *aptr, const void *bptr)
{
int result;
DCCDB_CHAN *a, *b;

    a = (DCCDB_CHAN *) aptr;
    b = (DCCDB_CHAN *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if ((result = strcmp(a->chn, b->chn)) != 0) return result;
    if ((result = strcmp(a->loc, b->loc)) != 0) return result;
    if (a->begt > b->begt) return  1;
    if (a->begt < b->begt) return -1;
    if (a->endt > b->endt) return  1;
    if (a->endt < b->endt) return -1;

    return 0;
}

void dccdbPrintChanRecord(FILE *fp, DCCDB_CHAN *chan)
{
    fprintf(fp, "%-6s", chan->sta);
    fprintf(fp, " %-8s", chan->chn);
    fprintf(fp, " %-2s", chan->loc);
    fprintf(fp, " %17.5f", chan->begt);
    fprintf(fp, " %17.5f", chan->endt);
    fprintf(fp, " %9.4f", chan->edepth);
    fprintf(fp, " %6.1f", chan->hang);
    fprintf(fp, " %6.1f", chan->vang);
    fprintf(fp, " %-2s", chan->flag);
    fprintf(fp, " %-6s", chan->instype);
    fprintf(fp, " %16.6f", chan->nomfreq);
    fprintf(fp, "\n");
}

BOOL dccdbReadChan(DCCDB *db)
{
int i;
static char *fid = "dccdbReadChan";

    if (dbquery(db->table.chan, dbRECORD_COUNT, &db->nchan) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->chan = (DCCDB_CHAN *) malloc(sizeof(DCCDB_CHAN) * db->nchan)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d chan records\n", db->nchan);
    for (i = 0; i < db->nchan; i++) {
        db->table.chan.record = i;
        if (dbgetv(db->table.chan, 0,
            DCCDB_CHAN_STA,      db->chan[i].sta,
            DCCDB_CHAN_CHN,      db->chan[i].chn,
            DCCDB_CHAN_LOC,      db->chan[i].loc,
            DCCDB_CHAN_FLAG,     db->chan[i].flag,
            DCCDB_CHAN_INSTYPE,  db->chan[i].instype,
            DCCDB_CHAN_BEGT,    &db->chan[i].begt,
            DCCDB_CHAN_ENDT,    &db->chan[i].endt,
            DCCDB_CHAN_EDEPTH,  &db->chan[i].edepth,
            DCCDB_CHAN_HANG,    &db->chan[i].hang,
            DCCDB_CHAN_VANG,    &db->chan[i].vang,
            DCCDB_CHAN_NOMFREQ, &db->chan[i].nomfreq,
            0
        ) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv failed", fid);
            return FALSE;
        }
        if (strcmp(db->chan[i].loc, DCCDB_NULL_STRING) == 0) strcpy(db->chan[i].loc, DCCDB_EMPTY_LOC);
    }

    qsort(db->chan, db->nchan, sizeof(DCCDB_CHAN), ChanCompareFunc);

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
 * $Log: chan.c,v $
 * Revision 1.4  2015/09/30 20:06:22  dechavez
 * removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
 * set loc to "  " (double blanks) if stored as a null value ("-") in the database
 *
 * Revision 1.3  2015/08/24 18:29:09  dechavez
 * fixed cut and paste typo when reporting the nmber of chan records read/sorted
 *
 * Revision 1.2  2015/07/10 20:39:56  dechavez
 * removed some leftover debug printf's
 *
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
