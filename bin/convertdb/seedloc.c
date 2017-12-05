#pragma ident "$Id: seedloc.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Build the new seedloc table, from the old
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_SEEDLOC

void BuildSeedloc(OLD *old, DCCDB *new, char *prefix)
{
int i;
FILE *fp;
char path[MAXPATHLEN+1];
static char *fid = "BuildSeedloc";

    if ((new->seedloc = (DCCDB_SEEDLOC *) malloc (sizeof(DCCDB_SEEDLOC) * old->nseedloc)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    new->nseedloc = old->nseedloc;
    for (i = 0; i < old->nseedloc; i++) {
        strncpy(new->seedloc[i].sta, old->seedloc[i].sta, DCCDB_SEEDLOC_STA_LEN+1);
        strncpy(new->seedloc[i].chn, old->seedloc[i].chan, DCCDB_SEEDLOC_CHN_LEN+1);
        strncpy(new->seedloc[i].seedchn, old->seedloc[i].newchan, DCCDB_SEEDLOC_SEEDCHN_LEN+1);
        strncpy(new->seedloc[i].loc, old->seedloc[i].loccode, DCCDB_SEEDLOC_LOC_LEN+1);
        new->seedloc[i].begt = old->seedloc[i].time;
        new->seedloc[i].endt = old->seedloc[i].endtime;
        new->seedloc[i].lddate = old->seedloc[i].lddate;
    }

    sprintf(path, "%s.%s", prefix, DCCDB_SEEDLOC_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 6);
    }

    for (i = 0; i < new->nseedloc; i++) dccdbPrintSeedlocRecord(fp, &new->seedloc[i]);
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: seedloc.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
