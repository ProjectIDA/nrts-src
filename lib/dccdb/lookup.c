#pragma ident "$Id: lookup.c,v 1.7 2015/09/30 20:08:12 dechavez Exp $"
/*======================================================================
 *
 *  Look up stuff
 *
 *====================================================================*/
#include "dccdb.h"

int dccdbLookupAbbrevCode(DCCDB *db, char *target)
{
int i;

    for (i = 0; i < db->nb33; i++) if (strcasecmp(target, db->b33[i].item) == 0) return db->b33[i].code;
    return -1;
}

int dccdbLookupUnitCode(DCCDB *db, char *target)
{
int i;

    for (i = 0; i < db->nb34; i++) if (strcasecmp(target, db->b34[i].unit) == 0) return db->b34[i].code;
    return -1;
}

static BOOL IdentsMatch(DCCDB_CHAN *chan, DCCDB_CASCADE *cascade)
{
    if (strcmp(chan->sta, cascade->sta) != 0) return FALSE;
    if (strcmp(chan->chn, cascade->chn) != 0) return FALSE;
    if (strcmp(chan->loc, cascade->loc) != 0) return FALSE;

    return TRUE;
}

static BOOL EpochsIntersect(DCCDB_CHAN *chan, DCCDB_CASCADE *cascade)
{
    if (cascade->endt < chan->begt) return FALSE; /* cascade epoch is fully before the channel epoch */
    if (cascade->begt > chan->endt) return FALSE; /*    "      "    "   "   after   "      "      "  */

    return TRUE; /* must intersect */
}

static BOOL AddEmptyCascade(LNKLST *list, DCCDB_CHAN *chan)
{
DCCDB_CASCADE new;

    strcpy(new.sta, chan->sta);
    strcpy(new.chn, chan->chn);
    strcpy(new.loc, chan->loc);
    new.begt = chan->begt;
    new.endt = chan->endt;
    new.srate = 0.0;
    new.freq = 0.0;
    new.a0 = 0.0;
    new.errcode = DCCDB_ERRCODE_NO_ERROR;
    new.nentry = 0;
    new.chan = chan;

    return listAppend(list, &new, sizeof(DCCDB_CASCADE));
}

LNKLST *dccdbLookupCascadeFromChan(DCCDB *db, DCCDB_CHAN *chan)
{
int i;
LNKLST *list;
DCCDB_CASCADE *cascade;

    if (db == NULL || chan == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((list = listCreate()) == NULL) return NULL;

    for (i = 0; i < db->ncascade; i++) {
        cascade = &db->cascade[i];
        if (IdentsMatch(chan, cascade) && EpochsIntersect(chan, cascade)) {
            cascade->chan = chan;
            if (!dccdbSetCascadeA0Freq(cascade, chan->nomfreq)) return NULL;
            if (!listAppend(list, cascade, sizeof(DCCDB_CASCADE))) {
                listDestroy(list);
                return NULL;
            }
        }
    }

    if (list->count == 0 && !AddEmptyCascade(list, chan)) {
        listDestroy(list);
        return NULL;
    }

    listSetArrayView(list);
    return list;
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
 * $Log: lookup.c,v $
 * Revision 1.7  2015/09/30 20:08:12  dechavez
 * AddEmptyCascade() if no stage channels match chan epoch in dccdbLookupCascadeFromChan()
 * This sets things up so that at blockette 52 will fall out "naturally" for said channel
 *
 * Revision 1.6  2015/09/11 16:08:20  dechavez
 * return actual code, not loop index, in dccdbLookupAbbrevCode() and dccdbLookupUnitCode()
 *
 * Revision 1.5  2015/09/04 00:34:28  dechavez
 * assign cascade->chan in dccdbLookupCascadeFromChan() with "parent"
 *
 * Revision 1.4  2015/08/27 23:08:24  dechavez
 * Fixed logic in "join" of chan and cascade in dccdbLookupCascadeFromChan()
 *
 * Revision 1.3  2015/08/24 18:28:07  dechavez
 * made rule for time window matching between stage and chan more lax, set a0 field
 *
 * Revision 1.2  2015/07/10 18:21:06  dechavez
 * added dccdbLookupCascadeFromChan()
 *
 * Revision 1.1  2015/03/06 23:16:41  dechavez
 * initial release
 *
 */
