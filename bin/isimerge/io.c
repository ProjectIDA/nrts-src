#pragma ident "$Id: io.c,v 1.2 2010/09/17 20:04:22 dechavez Exp $"
/*======================================================================
 *
 *  Remote ISI raw disk loop input
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isimerge.h"
#define MY_MOD_ID ISIMERGE_MOD_IO

/* Establish fresh data connection */

ISI *ConnectToServer(SITE_PAR *site, char *ident)
{
ISI *isi;
ISI_DATA_REQUEST *dreq;
static char *fid = "ConnectToServer";

    if ((dreq = isiAllocSimpleSeqnoRequest(&site->next, &site->end, site->name)) == NULL) {
        LogMsg(LOG_INFO, "*** FATAL ERROR *** %s: isiAllocSimpleSeqnoRequest: %s", fid, strerror(errno));
        Abort(MY_MOD_ID + 1);
    }
    LogMsg(LOG_INFO, "contacting %s@%s:%d", site->name, site->server, site->isi_param.port);
    isi = isiInitiateDataRequest(site->server, &site->isi_param, dreq);
    isiFreeDataRequest(dreq);

    if (isi == NULL) {
        LogMsg(LOG_INFO, "unable to connect to %s@%s:%d", site->name, site->server, site->isi_param.port);
        return NULL;
    }

    sprintf(ident, "%s(%lu)@%s", site->name, isi->iacp->peer.pid, site->server);

    return isi;
}

ISI *CloseConnection(ISI *isi, char *ident, SITE_PAR *par)
{
int i;

    LogMsg(LOG_INFO, "closing connection to %s", ident);
    isiClose(isi);

    return (ISI *) NULL;
}

int ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw, UINT8 *buf, UINT32 buflen)
{
int status;
static char *fid = "ReadRawPacket";

    while (1) {
        status = isiReadFrame(isi, TRUE); // TRUE means skip heartbeats
        if (status != ISI_OK) return status;
        if (isi->frame.payload.type != ISI_IACP_RAW_PKT) {
            LogMsg(LOG_INFO, "unexpected type %d packet ignored", isi->frame.payload.type);
        } else {
            isiUnpackRawPacket(isi->frame.payload.data, raw);
            if (raw->hdr.len.native > buflen) {
                LogMsg(LOG_INFO, "Unexpected large (%lu > %lu) packet dropped", raw->hdr.len.native, buflen);
                LogMsg(LOG_INFO, "DROP: %s", isiRawHeaderString(&raw->hdr, buf));
            } else {
                if (isiDecompressRawPacket(raw, buf, buflen)) return ISI_OK;
                LogMsg(LOG_INFO, "DROP: %s: %s", isiRawHeaderString(&raw->hdr, buf), strerror(errno));
            }
        }
    }
}

/* Revision History
 *
 * $Log: io.c,v $
 * Revision 1.2  2010/09/17 20:04:22  dechavez
 * reordered stuff in failed attempt to figure out why the threads hang when run as a daemon,
 * changed site@server specification to eliminate the isi= prefix
 *
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
