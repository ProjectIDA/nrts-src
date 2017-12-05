#pragma ident "$Id: sanity.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Sanity checks
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_SANITY

static BOOL StageUnitsCheck(OLD *old)
{
int i;
BOOL retval = TRUE;
static char *fid = "StageUnitsCheck";

    for (i = 0; i < old->nstage; i++) {
        if (LookupUnits(old->stage[i].iunits) == NULL) {
            LogErr("%s: no match for iunits=%d: old stage row %d = %s\n", fid, strerror(errno), i, stagestr(&old->stage[i]));
            retval = FALSE;
        }
    }

    LogMsg(1, "stage units sanity check %s\n", retval ? "OK" : "FAILED!");
    return retval;
}

static BOOL StageResponseFileCheck(OLD *old)
{
int i;
FILE *fp;
BOOL retval = TRUE;
char path[MAXPATHLEN+1];
static char *fid = "StageResponseFileCheck";

    for (i = 0; i < old->nstage; i++) {
        sprintf(path, "%s/%s", old->stage[i].dir, old->stage[i].dfile);
        if (strcmp(path, "-/-") != 0) {
            if ((fp = fopen(path, "r")) == NULL) {
                LogErr("%s: %s: %s: row %d = %s\n", fid, path, strerror(errno), i, stagestr(&old->stage[i]));
                retval = FALSE;
            } else {
                fclose(fp);
            }
        }
    }

    LogMsg(1, "stage response file sanity check %s\n", retval ? "OK" : "FAILED!");
    return retval;
}

static BOOL StageVsSeedlocCheck(OLD *old)
{
BOOL retval = TRUE;
OLD_SEEDLOC *instance[3];
LNKLST *list;
int i, j, k, count, errors;
char buf[1024];
#define SEEDLOC(arg) ((OLD_SEEDLOC *) arg)
static char *fid = "SanityCheck:StageVsSeedlocCheck";

    for (i = 0; i < old->nstage; i++) {
        if ((list = GenerateStaChnLocTuplesForNewStage(old, &old->stage[i])) != NULL) {
            sprintf(buf, "%s: %d (chn, loc) tuples found for stage row %d %s: %s = ", fid, list->count, i, stagestr(&old->stage[i]), old->stage[i].chan);
            for (j = 0; j < list->count; j++) sprintf(buf+strlen(buf), " (%s, %s)", SEEDLOC(list->array[j])->newchan, SEEDLOC(list->array[j])->loccode);
            LogMsg(2, "%s\n", buf);
            listDestroy(list);
        } else {
            LogErr("%s ERROR: 0 sta/chn/loc matches from seedloc for stage row %d %s\n", fid, i, stagestr(&old->stage[i]));
            retval = FALSE;
        }
    }

    LogMsg(1, "stage versus seedloc sanity check %s\n", retval ? "OK" : "FAILED!");
    return retval;
}

static BOOL SitechanVsSeedlocCheck(OLD *old)
{
BOOL retval = TRUE;
OLD_SEEDLOC *instance[3];
LNKLST *list;
int i, j, k, count, errors;
char buf[1024];
#define SEEDLOC(arg) ((OLD_SEEDLOC *) arg)
static char *fid = "SanityCheck:SitechanVsSeedlocCheck";

    for (i = 0; i < old->nsitechan; i++) {
        if ((list = GenerateStaChnLocTuplesForNewChan(old, &old->sitechan[i])) != NULL) {
            sprintf(buf, "%s: %d (chn, loc) tuples found for sitechan row %d %s: %s = ", fid, list->count, i, sitechanstr(&old->sitechan[i]), old->sitechan[i].chan);
            for (j = 0; j < list->count; j++) sprintf(buf+strlen(buf), " (%s, %s)", SEEDLOC(list->array[j])->newchan, SEEDLOC(list->array[j])->loccode);
            LogMsg(2, "%s\n", buf);
            listDestroy(list);
        } else {
            LogErr("%s WARNING: 0 sta/chn/loc matches from seedloc for sitechan row %d %s (ignored)\n", fid, i, sitechanstr(&old->sitechan[i]));
            retval = TRUE; /* ignore the error */
        }
    }

    LogMsg(1, "sitechan versus seedloc sanity check %s\n", retval ? "OK" : "FAILED!");
    return retval;
}

static BOOL SitechanVsSensorChanidCheck(OLD *old)
{
BOOL retval = TRUE;
int i, j, count, chanid, inid;
char *crnt, *prev;
static char *fid = "SitechanVsSensorChanidCheck";

    for (i = 0; i < old->nsitechan; i++) {
        chanid = old->sitechan[i].chanid;
        crnt = prev = NULL;
        for (j = 0; j < old->nsensor; j++) {
            if (old->sensor[j].chanid == chanid) {
                inid = old->sensor[j].inid;
                if ((crnt = LookupInstypeFromInid(old, inid)) == NULL) {
                    LogErr("ERROR: LookupInstypeFromInid() failed for sensor row %d, inid=%d\n", j+1, inid);
                    retval = FALSE;
                } else {
                    if (prev != NULL) {
                        if (strcmp(crnt, prev) != 0) {
                            LogErr("%s: ERROR sitechan row %d chanid=%d matches instype %s and %s\n", fid, i+1, old->sitechan[i].chanid, prev, crnt);
                            retval = FALSE;
                        }
                    }
                    prev = crnt;
                    LogMsg(1, "sitechan versus sensor chanid sanity check OK\n");
                    return TRUE;
                }
            }
        }
        LogErr("%s: ERROR: sitechan row %d chanid=%d does not have any matches in sensor table\n", fid, i+1, old->sitechan[i].chanid);
        retval = FALSE;
    }

    LogMsg(1, "sitechan versus sensor chanid sanity check %s\n", retval ? "OK" : "FAILED!");
    return retval;
}

BOOL SanityCheck(OLD *old)
{
FILE *fp;
int errors = 0;

    LogMsg(1, "conducting sanity checks on input database\n");

    if (!StageUnitsCheck(old)) ++errors;
    if (!StageResponseFileCheck(old)) ++errors;
    if (!StageVsSeedlocCheck(old)) ++errors;
    if (!SitechanVsSeedlocCheck(old)) ++errors;
    if (!SitechanVsSensorChanidCheck(old)) ++errors;

    return (errors == 0) ? TRUE : FALSE;
}

/* Revision History
 *
 * $Log: sanity.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
