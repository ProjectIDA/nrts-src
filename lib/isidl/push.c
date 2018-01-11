#pragma ident "$Id: push.c,v 1.5 2018/01/11 18:49:29 dechavez Exp $"
/*======================================================================
 *
 *  server to ISI_PUSH clients
 *
 *====================================================================*/
#define INCLUDE_APP_SUPPLIED_WRITER
#include "isi/dl.h"

typedef struct {
    int port;
    LOGIO *lp;
    ISI_DL *dl;
    IACP *client;
    ISI_DL_SAVE_FUNC func;
} LOCAL_PAR;

static void DebugRawPacket(ISI_RAW_PACKET *raw)
{
IDA10_TS ts;
static char *fid = "isidlPacketServer:DebugRawPacket";

    if (raw->hdr.desc.type != ISI_TYPE_IDA10) {
        printf("%s: unexpected packet type: %d", fid, raw->hdr.desc.type);
        return;
    }

    switch (ida10Type(raw->payload)) {
      case IDA10_TYPE_TS:
        printf("TS: ");
        if (ida10UnpackTS(raw->payload, &ts)) {
            printf("%s\n", ida10TStoString(&ts, NULL));
        } else {
            printf("ida10UnpackTS: %s\n", strerror(errno));
        }
        break;
      default:
        printf("%s: unsupported IDA10 type '%d'\n", fid, ida10Type(raw->payload));
    }
}

static THREAD_FUNC ClientThread(void *argptr)
{
int error;
BOOL finished;
char *ident;
LOCAL_PAR *par;
ISI_RAW_PACKET raw;
IACP_FRAME frame;
UINT8 buf[IDA10_MAXRECLEN];
UINT64 count = 0;
static char *fid = "ClientThread";

    par = (LOCAL_PAR *) argptr;
    ident = iacpPeerIdent(par->client);
    logioMsg(par->lp, LOG_INFO, "connection from %s", ident);

    finished = FALSE;
    while (!finished) {


        if (iacpRecvFrame(par->client, &frame, buf, IDA10_MAXRECLEN)) {

           /* Make sure signature is OK */

            if (!frame.auth.verified) {
                iacpSendAlert(par->client, IACP_ALERT_FAILED_AUTH);
                logioMsg(par->lp, LOG_INFO, "%s: %s", ident, iacpAlertString(IACP_ALERT_FAILED_AUTH));
                finished = TRUE;
            }

            /* quit on client disconnects, save data packets and ignore everything else */

            switch (frame.payload.type) {
              case ISI_IACP_RAW_PKT:
                isiUnpackRawPacket(frame.payload.data, &raw);
                strcpy(raw.hdr.site, par->dl->sys->site); /* dl writer sanity check requires this */
                if (!(par->func)(par->dl, &raw)) {
                    logioMsg(par->lp, LOG_INFO, "%s: THREAD_EXIT: app supplied write function failed: %s", fid, strerror(errno));
                    THREAD_EXIT((void *) 0);
                }
                if (++count == 1) logioMsg(par->lp, LOG_INFO, "%s: initial packet received", ident);
                break;

              case IACP_TYPE_ALERT:
                logioMsg(par->lp, LOG_INFO, "%s: %s", ident, iacpAlertString(iacpAlertCauseCode(&frame)));
                finished = TRUE;
            }

        } else {
            logioMsg(par->lp, LOG_INFO, "%s: %s", ident, strerror(iacpGetRecvError(par->client)));
            finished = TRUE;
        }
    }

    logioMsg(par->lp, LOG_INFO, "%s: connection closed, %llu packets received", ident, count);
    iacpClose(par->client);
    logioMsg(par->lp, LOG_DEBUG, "%s: thread 0x%x exits", fid, THREAD_SELF());
    free(par);
    THREAD_EXIT((void *) 0);
}

static THREAD_FUNC ListenThread(void *argptr)
{
LOCAL_PAR *master, *par;
IACP *server;
THREAD tid;
static char *fid = "isidlPacketServer:ListenThread";

    master = (LOCAL_PAR *) argptr;
    if ((server = iacpServer(master->port, NULL, master->lp, FALSE)) == NULL) {
        logioMsg(master->lp, LOG_INFO, "%s: iacpServer: %s", fid, strerror(errno));
        logioMsg(par->lp, LOG_INFO, "%s: THREAD_EXIT", fid);
        THREAD_EXIT((void *) 0);
    }

    logioMsg(master->lp, LOG_INFO, "listening for incoming packet connections at port %d\n", master->port);
    while (1) {
        if ((par = (LOCAL_PAR *) malloc(sizeof(LOCAL_PAR))) == NULL) {
            logioMsg(par->lp, LOG_INFO, "%s: malloc: %s", fid, strerror(errno));
            logioMsg(par->lp, LOG_INFO, "%s: THREAD_EXIT", fid);
            THREAD_EXIT((void *) 0);
        }
        *par = *master;
        if ((par->client = iacpAccept(server)) != NULL) {
            if (!THREAD_CREATE(&tid, ClientThread, par)) {
                logioMsg(par->lp, LOG_INFO, "%s: THREAD_CREATE: %s", fid, strerror(errno));
                logioMsg(par->lp, LOG_INFO, "%s: THREAD_EXIT", fid);
                THREAD_EXIT((void *) 0);
            }
        } else {
            logioMsg(par->lp, LOG_INFO, "port %d: incoming connection failed: %s", par->port, strerror(errno));
            free(par);
        }
    }
}

BOOL isidlPacketServer(ISI_DL *dl, int port, LOGIO *lp, ISI_DL_SAVE_FUNC func)
{
THREAD tid;
LOCAL_PAR *par;

    if ((par = (LOCAL_PAR *) malloc(sizeof(LOCAL_PAR))) == NULL) return FALSE;

    par->dl   = dl;
    par->port = port;
    par->lp   = lp;
    par->func = func;
    if (!THREAD_CREATE(&tid, ListenThread, par)) return FALSE;

    return TRUE;
}

/* Revision History
 *
 * $Log: push.c,v $
 * Revision 1.5  2018/01/11 18:49:29  dechavez
 * changed format of incoming connection message
 *
 * Revision 1.4  2014/08/28 21:25:11  dechavez
 * rework to use user supplied function for writing data, and move of options into ISI_DL handle
 *
 * Revision 1.3  2013/07/19 17:53:04  dechavez
 * fixed bug that caused core dump when serving more than one push client
 *
 * Revision 1.2  2011/11/07 17:30:09  dechavez
 * accept ISI_DL_OPTIONS
 *
 * Revision 1.1  2011/10/12 17:18:15  dechavez
 * created
 *
 */
