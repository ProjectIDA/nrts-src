#pragma ident "$Id: db.c,v 1.4 2015/09/04 00:38:00 dechavez Exp $"
/*======================================================================
 *
 *  Read the database
 *
 *====================================================================*/
#include "dccdb.h"
#include "sanio.h" /* for the SAN_EPOCH (1999) constant */

static DCCDB *FreeDB(DCCDB *db)
{
    if (db == NULL) return NULL;

    if (db->b30     != NULL) free(db->b30);
    if (db->b33     != NULL) free(db->b33);
    if (db->b34     != NULL) free(db->b34);
    if (db->site    != NULL) free(db->site);
    if (db->chan    != NULL) free(db->chan);
    if (db->stage   != NULL) free(db->stage);
    if (db->seedloc != NULL) free(db->seedloc);

    free(db);
    return NULL;
}

static BOOL GenerateB30(DCCDB *db)
{
int i;
LNKLST *list;
LNKLST_NODE *crnt;
static char *fid = "dccdbOpenDatabase:GenerateB30";

    if ((list = mseedDefaultBlockette30List()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: mseedDefaultBlockette30List failed: %s", fid, strerror(errno));
        return FALSE;
    }
    db->nb30 = list->count;

    if ((db->b30 = (MSEED_B30 *) malloc(sizeof(MSEED_B30) * db->nb30)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    i = 0;
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        db->b30[i++] = *((MSEED_B30 *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    if (i != list->count) {
        logioMsg(db->lp, LOG_ERR, "%s: PROGRAM LOGIC ERROR: i (%d) != list->count (%d)\n", fid, i, list->count);
        free(db->b30);
        return FALSE;
    }

    listDestroy(list);

    return TRUE;
}

static void InitDB(DCCDB *db, LOGIO *lp, BOOL verbose)
{
    db->lp = lp;
    db->verbose = verbose;

    db->b30 = NULL;
    db->nb30 = 0;

    db->b33 = NULL;
    db->nb33 = 0;

    db->b34 = NULL;
    db->nb34 = 0;

    db->site = NULL;
    db->nsite = 0;

    db->chan = NULL;
    db->nchan = 0;

    db->stage = NULL;
    db->nstage = 0;

    db->seedloc = NULL;
    db->nseedloc = 0;

    db->cascade = NULL;
    db->ncascade = 0;
}

DCCDB *dccdbOpenDatabase(char *dbpath, LOGIO *lp, BOOL verbose)
{
DCCDB *db;
int i, j, errors = 0;
static char *fid = "dccdbOpenDatabase";

    if ((db = (DCCDB *) malloc(sizeof(DCCDB))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    InitDB(db, lp, verbose);

    if (dbopen(dbpath, DCCDB_READONLY, &db->table.all) != 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbopen: ERROR: %s\n", fid, dbpath);
        return FreeDB(db);
    }

    db->table.abbrev  = dblookup(db->table.all, 0, DCCDB_ABBREV_TABLE_NAME,  0, 0);
    db->table.site    = dblookup(db->table.all, 0, DCCDB_SITE_TABLE_NAME,    0, 0);
    db->table.chan    = dblookup(db->table.all, 0, DCCDB_CHAN_TABLE_NAME,    0, 0);
    db->table.stage   = dblookup(db->table.all, 0, DCCDB_STAGE_TABLE_NAME,   0, 0);
    db->table.seedloc = dblookup(db->table.all, 0, DCCDB_SEEDLOC_TABLE_NAME, 0, 0);
    db->table.units   = dblookup(db->table.all, 0, DCCDB_UNITS_TABLE_NAME,   0, 0);

    if (!dccdbReadAbbrev(db)   ) return FreeDB(db);
    if (!dccdbReadChan(db)     ) return FreeDB(db);
    if (!dccdbReadSeedloc(db)  ) return FreeDB(db);
    if (!dccdbReadSite(db)     ) return FreeDB(db);
    if (!dccdbReadUnits(db)    ) return FreeDB(db);
    if (!dccdbReadStage(db)    ) return FreeDB(db);
    if (!GenerateB30(db)       ) return FreeDB(db);
    if (!dccdbBuildCascades(db)) return FreeDB(db);

/* sanity checks */

    for (i = 0; i < db->nstage; i++) {
        if (dccdbLookupUnitCode(db, db->stage[i].iunits) < 0) {
            logioMsg(db->lp, LOG_WARN, "No entry in units table for stage iunits='%s'\n", db->stage[i].iunits);
            ++errors;
        }
        if (dccdbLookupUnitCode(db, db->stage[i].ounits) < 0) {
            logioMsg(db->lp, LOG_WARN, "No entry in units table for stage ounits='%s'\n", db->stage[i].ounits);
            ++errors;
        }
    }

    for (i = 0; i < db->nchan; i++) {
        if (dccdbLookupAbbrevCode(db, db->chan[i].instype) < 0) {
            logioMsg(db->lp, LOG_WARN, "No entry in abbrev table for chan instype='%s'\n", db->chan[i].instype);
            ++errors;
        }
    }

    return errors ? FreeDB(db) : db;
}

DCCDB *dccdbCloseDatabase(DCCDB *db)
{
    if (db != NULL) {
        dbclose(db->table.abbrev);
        dbclose(db->table.site);
        dbclose(db->table.chan);
        dbclose(db->table.stage);
        dbclose(db->table.seedloc);
    }

    return FreeDB(db);
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
 * $Log: db.c,v $
 * Revision 1.4  2015/09/04 00:38:00  dechavez
 * removed end time sanity checks from previous commit
 *
 * Revision 1.3  2015/08/27 23:06:54  dechavez
 * added sanity checks for suspcious channel and stage end times, then ifdef'd out after patching database
 *
 * Revision 1.2  2015/07/10 18:18:07  dechavez
 * moved all the read functions to their own files, added dccdbCloseDatabase(), added cascade support
 *
 * Revision 1.1  2015/03/06 23:16:41  dechavez
 * initial release
 *
 */
