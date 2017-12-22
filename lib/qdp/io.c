#pragma ident "$Id: io.c,v 1.19 2017/10/11 20:40:27 dechavez Exp $"
/*======================================================================
 *
 * Low level QDP I/O
 *
 *====================================================================*/
#include "qdp.h"

static void DissectDACK(QDP *qp, QDP_PKT *pkt)
{
int i, j, bit;
UINT16 throttle;
UINT32 ack[4];
UINT8 *ptr;

    ptr = pkt->payload;
    ptr += utilUnpackUINT16(ptr, &throttle);
    ptr += sizeof(UINT16);
    for (i = 0; i < 4; i++) ptr += utilUnpackUINT32(ptr, &ack[i]);

    for (bit = 0, i = 0; i < 4; i++) {
        for (j = 0; j < 32; j++, bit++) {
            if (ack[i] & (1 << j)) qdpDebug(qp, "%s %hu acknowledged", qp->peer.ident, pkt->hdr.ack + bit);
        }
    }
}

void qdpHostToNet(QDP_PKT *pkt)
{
UINT8 *ptr;

    ptr = pkt->raw + 4; /* skip over CRC for now */
    *ptr++ = pkt->hdr.cmd;
    *ptr++ = pkt->hdr.ver;
    ptr += utilPackUINT16(ptr, pkt->hdr.dlen);
    ptr += utilPackUINT16(ptr, pkt->hdr.seqno);
    ptr += utilPackUINT16(ptr, pkt->hdr.ack);
    ptr += utilPackBytes(ptr, pkt->payload, pkt->hdr.dlen);
    pkt->hdr.crc = qdpCRC(pkt->raw + 4, pkt->hdr.dlen + QDP_CMNHDR_LEN - 4);
    utilPackUINT32(pkt->raw, pkt->hdr.crc);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpNetToHost(QDP_PKT *pkt)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    ptr = pkt->raw;
    ptr += qdpDecodeCMNHDR(ptr, &pkt->hdr);
    pkt->payload = ptr;
    pkt->len = pkt->hdr.dlen + QDP_CMNHDR_LEN;
}

MSGQ_MSG *qdpRecvPkt(QDP *qp, QIO *up, int *status)
{
int got;
QDP_PKT *pkt;
MSGQ_MSG *msg;
static char *fid = "qdpRecvPkt";

    msg = qdpGetEmptyPktMsg(fid, qp, MSGQ_WAIT);
    pkt = (QDP_PKT *) msg->data;
    got = qioRecv(up, (char *) pkt->raw, QDP_MAX_MTU);
    if (got > 0) {
        qdpNetToHost(pkt);
        if (qdpVerifyCRC(pkt)) {
            if (pkt->len == got) {
                qdpDebug(qp, "%s -> %s(%d, %d)", qp->peer.ident, qdpCmdString(pkt->hdr.cmd), pkt->hdr.seqno, pkt->hdr.ack);
            } else {
                qdpWarn(qp, "%s: WARNING: len=%d, got=%d", fid, pkt->len, got);
            }
            *status = QDP_OK;
        } else {
            *status = QDP_CRCERR;
            qdpReturnPktMsgToHeap(fid, qp, msg);
            msg = NULL;
        }
    } else if (got == 0) {
        *status = QDP_TIMEOUT;
        qdpReturnPktMsgToHeap(fid, qp, msg);
        msg = NULL;
    } else {
        *status = QDP_IOERROR;
        qdpError(qp, "%s I/O error (got=%d) reading %s: %s", qp->peer.ident, got, up->ident, strerror(errno));
        qdpReturnPktMsgToHeap(fid, qp, msg);
        msg = NULL;
    }

    return msg;
}

void qdpSendPkt(QDP *qp, int which, QDP_PKT *pkt)
{
QIO *up;
char *desc;
int port;
static char *ctrl = "ctrl";
static char *data = "data";
static char *fid = "qdpSendPkt";

    if (qp == NULL || pkt == NULL) return;

    if (which == QDP_CTRL) {
        up = &qp->ctrl;
        port = (qp->peer.addr == QDP_BROADCAST_IP) ? QDP_BROADCAST_PORT : qp->peer.port.ctrl;
        desc = ctrl;
    } else {
        up = &qp->data;
        port = (qp->peer.addr == QDP_BROADCAST_IP) ? QDP_BROADCAST_PORT : qp->peer.port.data;
        desc = data;
    }

    qdpHostToNet(pkt);
        MUTEX_LOCK(&qp->fsa.mutex);
            qioSend(up, qp->peer.addr, port, (char *) pkt->raw, pkt->len);
        MUTEX_UNLOCK(&qp->fsa.mutex);
    qdpNetToHost(pkt);
    qdpDebug(qp, "%s(%hu, %hu) -> %s (%s)", qdpCmdString(pkt->hdr.cmd), pkt->hdr.seqno, pkt->hdr.ack, qp->peer.ident, desc);
    if (pkt->hdr.cmd == QDP_DT_DACK) DissectDACK(qp, pkt);
}

#ifdef notdef
void qdpLogQdp(QDP *qdp)
{
    if (qdp == NULL) return;

    qdpLogStats(qdp);
    qioLogStats(LOG_INFO, &qdp->ctrl, "ctrl");
    qioLogStats(LOG_INFO, &qdp->ctrl, "data");
}
#endif /* notdef */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: io.c,v $
 * Revision 1.19  2017/10/11 20:40:27  dechavez
 * qdpRecvPkt() changed to log qioRecv() return value after failure
 *
 * Revision 1.18  2016/07/20 17:14:25  dechavez
 * qdpRecvPkt() now includes strerror() in I/O error message
 *
 * Revision 1.17  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.16  2011/02/03 17:49:46  dechavez
 * removed qdpClose()
 *
 * Revision 1.15  2011/01/31 21:12:15  dechavez
 * when ip is broadcast address, override qdpSendPkt() destination port with broadcast port
 *
 * Revision 1.14  2011/01/25 18:20:04  dechavez
 * switched to libqio calls instead of local qdpioX
 *
 * Revision 1.13  2011/01/11 17:29:12  dechavez
 * all libudpio structures and library calls changed to qdpio equivalents
 *
 * Revision 1.12  2010/12/06 17:26:04  dechavez
 * added some ifdef'd out code for possible future logging use
 *
 * Revision 1.11  2010/03/31 20:32:25  dechavez
 * removed trash link options, added call to qdpShutdown() in qdpClose()
 *
 * Revision 1.10  2010/03/22 21:39:25  dechavez
 * added qdpClose()
 *
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
