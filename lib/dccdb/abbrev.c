#pragma ident "$Id: abbrev.c,v 1.3 2015/09/30 20:03:28 dechavez Exp $"
/*======================================================================
 *
 *  abbrev (aka MSEED_B33) stuff
 *
 *====================================================================*/
#include "dccdb.h"

static int B33CompareFunc(const void *aptr, const void *bptr)
{
MSEED_B33 *a, *b;

    a = (MSEED_B33 *) aptr;
    b = (MSEED_B33 *) bptr;

    return (a->code - b->code);
}

void dccdbPrintAbbrevRecord(FILE *fp, MSEED_B33 *b33)
{
    fprintf(fp, "%-6s", b33->item);
    fprintf(fp, " %-50s", b33->desc);
    fprintf(fp, "\n");
}

BOOL dccdbReadAbbrev(DCCDB *db)
{
int i;
static char *fid = "dccdbReadAbbrev";

    if (dbquery(db->table.abbrev, dbRECORD_COUNT, &db->nb33) < 0) {
        logioMsg(db->lp, LOG_ERR, "%s: dbquery failed", fid);
        return FALSE;
    }

    if ((db->b33 = (MSEED_B33 *) malloc(sizeof(MSEED_B33) * db->nb33)) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: malloc failed: %s", fid, strerror(errno));
        return FALSE;
    }

    if (db->verbose) logioMsg(db->lp, LOG_INFO, "reading/sorting %d abbrev records\n", db->nb33);
    for (i = 0; i < db->nb33; i++) {
        db->table.abbrev.record = i;
        db->b33[i].code = db->table.abbrev.record + 1; /* +1 to ensure > 0 because some external code assumes that */
        if (dbgetv(db->table.abbrev, 0,
            DCCDB_ABBREV_ITEM, db->b33[i].item,
            DCCDB_ABBREV_DESC, db->b33[i].desc,
            0
        ) < 0) {
            logioMsg(db->lp, LOG_ERR, "%s: dbgetv failed", fid);
            return FALSE;
        }
    }

    qsort(db->b33, db->nb33, sizeof(MSEED_B33), B33CompareFunc);

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
 * $Log: abbrev.c,v $
 * Revision 1.3  2015/09/30 20:03:28  dechavez
 * removed uneeded free() in the event of dbgetv error (FreeDB() in db.c takes care of that)
 *
 * Revision 1.2  2015/09/11 16:07:29  dechavez
 * use positive, non-zero codes
 *
 * Revision 1.1  2015/07/10 18:23:26  dechavez
 * initial release
 *
 */
