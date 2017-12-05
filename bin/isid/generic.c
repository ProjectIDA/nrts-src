#pragma ident "$Id: generic.c,v 1.1 2010/08/27 18:55:56 dechavez Exp $"
/*======================================================================
 *
 *  Process generic report requests
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_GENERIC

VOID ProcessGenericReportReq(CLIENT *client)
{
UINT16 type;
LNKLST list;
static char *fid = "ProcessGenericReportReq";

    if (!listInit(&list)) {
        LogMsg(LOG_ERR, "%s: listInit: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    utilUnpackUINT16(client->recv.frame.payload.data, &type);

    client->result = IACP_ALERT_REQUEST_COMPLETE; /* unless we fail */

    switch (type) {
      case ISI_REPORT_TYPE_SOH:
      case ISI_REPORT_TYPE_CNF:
      case ISI_REPORT_TYPE_WFDISC:
        client->result = IACP_ALERT_UNSUPPORTED;
        break;
      case ISI_REPORT_TYPE_DLSYS:
        if (!isidlBuildDlSysReport(client->master, &list)) {
            LogMsg(LOG_ERR, "%s: isidlBuildDlSysReportList: %s", fid, strerror(errno));
            client->result = IACP_ALERT_SERVER_FAULT;
        }
        break;
      default:
        client->result = IACP_ALERT_UNSUPPORTED;
        break;
    }

    if (client->result == IACP_ALERT_REQUEST_COMPLETE) {
        if (!isiIacpSendReport(client->iacp, &list)) {
            LogMsg(LOG_ERR, "%s: isiIacpSendReportList: error %d", client->ident, errno);
        }
        LogMsg(LOG_DEBUG, "%s: %lu type %d report records sent", client->ident, type, list.count);
    }

    listDestroy(&list);
    client->finished = TRUE;
}

/* Revision History
 *
 * $Log: generic.c,v $
 * Revision 1.1  2010/08/27 18:55:56  dechavez
 * initial release (only supports DL_SYS reports)
 *
 */
