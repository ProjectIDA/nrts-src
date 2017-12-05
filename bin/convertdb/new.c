#pragma ident "$Id: new.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  NEW database stuff
 *
 *====================================================================*/
#include "convertdb.h"

static DCCDB *newdb = NULL;

static void FreeNewDB(DCCDB *new)
{
    if (new == NULL) return;
    if (new->b30   != NULL) free(new->b30);   new->b30   = NULL;
    if (new->b33   != NULL) free(new->b33);   new->b33   = NULL;
    if (new->b34   != NULL) free(new->b34);   new->b34   = NULL;
    if (new->site  != NULL) free(new->site);  new->site  = NULL;
    if (new->chan  != NULL) free(new->chan);  new->chan  = NULL;
    if (new->stage != NULL) free(new->stage); new->stage = NULL;
}

void CloseNewDatabase(void)
{
    if (newdb != NULL) {
        dbclose(newdb->table.abbrev);
        dbclose(newdb->table.site);
        dbclose(newdb->table.chan);
        dbclose(newdb->table.stage);
        dbclose(newdb->table.units);
    }

    FreeNewDB(newdb);
}

static void InitDB(DCCDB *new, LOGIO *lp)
{
    new->lp = lp;

    new->b30 = NULL;
    new->nb30 = 0;

    new->b33 = NULL;
    new->nb33 = 0;

    new->b34 = NULL;
    new->nb34 = 0;

    new->site = NULL;
    new->nsite = 0;

    new->chan = NULL;
    new->nchan = 0;

    new->stage = NULL;
    new->nstage = 0;
}

DCCDB *CreateNewDatabase(LOGIO *lp)
{
DCCDB *new;
static char *fid = "CreateNewDatabase";

    if ((new = (DCCDB *) malloc(sizeof(DCCDB))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    InitDB(new, lp);
    dccdbBuildB30(new);

    newdb = new;
    return new;
}

/* Revision History
 *
 * $Log: new.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
