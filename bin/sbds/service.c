#pragma ident "$Id: service.c,v 1.4 2013/05/11 23:05:17 dechavez Exp $"
/*======================================================================
 *
 *  Service a single client connection.
 *
 *====================================================================*/
#include "sbds.h"

void LogRecvError(SBD *sbd)
{
int error, LogLevel;

    error = sbdGetRecvError(sbd);

    switch(error) {
      case ECONNRESET:
        LogMsg(LOG_DEBUG, "%s: connection reset by peer", sbdPeerIdent(sbd));
        LogLevel = LOG_DEBUG;
        break;

      default:
        LogLevel = LOG_INFO;
        break;
    }

    LogMsg(LogLevel, "%s: sbdRecvFrame: error=%d", sbdPeerIdent(sbd), error);
}


static void ProcessMessage(CLIENT *client, SBD_MESSAGE *message)
{
int len, code;
UINT8 myid[5];
char  imei[16];
static char *fid = "ProcessMessage";

    LogMsg(LOG_INFO, "%d bytes received from %s\n", message->len, client->ident);
    SaveMessage(message);
    if (message->mt.header.valid) {
        memcpy(myid, message->mt.header.myid, 5);
        memcpy(imei, message->mt.header.imei, 16);
        sbdClearMessage(message);
        message->mt.confirm.cdr = 0xabcdef12;
        memcpy(message->mt.confirm.myid, myid, 5);
        memcpy(message->mt.confirm.imei, imei, 16);
        message->mt.confirm.status = 1;
        message->mt.confirm.valid = TRUE;
        sbdPackMessage(message);
        if (!sbdSendMessage(client->sbd, message)) LogMsg(LOG_INFO, "%s: error sending MT_CONFIRM: %s", fid, strerror(errno));
    }
}

/* Thread to service one client */

static THREAD_FUNC ServiceThread(void *argptr)
{
CLIENT *client;
UINT64 count = 0;
SBD_MESSAGE message;
static char *fid = "ServiceThread";

    LogMsg(LOG_DEBUG, "%s: thread started, id = 0x%x", fid, THREAD_SELF());

    client = (CLIENT *) argptr;
    client->status = SBDS_STATUS_RECV;

/* Read and process one SBD message */

    if (sbdRecvMessage(client->sbd, &message)) ProcessMessage(client, &message);

    CloseClientConnection(client);
    LogMsg(LOG_DEBUG, "%s: thread 0x%x exits", fid, THREAD_SELF());
    THREAD_EXIT((void *) 0);
}

/* Break a newly established connection w/o servicing it */

void BreakNewConnection(SBD *sbd)
{
    LogMsg(LOG_INFO, "%s: connection aborted", sbdPeerIdent(sbd));
    sbdClose(sbd);
}

/* Service a new connection */

void ServiceConnection(SBD *sbd)
{
THREAD tid;
CLIENT *client;
static CHAR *fid = "ServiceConnection";

    BlockOnShutdown();

/* Grab the next available CLIENT slot */

    if ((client = NextAvailableClient(sbd)) == NULL) {
        LogMsg(LOG_WARN, "WARNING: new connection rejected (threshold reached)");
        BreakNewConnection(sbd);
        return;
    }

/* And leave behind a thread to deal with it */

    if (!THREAD_CREATE(&tid, ServiceThread, client)) {
        LogMsg(LOG_ERR, "%s: %s: THREAD_CREATE: %s", client->ident, fid, strerror(errno));
        BreakNewConnection(sbd);
        return;
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: service.c,v $
 * Revision 1.4  2013/05/11 23:05:17  dechavez
 * VOID -> void
 *
 * Revision 1.3  2013/03/15 17:48:34  dechavez
 * minor change to message received log entry
 *
 * Revision 1.2  2013/03/13 21:34:12  dechavez
 * accomodate libsbd 1.2.0 SBD_MESSAGE layout
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
