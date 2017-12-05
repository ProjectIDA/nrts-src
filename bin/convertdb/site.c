#pragma ident "$Id: site.c,v 1.2 2015/03/06 21:44:15 dechavez Exp $"
/*======================================================================
 *
 *  Build the new site table, from the old
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_SITE

void BuildSite(OLD *old, DCCDB *new, char *prefix)
{
int i;
FILE *fp;
char path[MAXPATHLEN+1];
static char *fid = "BuildSite";

    if ((new->site = (DCCDB_SITE *) malloc (sizeof(DCCDB_SITE) * old->nsite)) == NULL) {
        LogErr("%s: malloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    new->nsite = old->nsite;
    for (i = 0; i < old->nsite; i++) {
        strncpy(new->site[i].sta, old->site[i].sta, DCCDB_SITE_STA_LEN+1);
        strncpy(new->site[i].desc, old->site[i].staname, DCCDB_SITE_DESC_LEN+1);
        new->site[i].lat = old->site[i].lat;
        new->site[i].lon = old->site[i].lon;
        new->site[i].elev = old->site[i].elev;
        new->site[i].begt = dccdbOndateToBegt(old->site[i].ondate);
        new->site[i].endt = dccdbOffdateToEndt(old->site[i].offdate);
        new->site[i].lddate = old->site[i].lddate;
    }

    sprintf(path, "%s.%s", prefix, DCCDB_SITE_TABLE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 6);
    }

    for (i = 0; i < new->nsite; i++) dccdbPrintSiteRecord(fp, &new->site[i]);
    fclose(fp);
    LogMsg(1, "%s created OK\n", path);
}

/* Revision History
 *
 * $Log: site.c,v $
 * Revision 1.2  2015/03/06 21:44:15  dechavez
 * initial production release
 *
 */
