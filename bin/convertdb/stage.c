#pragma ident "$Id: stage.c,v 1.2 2015/03/06 21:44:15 dechavez Exp $"
/*======================================================================
 *
 *  Build the new stage table, from the old
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_STAGE

static int StageCompare(const void *aptr, const void *bptr)
{
int result;
DCCDB_STAGE *a, *b;

    a = (DCCDB_STAGE *) aptr;
    b = (DCCDB_STAGE *) bptr;

    if ((result = strcmp(a->sta, b->sta)) != 0) return result;
    if ((int) a->begt != (int) b->begt) return ((int) a->begt < (int) b->begt) ? -1 : 1;
    if ((result = utilChnCompare(a->chn, b->chn)) != 0) return result;
    if ((result = strcmp(a->loc, b->loc)) != 0) return result;
    if (a->stageid != b->stageid) return (a->stageid < b->stageid) ? -1 : 1;
    if ((int) a->endt != (int) b->endt) return ((int) a->endt < (int) b->endt) ? -1 : 1;

    return 0;
}

static BOOL AddNewStageEntry(DCCDB *new, OLD_STAGE *stage, OLD_SEEDLOC *seedloc, LNKLST *head)
{
DCCDB_STAGE entry;
char *iunits, *ounits;

    memset(&entry, 0, sizeof(DCCDB_STAGE));
    strncpy(entry.sta, stage->sta, DCCDB_STAGE_STA_LEN+1);
    strncpy(entry.chn, seedloc->newchan, DCCDB_STAGE_CHN_LEN+1);
    strncpy(entry.loc, seedloc->loccode, DCCDB_STAGE_LOC_LEN+1);
    entry.begt = stage->time;
    entry.endt = stage->endtime;
    entry.stageid = stage->stageid;
    strncpy(entry.ssident, stage->ssident, DCCDB_STAGE_SSIDENT_LEN+1);
    entry.gnom = stage->gnom;
    entry.gcalib = stage->gcalib;
    strncpy(entry.iunits, LookupUnits(stage->iunits), DCCDB_STAGE_UNITS_LEN+1);
    strncpy(entry.ounits, LookupUnits(stage->ounits), DCCDB_STAGE_UNITS_LEN+1);
    entry.izero = stage->izero;
    entry.decifac = stage->decifac;
    entry.srate = stage->samprate;
    entry.leadfac = stage->leadfac;
    strncpy(entry.dir, stage->dir, DCCDB_STAGE_DIR_LEN+1);
    strncpy(entry.dfile, stage->dfile, DCCDB_STAGE_DFILE_LEN+1);
    entry.lddate = stage->lddate;
    entry.isduplicate = FALSE;

    return listAppend(head, &entry, sizeof(DCCDB_STAGE));
}

static BOOL IsDuplicateStage(DCCDB_STAGE *a, DCCDB_STAGE *b)
{
static char *fid = "IsDuplicateStage";

    if (strcmp(a->sta,     b->sta    ) != 0) return FALSE;
    if (strcmp(a->chn,     b->chn    ) != 0) return FALSE;
    if (strcmp(a->loc,     b->loc    ) != 0) return FALSE;
    if (a->stageid != b->stageid) return FALSE;
    if ((long) a->begt != (long) b->begt) return FALSE;
    if ((long) a->endt != (long) b->endt) return FALSE;

    LogMsg(2, "%s: A = %s\n", fid, dccdbStageString(a, NULL));
    LogMsg(2, "%s: B = %s\n", fid, dccdbStageString(b, NULL));
    LogMsg(2, "%s: A and B match:\n", fid);

    return TRUE;
}

static void CopyDfile(char *cwd, char *dir, char *dfile)
{
char src[MAXPATHLEN+1], dst[MAXPATHLEN+1], *subdir;
static char *fid = "CopyDfile";

    if (strcmp(dir, "-") == 0 || strcmp(dfile, "-") == 0) return;

    sprintf(src, "%s/%s", dir, dfile);
    subdir = utilBasename(dir);
    sprintf(dir, "%s/%s", cwd, subdir); free(subdir);
    sprintf(dst, "%s/%s", dir, dfile);
    util_mkpath(dir, 0775);
    if (!utilCopyfile(dst, src, 0)) {
        LogErr("%s: utilCopyfile('%s', '%s', 0): %s\n", fid, dst, src, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
}

void BuildStage(OLD *old, DCCDB *new, char *prefix)
{
int i, j;
FILE *fp;
OLD_SEEDLOC *seedloc;
LNKLST *nlist, *slist;
char path[MAXPATHLEN+1];
char cwd[DCCDB_STAGE_DIR_LEN+1];
static char *fid = "BuildStage";

/* Figure out our current directory so we can rewrite the path names */

    if (getcwd(cwd, DCCDB_STAGE_DIR_LEN+1) == NULL) {
        LogErr("%s: ERROR: getcwd: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

/* We don't know in advance how many new stage records we'll need, so we'll append to a linked list first */

    if ((slist = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

/* Generate one new stage record for every sta/chn/loc match with seedloc */

    for (i = 0; i < old->nstage; i++) {
        if ((nlist = GenerateStaChnLocTuplesForNewStage(old, &old->stage[i])) != NULL) {
            for (j = 0; j < nlist->count; j++) {
                seedloc = (OLD_SEEDLOC *) nlist->array[j];
                if (!AddNewStageEntry(new, &old->stage[i], seedloc, slist)) {
                    LogErr("%s: AddNewStageEntry: %s", fid, strerror(errno));
                    GracefulExit(MY_MOD_ID + 4);
                }
            }
            listDestroy(nlist);
        }
    }
    if (!listSetArrayView(slist)) {
        LogErr("%s: listSetArrayView: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }

/* Now we can build our new->stage array */

    if ((new->stage = (DCCDB_STAGE *) malloc (sizeof(DCCDB_STAGE) * slist->count)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 6);
    }

    for (i = 0; i < slist->count; i++) new->stage[i] = *(DCCDB_STAGE *) slist->array[i];
    new->nstage = slist->count;

/* Sort it into sta/begt/chn/loc/stage order */

    qsort(new->stage, new->nstage, sizeof(DCCDB_STAGE), StageCompare);

/* Identify any duplicate records */

    for (i = 0; i < new->nstage-1; i++) {
        for (j = i+1; !new->stage[i].isduplicate && j < new->nstage; j++) {
            new->stage[i].isduplicate = IsDuplicateStage(&new->stage[i], &new->stage[j]);
        }
    }

/* And write out the table */

    sprintf(path, "%s.%s", prefix, DCCDB_STAGE_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 7);
    }

    for (i = 0; i < new->nstage; i++) {
        if (new->stage[i].isduplicate) {
            LogMsg(2, "%s: DROP record %d as duplicate: %s\n", fid, i, dccdbStageString(&new->stage[i], NULL));
        } else {
            CopyDfile(cwd, new->stage[i].dir, new->stage[i].dfile);
            dccdbPrintStageRecord(fp, &new->stage[i]);
        }
    }
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: stage.c,v $
 * Revision 1.2  2015/03/06 21:44:15  dechavez
 * initial production release
 *
 */
