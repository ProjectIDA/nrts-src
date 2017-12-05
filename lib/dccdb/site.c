#pragma ident "$Id: site.c,v 1.1 2015/07/10 18:23:26 dechavez Exp $"
/*======================================================================
 *
 *  DCCDB_SITE stuff
 *
 *====================================================================*/
#include "dccdb.h"

static int SiteCompareFunc(const void *aptr, const void *bptr)
{
int result;
DCCDB_SITE *a, *b;

    a = (DCCDB_SITE *) aptr;
    b = (DCCDB_SITE *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if (a->begt > b->begt) return  1;
    if (a->begt < b->begt) return -1;
    if (a->endt > b->endt) return  1;
    if (a->endt < b->endt) return -1;

    return 0;
}

void dccdbPrintSiteRecord(FILE *fp, DCCDB_SITE *site)
{
    fprintf(fp, "%-6s", site->sta);
    fprintf(fp, " %17.5f", site->begt);
    fprintf(fp, " %17.5f", site->endt);
    fprintf(fp, " %9.4f", site->lat);
    fprintf(fp, " %9.4f", site->lon);
    fprintf(fp, " %9.4f", site->elev);
    fprintf(fp, " %-50s", site->desc);
    fprintf(fp, " %17.5f", site->lddate);
    fprintf(fp, "\n");
}

BOOL dccdbReadSite(DCCDB *db)
{
int i;
static char *fid = "dccdbReadSite";

    if (dbquery(db->table.site, dbRECORD_COUNT, &db->nsite) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->site = (DCCDB_SITE *) malloc(sizeof(DCCDB_SITE) * db->nsite)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d site records\n", db->nsite);
    for (i = 0; i < db->nsite; i++) {
        db->table.site.record = i;
        if (dbgetv(db->table.site, 0, DCCDB_SITE_STA, db->site[i].sta, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_STA) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_DESC, db->site[i].desc, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_DESC) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_BEGT, &db->site[i].begt, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_BEGT) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_ENDT, &db->site[i].endt, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_ENDT) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_LAT, &db->site[i].lat, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_LAT) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_LON, &db->site[i].lon, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_LON) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_ELEV, &db->site[i].elev, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_ELEV) failed\n", fid);
            return FALSE;
        }
        if (dbgetv(db->table.site, 0, DCCDB_SITE_LDDATE, &db->site[i].lddate, 0) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv(DCCDB_SITE_LDDATE) failed\n", fid);
            return FALSE;
        }
    }

    qsort(db->site, db->nsite, sizeof(DCCDB_SITE), SiteCompareFunc);

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
 * $Log: site.c,v $
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
