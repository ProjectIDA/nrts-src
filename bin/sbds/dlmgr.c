#pragma ident "$Id: dlmgr.c,v 1.2 2015/07/17 18:56:34 dechavez Exp $"
/*======================================================================
 *
 *  Manage IDA10 disk loops
 *
 *====================================================================*/
#include "sbds.h"
#include "addoss.h"

static ISI_GLOB *glob;
static LNKLST head;
static ISI_DL *sbddl; /* the SBD disk loop */

typedef struct {
    char *site;
    ISI_DL *dl;
} ENTRY;

static void InitSeedLinkOption(ISI_DL *dl, char *net)
{
char cfgstr[MAXPATHLEN];
static char *default_seedlink = "localhost:16000:512:500";
static char *fid = "InitSeedLinkOption";

    sprintf(cfgstr, "%s:%s", default_seedlink, net);
    if (!isidlSetSeedLinkOption(dl, cfgstr, "sbds", ISI_DL_DEFAULT_SLINK_DEBUG)) {
        LogMsg(LOG_ERR, "*** IGNORED ERROR *** %s: isidlSetSeedLinkOption: %s", fid, strerror(errno));
        return;
    }
}

/* Close all open disk loops */

void CloseAllDiskLoops(void)
{
LNKLST_NODE *crnt;
ENTRY *entry;

    LogMsg(LOG_INFO, "closing '%s' SBD disk loop", sbddl->sys->site);
    isidlCloseDiskLoop(sbddl);

    crnt = listFirstNode(&head);
    while (crnt != NULL) {
        entry = (ENTRY *) crnt->payload;
        LogMsg(LOG_INFO, "closing '%s' IDA10 disk loop", entry->site);
        isidlCloseDiskLoop(entry->dl);
        crnt = listNextNode(crnt);
    }
}

/* Return handle to IDA10 disk loop, opening if needed */

static ISI_DL *OpenIDA10dl(char *site, char *net, BOOL seedlink)
{
LOGIO *lp;
ISI_DL *dl;
ENTRY entry;
static UINT32 options = ISI_OPTION_GENERATE_SEQNO | ISI_OPTION_INSERT_32BIT_SEQNO | ISI_OPTION_INSERT_32BIT_TSTAMP;
static char *fid = "OpenIDA10dl";

    lp = LogPointer();

    if ((entry.site = strdup(site)) == NULL) {
        LogMsg(LOG_ERR, "*** IGNORED ERROR *** %s: strdup: %s", fid, strerror(errno));
        return NULL;
    }

    if ((entry.dl = isidlOpenDiskLoop(glob, site, lp, ISI_RDWR, options)) == NULL) {
        LogMsg(LOG_ERR, "*** IGNORED ERROR *** %s: can't open '%s' IDA10 disk loop", fid, site);
        return NULL;
    }
    LogMsg(LOG_INFO, "ISI disk loop '%s' opened for IDA10 read/write", site);
    if (seedlink) InitSeedLinkOption(entry.dl, net);
    isidlLogSeedLinkOption(lp, LOG_INFO, entry.dl);

    if (!listAppend(&head, &entry, sizeof(ENTRY))) {
        LogMsg(LOG_ERR, "*** IGNORED ERROR *** %s: listAppend: %s", fid, strerror(errno));
        return NULL;
    }

    if ((dl = GetIDA10dl(site, net, seedlink)) == NULL) {
        LogMsg(LOG_ERR, "*** IGNORED ERROR *** %s: can't get back my own DL???", fid);
        return NULL;
    }

    return dl;
}

ISI_DL *GetIDA10dl(char *site, char *net, BOOL seedlink)
{
LNKLST_NODE *crnt;
ENTRY *entry;

    crnt = listFirstNode(&head);
    while (crnt != NULL) {
        entry = (ENTRY *) crnt->payload;
        if (strcmp(entry->site, site) == 0) return entry->dl;
        crnt = listNextNode(crnt);
    }

    return OpenIDA10dl(site, net, seedlink);
}

void InitDLmgr(PARAM *par)
{
    glob = &par->glob;
    sbddl = InitSBDdl(par); /* Open SBD disk loop */
    listInit(&head);
}

/* Revision History
 *
 * $Log: dlmgr.c,v $
 * Revision 1.2  2015/07/17 18:56:34  dechavez
 * rework to support move of options into ISI_DL handle
 *
 * Revision 1.1  2013/05/11 23:03:58  dechavez
 * initial release
 *
 */
