#pragma ident "$Id: lookup.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Lookup stuff
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_LOOKUP

static void AddUniqueRecord(LNKLST *dest, OLD_SEEDLOC *that)
{
LNKLST_NODE *crnt;
OLD_SEEDLOC *this;

    crnt = listFirstNode(dest);
    while (crnt != NULL) {
        this = (OLD_SEEDLOC *) crnt->payload;
        if (strcmp(this->sta, that->sta) == 0 && strcmp(this->newchan, that->newchan) == 0 && strcmp(this->loccode, that->loccode) == 0) return;
        crnt = listNextNode(crnt);
    }
    listAppend(dest, that, sizeof(OLD_SEEDLOC));
}

static LNKLST *LookupSeedlocFromStageExactTimes(OLD *old, OLD_STAGE *stage)
{
int i;
LNKLST *list = NULL;
static char *fid = "LookupSeedlocFromStageExactTimes";

    if ((list = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    for (i = 0; i < old->nseedloc; i++) {
        if (StageSeedlocExactMatch(stage, &old->seedloc[i])) {
            if (!listAppend(list, &old->seedloc[i], sizeof(OLD_SEEDLOC))) {
                LogErr("%s: listAppend: %s\n", fid, strerror(errno));
                GracefulExit(MY_MOD_ID + 2);
            }
        }
    }

    if (list->count == 0) {
        listDestroy(list);
        list = NULL;
    } else if (!listSetArrayView(list)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    return list;
}

static LNKLST *LookupSeedlocFromStagePartialTimes(OLD *old, OLD_STAGE *stage)
{
int i;
LNKLST *list = NULL;
static char *fid = "LookupSeedlocFromStagePartialTimes";

    if ((list = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }

    for (i = 0; i < old->nseedloc; i++) {
        if (StageSeedlocPartialMatch(stage, &old->seedloc[i])) {
            if (!listAppend(list, &old->seedloc[i], sizeof(OLD_SEEDLOC))) {
                LogErr("%s: listAppend: %s\n", fid, strerror(errno));
                GracefulExit(MY_MOD_ID + 5);
            }
        }
    }

    if (list->count == 0) {
        listDestroy(list);
        list = NULL;
    } else if (!listSetArrayView(list)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 6);
    }

    return list;
}

static void BuildOwnStaChnLocTupleFromChan(LNKLST *list, char *chan)
{
static OLD_SEEDLOC seedloc;
static char *fid = "BuildOwnStaChnLocTupleFromChan";

    memset(&seedloc, 0, sizeof(OLD_SEEDLOC));

    if (strlen(chan) != 5) return;
    strcpy(seedloc.chan, chan);
    strncpy(seedloc.newchan, &chan[0], 3); seedloc.newchan[3] = 0;
    strncpy(seedloc.loccode, &chan[3], 2); seedloc.loccode[2] = 0;

    if (!listAppend(list, &seedloc, sizeof(OLD_SEEDLOC))) {
        LogErr("%s: listAppend: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 7);
    }
}

LNKLST *GenerateStaChnLocTuplesForNewStage(OLD *old, OLD_STAGE *stage)
{
int i;
LNKLST *list, *result;
static char *fid = "GenerateStaChnLocTuplesForNewStage";

    if ((result = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 8);
    }

/* Look for partial matches only if there is no exact match */

    if ((list = LookupSeedlocFromStageExactTimes(old, stage)) != NULL) {
        for (i = 0; i < list->count; i++) AddUniqueRecord(result, (OLD_SEEDLOC *) list->array[i]);
        listDestroy(list);
    } else if ((list = LookupSeedlocFromStagePartialTimes(old, stage)) != NULL) {
        for (i = 0; i < list->count; i++) AddUniqueRecord(result, (OLD_SEEDLOC *) list->array[i]);
        listDestroy(list);
    }

/* If no matches at all, generate our own tuple from the name (if possible) */

    if (result->count == 0) BuildOwnStaChnLocTupleFromChan(result, stage->chan);

/* All done */

    if (result->count == 0) {
        listDestroy(result);
        result = NULL;
    } else if (!listSetArrayView(result)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 9);
    }

    return result;
}

static LNKLST *LookupSeedlocFromSitechanExactTimes(OLD *old, OLD_SITECHAN *sitechan)
{
int i;
LNKLST *list = NULL;
static char *fid = "LookupSeedlocFromSitechanExactTimes";

    if ((list = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 10);
    }

    for (i = 0; i < old->nseedloc; i++) {
        if (SitechanSeedlocExactMatch(sitechan, &old->seedloc[i])) {
            if (!listAppend(list, &old->seedloc[i], sizeof(OLD_SEEDLOC))) {
                LogErr("%s: listAppend: %s\n", fid, strerror(errno));
                GracefulExit(MY_MOD_ID + 11);
            }
        }
    }

    if (list->count == 0) {
        listDestroy(list);
        list = NULL;
    } else if (!listSetArrayView(list)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 12);
    }

    return list;
}

static LNKLST *LookupSeedlocFromSitechanPartialTimes(OLD *old, OLD_SITECHAN *sitechan)
{
int i;
LNKLST *list = NULL;
static char *fid = "LookupSeedlocFromSitechanPartialTimes";

    if ((list = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 13);
    }

    for (i = 0; i < old->nseedloc; i++) {
        if (SitechanSeedlocPartialMatch(sitechan, &old->seedloc[i])) {
            if (!listAppend(list, &old->seedloc[i], sizeof(OLD_SEEDLOC))) {
                LogErr("%s: listAppend: %s\n", fid, strerror(errno));
                GracefulExit(MY_MOD_ID + 14);
            }
        }
    }

    if (list->count == 0) {
        listDestroy(list);
        list = NULL;
    } else if (!listSetArrayView(list)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 15);
    }

    return list;
}

LNKLST *GenerateStaChnLocTuplesForNewChan(OLD *old, OLD_SITECHAN *sitechan)
{
int i;
LNKLST *list, *result;
static char *fid = "GenerateStaChnLocTuplesForNewChan";

    if ((result = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 16);
    }

/* Look for partial matches only if there is no exact match */

    if ((list = LookupSeedlocFromSitechanExactTimes(old, sitechan)) != NULL) {
        for (i = 0; i < list->count; i++) AddUniqueRecord(result, (OLD_SEEDLOC *) list->array[i]);
        listDestroy(list);
    } else if ((list = LookupSeedlocFromSitechanPartialTimes(old, sitechan)) != NULL) {
        for (i = 0; i < list->count; i++) AddUniqueRecord(result, (OLD_SEEDLOC *) list->array[i]);
        listDestroy(list);
    }

/* If no matches at all, generate our own tuple from the name (if possible) */

    if (result->count == 0) BuildOwnStaChnLocTupleFromChan(result, sitechan->chan);

/* All done */

    if (result->count == 0) {
        listDestroy(result);
        result = NULL;
    } else if (!listSetArrayView(result)) {
        LogErr("%s: listSetArrayView: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 17);
    }

    return result;
}

char *LookupInstypeFromInid(OLD *old, int inid)
{
int i;
static char *fid = "LookupInstypeFromInid";

    for (i = 0; i < old->ninstrument; i++) {
        if (old->instrument[i].inid == inid) {
            if (VerifyAbbrevItem(old->instrument[i].instype)) {
                return old->instrument[i].instype;
            } else {
                LogErr("%s: ERROR: no abbrev match for instrument row %d, insname=%s\n", fid, i+1, old->instrument[i].instype);
                return NULL;
            }
        }
    }
    LogErr("%s: ERROR: no entry in old instrument table for for inid=%d\n", fid, inid);
    return NULL;
}

BOOL LookupCalperInstype(OLD *old, DCCDB *new, OLD_SITECHAN *sitechan, double *ncalper, char *instype)
{
char *match;
int i, inid;

    for (i = 0; i < old->nsensor; i++) {
        if (old->sensor[i].chanid == sitechan->chanid) {
            inid = old->sensor[i].inid;
            *ncalper = old->sensor[i].calper;
            if ((match = LookupInstypeFromInid(old, inid)) != NULL) {
                strncpy(instype, match, DCCDB_CHAN_INSTYPE_LEN+1);
                return TRUE;
            } else {
                LogErr("WARNING: LookupInstypeFromInid() failed for inid=%d\n", inid);
                strcpy(instype, "-");
                return FALSE;
            }
        }       
    }
    LogErr("WARNING: no entry in old sensor table for old sitechan %s\n", sitechanstr(sitechan));
    *ncalper = -1.0;
    strcpy(instype, "-");

    return FALSE;
}

/* Revision History
 *
 * $Log: lookup.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
