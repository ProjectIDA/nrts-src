#pragma ident "$Id: report.c,v 1.1 2010/08/27 18:48:35 dechavez Exp $"
/*======================================================================
 *
 *  Generate various reports
 *
 *====================================================================*/
#include "isi/dl.h"

BOOL isidlBuildDlSysReport(ISI_DL_MASTER *master, LNKLST *list)
{
int i;
INT64 start, write, now;
ISI_REPORT_ENTRY report;
static char *fid = "isidlBuildDlSysReport";

    if (master == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    now = utilTimeStamp();
    
    for (i = 0; i < master->ndl; i++) {
        if (master->dl[i]->sys != NULL) {
            start = master->dl[i]->sys->tstamp.start; /* preserve */
            write = master->dl[i]->sys->tstamp.write; /* preserve */
            master->dl[i]->sys->tstamp.start = now - start; /* convert start to uptime */
            master->dl[i]->sys->tstamp.write = now - write; /* convert write to latency */
            report.type = ISI_REPORT_TYPE_DLSYS;
            report.len  = isidlPackDLSys(report.payload, master->dl[i]->sys);
            if (!listAppend(list, (VOID *) &report, (UINT32) sizeof(ISI_REPORT_ENTRY))) return FALSE;
            master->dl[i]->sys->tstamp.start = start; /* restore */
            master->dl[i]->sys->tstamp.write = write; /* restore */
        }
    }

    if (!listSetArrayView(list)) {
        logioMsg(master->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        return FALSE;
    }
    return TRUE;
}

/* Revision History
 *
 * $Log: report.c,v $
 * Revision 1.1  2010/08/27 18:48:35  dechavez
 * initial release
 *
 */
