#pragma ident "$Id: client.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Manage the CLIENT list
 *
 *====================================================================*/
#include "wgmcd.h"

#define MY_MOD_ID WGMCD_MOD_CLIENT

static struct {
    UINT32 num;
    CLIENT *client;
} ClientList;

/* Close a connection with a client */

VOID CloseClientConnection(CLIENT *client)
{

/* If we are breaking this connection, tell the client why */

    if (client->status != WGMCD_STATUS_CLIENT_ABORT) iacpSendAlert(client->iacp, client->result);

/* Close the socket and clear the handle */

    iacpClose(client->iacp);
    client->iacp = NULL;

    isidlCloseDiskLoop(client->dl);
    client->dl = NULL;

    client->status = WGMCD_STATUS_IDLE;
}

/* Grab an available slot */

CLIENT *NextAvailableClient(IACP *iacp)
{
CLIENT *client;
UINT32 i, MinimumPollInterval;

    for (client = NULL, i = 0; client == NULL && i < ClientList.num; i++) {
        MUTEX_LOCK(&ClientList.client[i].mutex);
            if (ClientList.client[i].iacp == NULL) {
                client = &ClientList.client[i];
                client->iacp   = iacp;
                client->status = WGMCD_STATUS_RECV;
                client->result = IACP_ALERT_NONE;
                client->ident  = iacpPeerIdent(iacp);
                MinimumPollInterval = iacpGetTimeoutInterval(client->iacp) / 2;
                if (MinimumPollInterval < DEFAULT_POLL_INTERVAL) {
                    client->interval.poll = MinimumPollInterval;
                } else {
                    client->interval.poll = DEFAULT_POLL_INTERVAL;
                }
                client->interval.hbeat = MinimumPollInterval;
            }
        MUTEX_UNLOCK(&ClientList.client[i].mutex);
    }

    return client;
}

static BOOL InitClient(CLIENT *client, int index, PARAM *par)
{
    MUTEX_INIT(&client->mutex);
    client->index         = index;
    client->iacp          = (IACP *) NULL;
    client->ident         = (char *) NULL;
    client->status        = WGMCD_STATUS_IDLE;
    client->result        = IACP_ALERT_NONE;
    client->send.buflen   = par->buflen.send;
    client->recv.buflen   = par->buflen.recv;
    client->temp.buflen   = par->buflen.send * 2; /* bigger to handle compression failure */
    client->dl            = NULL;
    if ((client->send.buf = (UINT8 *) malloc(client->send.buflen)) == NULL) return FALSE;
    if ((client->recv.buf = (UINT8 *) malloc(client->recv.buflen)) == NULL) return FALSE;
    if ((client->temp.buf = (UINT8 *) malloc(client->temp.buflen)) == NULL) return FALSE;
    client->glob          = &par->glob;

    return TRUE;
}

VOID InitClientList(PARAM *par)
{
UINT32 i;
static char *fid = "InitClientList";

    ClientList.num = par->maxclient;
    ClientList.client = (CLIENT *) calloc(1,ClientList.num * sizeof(CLIENT));
    if (ClientList.client == NULL) {
        LogMsg(LOG_ERR, "%s: calloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    for (i = 0; i < ClientList.num; i++) if (!InitClient(&ClientList.client[i], i, par)) {
        LogMsg(LOG_ERR, "%s: InitClient: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: client.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
