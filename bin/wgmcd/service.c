#pragma ident "$Id: service.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Service a single client connection.
 *
 *====================================================================*/
#include "wgmcd.h"

static VOID LogUnexpectedFrame(CLIENT *client, UINT32 type)
{
    LogMsg(LOG_INFO, "%s: unexpected frame type %lu (0x%08x) received",
        client->ident, type, type
    );
}

VOID LogRecvError(IACP *iacp)
{
int error, status, LogLevel;

    error = iacpGetRecvError(iacp);
    status = iacpGetRecvStatus(iacp);

    switch(error) {
      case ECONNRESET:
        LogMsg(LOG_DEBUG, "%s: connection reset by peer", iacpPeerIdent(iacp));
        LogLevel = LOG_DEBUG;
        break;

      default:
        LogLevel = LOG_INFO;
        break;
    }

    LogMsg(LogLevel, "%s: iacpRecvFrame: error=%d, status=%d", iacpPeerIdent(iacp), error, status);
}

static VOID EchoClientMessage(CLIENT *client)
{
}

static VOID ProcessClientMessage(CLIENT *client)
{
int len, code;
WG_WGOBS1 wgobs1;
char site[WG_STALEN+1];

static char *fid = "ProcessClientMessage";

    switch (client->recv.frame.payload.type) {

/* quit now if the client has sent us a break */

      case IACP_TYPE_ALERT:
        LogMsg(LOG_DEBUG, "%s: IACP_TYPE_ALERT", client->ident);
        code = iacpAlertCauseCode(&client->recv.frame);
        if (code == IACP_ALERT_DISCONNECT) {
            LogMsg(LOG_INFO, "%s: connection reset by peer", client->ident);
        } else {
            LogMsg(LOG_INFO, "%s: connection reset by peer (code=%lu)", client->ident, code);
        }
        client->status = WGMCD_STATUS_CLIENT_ABORT;
        return;

/* Supported client messages */

      case WG_IACP_WGID:
        LogMsg(LOG_INFO, "%s: WG_IACP_WGID", client->ident);
        len = client->recv.frame.payload.len > WG_STALEN ? WG_STALEN : client->recv.frame.payload.len;
        wgUnpackWGID(client->recv.frame.payload.data, site, len);
        Process_WG_IACP_WGID(client, site);
        return;

      case WG_IACP_OBSPKT:
        LogMsg(LOG_INFO, "%s: WG_IACP_OBSPKT", client->ident);
        wgUnpackWGOBS1(client->recv.frame.payload.data, &wgobs1);
        Process_WG_IACP_OBSPKT(client, &wgobs1);
        return;

/* NULL means client is done sending everything it has and is waiting for anything from us */

      case IACP_TYPE_NULL:
        LogMsg(LOG_INFO, "%s: IACP_TYPE_NULL", client->ident);
        client->status = WGMCD_STATUS_SEND; /* now it's our turn */
        return;

/* Give up if we don't know what it is */

      default:
        LogUnexpectedFrame(client, client->recv.frame.payload.type);
        client->status = WGMCD_STATUS_SERVER_ABORT;
        client->result = IACP_ALERT_UNSUPPORTED;
    }
}

/* Thread to service one client */

static THREAD_FUNC ServiceThread(void *argptr)
{
CLIENT *client;
UINT64 count = 0;
static char *fid = "ServiceThread";

    LogMsg(LOG_DEBUG, "%s: thread started, id = 0x%x", fid, THREAD_SELF());

    client = (CLIENT *) argptr;
    client->status = WGMCD_STATUS_RECV;

/* absorb everything the client has to send us */

    while (client->status == WGMCD_STATUS_RECV) {

        if (iacpRecvFrame(client->iacp, &client->recv.frame, client->recv.buf, client->recv.buflen)) {

            /* Make sure signature is OK */

            if (!client->recv.frame.auth.verified) {
                LogMsg(LOG_ERR, "%s: authentication failed", client->ident);
                client->status = WGMCD_STATUS_SERVER_ABORT;
                client->result = IACP_ALERT_FAILED_AUTH;
            }

            /* provide the service */

            ProcessClientMessage(client);

        } else {

            LogRecvError(client->iacp);
            if (iacpGetRecvError(client->iacp) != ETIMEDOUT) {
                client->status = WGMCD_STATUS_SERVER_ABORT;
                client->result = IACP_ALERT_IO_ERROR;
            } else {
                client->status = WGMCD_STATUS_CLIENT_ABORT;
            }
        }
    }

    if (client->status == WGMCD_STATUS_SEND) SendOutboundCommands(client);

    CloseClientConnection(client);

    LogMsg(LOG_DEBUG, "%s: thread 0x%x exits", fid, THREAD_SELF());
    THREAD_EXIT((void *) 0);
}

/* Break a newly established connection w/o servicing it */

VOID BreakNewConnection(IACP *iacp, UINT32 cause)
{
    LogMsg(LOG_INFO, "%s: connection aborted", iacpPeerIdent(iacp));
    iacpDisconnect(iacp, cause);
}

/* Service a new connection */

VOID ServiceConnection(IACP *iacp)
{
THREAD tid;
CLIENT *client;
static CHAR *fid = "ServiceConnection";

    BlockOnShutdown();

/* Grab the next available CLIENT slot */

    if ((client = NextAvailableClient(iacp)) == NULL) {
        LogMsg(LOG_WARN, "WARNING: new connection rejected (threshold reached)");
        BreakNewConnection(iacp, IACP_ALERT_SERVER_BUSY);
        return;
    }

/* Otherwise, leave behind a thread to deal with it */

    if (!THREAD_CREATE(&tid, ServiceThread, client)) {
        LogMsg(LOG_ERR, "%s: %s: THREAD_CREATE: %s", client->ident, fid, strerror(errno));
        BreakNewConnection(iacp, IACP_ALERT_SERVER_FAULT);
        return;
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: service.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
