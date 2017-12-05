#pragma ident "$Id: echo.c,v 1.1 2012/05/30 21:16:25 dechavez Exp $"
/*======================================================================
 *
 *  echo server, for testing IACP clients
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_ECHO

static THREAD_FUNC EchoClientThread(void *argptr)
{
CLIENT *client;
IACP_FRAME *frame;
static char *fid = "EchoClientThread";

    client = (CLIENT *) argptr;

    LogMsg(LOG_DEBUG, "%s: thread started, id = 0x%x", fid, THREAD_SELF());

    frame = &client->recv.frame;

    while (!client->finished) {

        if (iacpRecvFrame(client->iacp, frame, client->recv.buf, client->recv.buflen)) {

            if (frame->payload.type == IACP_TYPE_ALERT) {
                LogMsg(LOG_INFO, "echo %s: alert: %s\n", client->iacp->peer.ident, iacpAlertString(iacpAlertCauseCode(frame)));
            } else {
                LogMsg(LOG_INFO, "echo %s: type=%d, len=%d\n", client->iacp->peer.ident, frame->payload.type, frame->payload.len);
                logioHexDump(client->iacp->lp, LOG_INFO, frame->payload.data, frame->payload.len);
            }

            /* Make sure signature is OK */

            if (!frame->auth.verified) {
                LogMsg(LOG_ERR, "%s: authentication failed", client->ident);
                iacpSendAlert(client->iacp, IACP_ALERT_FAILED_AUTH);
                client->finished = TRUE;
            } else if (!iacpSendFrame(client->iacp, &client->recv.frame)) {
                if (errno == EPIPE) errno = ECONNRESET;
                LogMsg(LOG_INFO, "%s: iacpSendFrame: %s", client->ident,  strerror(errno));
                client->finished = TRUE;
            }

            client->finished = (frame->payload.type == IACP_TYPE_ALERT && iacpAlertCauseCode(frame) == IACP_ALERT_DISCONNECT);

        } else {

            LogRecvError(client->iacp);
            if (iacpGetRecvError(client->iacp) != ETIMEDOUT) iacpSendAlert(client->iacp, IACP_ALERT_IO_ERROR);
            client->finished = TRUE;

        }
    }

    CloseClientConnection(client);

    LogMsg(LOG_DEBUG, "%s: thread 0x%x exits", fid, THREAD_SELF());
    THREAD_EXIT((void *) 0);
}

static void ServiceEchoConnection(IACP *iacp)
{
THREAD tid;
CLIENT *client;
static char *fid = "ServiceEchoConnection";

/* Grab the next available CLIENT */

    if ((client = NextAvailableClient(iacp)) == NULL) {
        LogMsg(LOG_WARN, "WARNING: new connection rejected (threshold reached)");
        BreakNewConnection(iacp, IACP_ALERT_SERVER_BUSY);
        return;
    }

/* Leave behind a thread to deal with it */

    if (!THREAD_CREATE(&tid, EchoClientThread, client)) {
        LogMsg(LOG_ERR, "%s: %s: THREAD_CREATE: %s", client->ident, fid, strerror(errno));
        BreakNewConnection(iacp, IACP_ALERT_SERVER_FAULT);
        return;
    }
    THREAD_DETACH(tid);
}

static THREAD_FUNC EchoServer(void *argptr)
{
IACP *server, *client;
static char *fid = "EchoServer";

    LogMsg(LOG_DEBUG, "%s: thread started, id = %d", fid, THREAD_SELF());

    server = (IACP *) argptr;

/* Service each connection in its own thread */

    LogMsg(LOG_INFO, "listening for echo requests at port %hu", server->port);
    while (1) {
        BlockOnShutdown();
        if ((client = iacpAccept(server)) != NULL) {
            if (!ShutdownInProgress()) {
                LogMsg(LOG_INFO, "%s: connection established", client->peer.ident);
                ServiceEchoConnection(client);
            } else {
                iacpDisconnect(client, IACP_ALERT_SHUTDOWN);
            }
        } else {
            LogMsg(LOG_INFO, "incoming echo connection failed: %s (ignored)", strerror(errno));
        }
    }
}

VOID StartEchoServer(PARAM *par)
{
THREAD tid;
static IACP *server;
static char *fid = "StartEchoServer";

    if (par->echo == 0) return;

    server = iacpServer(par->echo, &par->attr, &par->lp, 1);
    if (server == (IACP *) NULL) {
        LogMsg(LOG_ERR, "Unable to start port %hu echo server: %s", par->nrts, strerror(errno));
        return;
    }

    if (!THREAD_CREATE(&tid, EchoServer, (void *) server)) {
        LogMsg(LOG_ERR, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 0);
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: echo.c,v $
 * Revision 1.1  2012/05/30 21:16:25  dechavez
 * created
 *
 */
