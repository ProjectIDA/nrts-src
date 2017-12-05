#pragma ident "$Id: client.c,v 1.2 2013/05/11 23:05:38 dechavez Exp $"
/*======================================================================
 *
 *  Manage the CLIENT list
 *
 *====================================================================*/
#include "sbds.h"

#define MY_MOD_ID SBDS_MOD_CLIENT

static struct {
    UINT32 num;
    CLIENT *client;
} ClientList;

/* Close a connection with a client */

void CloseClientConnection(CLIENT *client)
{

/* Close the socket and clear the handle */

    client->sbd = sbdClose(client->sbd);
    client->status = SBDS_STATUS_IDLE;
}

/* Grab an available slot */

CLIENT *NextAvailableClient(SBD *sbd)
{
CLIENT *client;
UINT32 i;

    for (client = NULL, i = 0; client == NULL && i < ClientList.num; i++) {
        MUTEX_LOCK(&ClientList.client[i].mutex);
            if (ClientList.client[i].sbd == NULL) {
                client = &ClientList.client[i];
                client->sbd   = sbd;
                client->status = SBDS_STATUS_RECV;
                client->ident  = sbdPeerIdent(sbd);
            }
        MUTEX_UNLOCK(&ClientList.client[i].mutex);
    }

    return client;
}

static BOOL InitClient(CLIENT *client, int index, PARAM *par)
{
    MUTEX_INIT(&client->mutex);
    client->index         = index;
    client->sbd           = (SBD *) NULL;
    client->ident         = (char *) NULL;
    client->status        = SBDS_STATUS_IDLE;

    return TRUE;
}

void InitClientList(PARAM *par)
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
 * Revision 1.2  2013/05/11 23:05:38  dechavez
 * VOID -> void
 *
 * Revision 1.1  2013/03/11 23:04:15  dechavez
 * initial release
 *
 */
