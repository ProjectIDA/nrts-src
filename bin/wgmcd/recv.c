#pragma ident "$Id: recv.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Process a WG_IACP_WGID message
 *
 *====================================================================*/
#include "wgmcd.h"

VOID Process_WG_IACP_WGID(CLIENT *client, char *site)
{
BOOL ok;
LOGIO *lp;
WG_OBSPKT_SEQNO seqno;
static char *fid = "Process_WG_IACP_WGID";

/* open the associated disk loop */

    lp = iacpGetLogio(client->iacp);
    client->dl = isidlOpenDiskLoop(client->glob, site, lp, ISI_RDWR);

/* we had better have a disk loop! */

    if (client->dl == NULL) {
        LogMsg(LOG_ERR, "%s: can't open `%s' disk loop for read/write!", fid, site);
        client->status = WGMCD_STATUS_SERVER_ABORT;
        client->result = IACP_ALERT_SERVER_FAULT;
        return;
    }
    LogMsg(LOG_INFO, "'%s' disk loop opened for read/write", site);

/* get the expected sequence number for the next packet */

    if (!GetNextSeqno(client->dl, &seqno)) {
        LogMsg(LOG_ERR, "%s: GetNextSeqno: %s", fid, strerror(errno));
        client->status = WGMCD_STATUS_SERVER_ABORT;
        client->result = IACP_ALERT_SERVER_FAULT;
        return;
    }

/* send it to the wave glider */

    client->send.frame.payload.data = client->send.buf;
    client->send.frame.payload.type = WG_IACP_SEQNO;
    client->send.frame.payload.len = wgPackSeqno(client->send.frame.payload.data, &seqno);

    ok = iacpSendFrame(client->iacp, &client->send.frame);
    if (!ok && errno == EPIPE) {
        errno = ECONNRESET;
        client->status = WGMCD_STATUS_CLIENT_ABORT;
        LogMsg(LOG_ERR, "%s: iacpSendFrame: %s", fid, strerror(errno));
    }

    return;
}

VOID Process_WG_IACP_OBSPKT(CLIENT *client, WG_WGOBS1 *wgobs1)
{
BOOL ok;
ISI_RAW_PACKET *raw;
ISI_DL_OPTIONS options;
static char *fid = "Process_WG_IACP_OBSPKT";

    if ((raw = isiAllocateRawPacket(client->dl->sys->maxlen)) == NULL) {
        LogMsg(LOG_ERR, "%s: isiAllocateRawPacket(%d): %s", fid, client->dl->sys->maxlen, strerror(errno));
        client->status = WGMCD_STATUS_SERVER_ABORT;
        client->result = IACP_ALERT_SERVER_FAULT;
    }

    strlcpy(raw->hdr.site, client->dl->sys->site, ISI_SITELEN+1);
    raw->hdr.len.used   = raw->hdr.len.native = wgPackWGOBS1(raw->payload, wgobs1);
    raw->hdr.desc.comp  = ISI_COMP_NONE;
    raw->hdr.desc.type  = ISI_TYPE_WGOBS1;
    raw->hdr.desc.order = ISI_ORDER_UNDEF;
    raw->hdr.desc.size  = sizeof(UINT8);
    raw->hdr.status     = ISI_RAW_STATUS_OK;

    isidlInitOptions(&options);
    options.flags = ISI_OPTION_GENERATE_SEQNO;
    BlockShutdown(fid);
        ok = isidlWriteToDiskLoop(client->dl, raw, &options);
    UnblockShutdown(fid);

    if (!ok) {
        LogMsg(LOG_ERR, "%s: isidlWriteToDiskLoop: %s", fid, strerror(errno));
        client->status = WGMCD_STATUS_SERVER_ABORT;
        client->result = IACP_ALERT_SERVER_FAULT;
    }
LogMsg(LOG_INFO, "write: seqno=%d, len=%d", wgobs1->seqno, wgobs1->len);


    isiDestroyRawPacket(raw);
}

/* Revision History
 *
 * $Log: recv.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
