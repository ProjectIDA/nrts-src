#pragma ident "$Id: units.c,v 1.2 2015/09/30 20:12:10 dechavez Exp $"
/*======================================================================
 *
 *  units (aka MSEED_B34) stuff
 *
 *====================================================================*/
#include "dccdb.h"

static int B34CompareFunc(const void *aptr, const void *bptr)
{
MSEED_B34 *a, *b;

    a = (MSEED_B34 *) aptr;
    b = (MSEED_B34 *) bptr;

    return (a->code - b->code);
}

void dccdbPrintUnitsRecord(FILE *fp, MSEED_B34 *b34)
{
    fprintf(fp, "%-16s", b34->unit);
    fprintf(fp, " %-50s", b34->desc);
    fprintf(fp, "\n");
}

BOOL dccdbReadUnits(DCCDB *db)
{
int i;
static char *fid = "dccdbReadUnits";

    if (dbquery(db->table.units, dbRECORD_COUNT, &db->nb34) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->b34 = (MSEED_B34 *) malloc(sizeof(MSEED_B34) * db->nb34)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d units records\n", db->nb34);
    for (i = 0; i < db->nb34; i++) {
        db->table.units.record = i;
        db->b34[i].code = db->table.units.record + 1; /* +1 to ensure > 0 because some external code assumes that */
        if (dbgetv(db->table.units, 0,
            DCCDB_UNITS_UNIT, db->b34[i].unit,
            DCCDB_UNITS_DESC, db->b34[i].desc,
            0
        ) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv failed", fid);
            return FALSE;
        }
    }

    qsort(db->b34, db->nb34, sizeof(MSEED_B34), B34CompareFunc);

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
 * $Log: units.c,v $
 * Revision 1.2  2015/09/30 20:12:10  dechavez
 * use positive, non-zero codes
 * removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
 *
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
