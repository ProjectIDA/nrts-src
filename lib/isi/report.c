#pragma ident "$Id: report.c,v 1.1 2010/08/27 18:52:28 dechavez Exp $"
/*======================================================================
 *
 *  Convenience functions for ISI/IACP reports
 *
 *====================================================================*/
#include "isi.h"
#include "util.h"

#ifdef ISI_SERVER

/* Send a single record via IACP */

static BOOL SendReportEntry(IACP *iacp, ISI_REPORT_ENTRY *report, UINT8 *buf)
{
IACP_FRAME frame;

    frame.payload.type = ISI_IACP_REPORT_ENTRY;
    frame.payload.data = buf;
    frame.payload.len  = isiPackReportEntry(frame.payload.data, report);

    return iacpSendFrame(iacp, &frame);
}

/* Send a list of report entries via IACP */

BOOL isiIacpSendReport(IACP *iacp, LNKLST *list)
{
LNKLST_NODE *crnt;
#define LOCAL_BUFLEN (sizeof(ISI_REPORT_ENTRY) * 2)
UINT8 buf[LOCAL_BUFLEN];

    if (iacp == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    /* send over each element of the list */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        if (!SendReportEntry(iacp, (ISI_REPORT_ENTRY *) crnt->payload, buf)) return FALSE;
        crnt = listNextNode(crnt);
    }

    /* send over a NULL to signal the end of the list */

    return iacpSendNull(iacp);
}

#endif /* ISI_SERVER */

/* Receive a list of report entries via IACP */

BOOL isiIacpRecvReport(IACP *iacp, LNKLST *list)
{
time_t begin, ElapsedTime;
IACP_FRAME frame;
ISI_REPORT_ENTRY entry;
#define LOCAL_BUFLEN (sizeof(ISI_REPORT_ENTRY) * 2)
UINT8 buf[LOCAL_BUFLEN];

    begin = time(NULL);

    do {

        if (!iacpRecvFrame(iacp, &frame, buf, LOCAL_BUFLEN)) return FALSE;

        switch (frame.payload.type) {

          case ISI_IACP_REPORT_ENTRY:
            isiUnpackReportEntry(frame.payload.data, &entry);
            if (!listAppend(list, &entry, sizeof(ISI_REPORT_ENTRY))) return FALSE;
            begin = time(NULL);
            break;

          case IACP_TYPE_NULL:
            return listSetArrayView(list);
            break;

          case IACP_TYPE_NOP:
            break;

          case IACP_TYPE_ENOSUCH:
            errno = UNSUPPORTED;
            return FALSE;

          case IACP_TYPE_ALERT:
            errno = ECONNABORTED;
            return FALSE;
        }

        ElapsedTime = time(NULL) - begin;

    } while (ElapsedTime < (time_t) iacpGetTimeoutInterval(iacp));

    errno = ETIMEDOUT;
    return FALSE;
}

LNKLST *isiRequestReport(ISI *isi, UINT16 type)
{
LNKLST *list;
static char *fid = "isiRequestReport";

    if (isi == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((list = listCreate()) == NULL) return NULL;

    if (!iacpSendUINT16(isi->iacp, ISI_IACP_REQ_REPORT, type)) {
        isiLogMsg(isi, LOG_INFO, "%s: iacpSendUINT16 failed", fid);
        listDestroy(list);
        return NULL;
    }

    if (!isiIacpRecvReport(isi->iacp, list)) {
        isiLogMsg(isi, LOG_INFO, "%s: isiIacpRecvReport failed", fid);
        listDestroy(list);
        return NULL;
    }

    return list;
}

/* Revision History
 *
 * $Log: report.c,v $
 * Revision 1.1  2010/08/27 18:52:28  dechavez
 * initial release
 *
 */
