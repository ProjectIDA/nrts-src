#pragma ident "$Id: push.c,v 1.4 2014/01/27 18:43:39 dechavez Exp $"
/*======================================================================
 *
 *  Relentless packet pusher to remote disk loop packet writer
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "isi.h"

static void SendFrame(ISI_PUSH *ph, IACP_FRAME *frame)
{
BOOL sent;

    MUTEX_LOCK(&ph->mutex);

        sent = FALSE;
        while (!ph->disabled && !sent) {

            if (ph->shutdown) {
                if (ph->iacp != NULL) iacpClose(ph->iacp);
                logioMsg(ph->log.lp, ph->log.level, "%s:%s push shutdown, %llu packets sent", ph->server, ph->port, ph->count);
                free(ph);
                THREAD_EXIT((void *) 0);
            }

            if (ph->iacp == NULL) {
                if ((ph->iacp = iacpOpen(ph->server, ph->port, &ph->attr, ph->log.lp, ph->debug)) == NULL) {
                    logioMsg(ph->log.lp, ph->log.level, "unable to connect to %s:%d", ph->server, ph->port);
                    if (!ph->attr.at_retry) ph->disabled = TRUE;
                    break;
                }
                logioMsg(ph->log.lp, ph->log.level, "connected to %s\n", ph->iacp->peer.ident);
            }

            sent = (frame != NULL) ? iacpSendFrame(ph->iacp, frame) : iacpHeartbeat(ph->iacp);

            if (!sent) {
                if (errno == EPIPE) errno = ECONNRESET;
                logioMsg(ph->log.lp, ph->log.level, "%s: %s", ph->iacp->peer.ident, strerror(errno));
                ph->iacp = iacpClose(ph->iacp);
                if (!ph->attr.at_retry) ph->disabled = TRUE;
                break;
            } else {
                if (frame != NULL) ++ph->count;
                utilResetTimer(&ph->timer);
            }
        }

    MUTEX_UNLOCK(&ph->mutex);

}

static THREAD_FUNC PacketThread(void *arg)
{
ISI_PUSH *ph;
MSGQ_MSG *msg;
ISI_PUSH_PACKET *pkt;
IACP_FRAME frame;
ISI_RAW_HEADER hdr;
static char *fid = "isiPush:PacketThread";

    ph = (ISI_PUSH *) arg;
    logioMsg(ph->log.lp, LOG_INFO, "%s:%d ISI push packet thread started", ph->server, ph->port);

    SEM_POST(&ph->sem);

    SendFrame(ph, NULL);

    while (1) {
        while ((msg = msgqGet(&ph->queue.full, MSGQ_NOWAIT)) != NULL) {
            pkt = (ISI_PUSH_PACKET *) msg->data;
            hdr.desc.type   = pkt->type;
            hdr.desc.comp   = ISI_COMP_NONE;
            hdr.desc.order  = ISI_ORDER_UNDEF;
            hdr.desc.size   = sizeof(UINT8);
            hdr.len.payload = hdr.len.used = hdr.len.native = pkt->len;
            frame.payload.type = ISI_IACP_RAW_PKT;
            frame.payload.data = ph->buf.data;
            frame.payload.len  = isiBuildPackRawPacket(frame.payload.data, &hdr, pkt->payload);
            SendFrame(ph, &frame);
            msgqPut(&ph->queue.heap, msg);
        }
        if (utilTimerExpired(&ph->timer)) SendFrame(ph, NULL);
        sleep(1);
    }
}

UINT64 isiPushCount(ISI_PUSH *ph)
{
UINT64 retval;

    if (ph == NULL) return 0;

    MUTEX_LOCK(&ph->mutex);
        retval = ph->count;
    MUTEX_UNLOCK(&ph->mutex);

    return retval;
}

void isiPushShutdown(ISI_PUSH *ph)
{
    if (ph == NULL) return;
    MUTEX_LOCK(&ph->mutex);
        ph->shutdown = TRUE;
    MUTEX_UNLOCK(&ph->mutex);
}

BOOL isiPushRawPacket(ISI_PUSH *ph, UINT8 *buf, UINT32 len, UINT8 type)
{
MSGQ_MSG *msg = NULL;
ISI_PUSH_PACKET *pkt;
static char *fid = "isiPushRawPacket";

    if (ph->buf.maxlen < len) {
        errno = EMSGSIZE;
        return FALSE;
    }

    /* grap an empty buffer from the heap or steal from the full queue if heap is empty */

    if (ph->block) {
        if ((msg = msgqGet(&ph->queue.heap, MSGQ_WAIT)) == NULL) return FALSE;
    } else {
        if ((msg = msgqGet(&ph->queue.heap, MSGQ_NOWAIT)) == NULL) {
            if ((msg = msgqGet(&ph->queue.full, MSGQ_NOWAIT)) == NULL) {
                errno = ENOSPC;
                return FALSE;
            }
        }
    }

    /* copy user supplied packet to the full queue */

    pkt = (ISI_PUSH_PACKET *) msg->data;
    pkt->type = type;
    pkt->len  = len;
    memcpy(pkt->payload, buf, len);

    msgqPut(&ph->queue.full, msg);

    return TRUE;
}

static BOOL InitBuf(ISI_PUSH *ph, int qdepth, int maxlen)
{
MSGQ_MSG *msg;
ISI_PUSH_PACKET *pkt;
static char *fid = "isiPush:InitBuf";

    if (!msgqInit(&ph->queue.full, qdepth, qdepth, sizeof(ISI_PUSH_PACKET))) return FALSE;
    if (!msgqInit(&ph->queue.heap,      0, qdepth, sizeof(ISI_PUSH_PACKET))) return FALSE;

    while ((msg = msgqGet(&ph->queue.full, MSGQ_NOWAIT)) != NULL) {
        pkt = (ISI_PUSH_PACKET *) msg->data;
        if ((pkt->payload = (UINT8 *) malloc(maxlen)) == NULL) return FALSE;
        msgqPut(&ph->queue.heap, msg);
    }

    return TRUE;
}

ISI_PUSH *isiPushInit(char *server, int port, IACP_ATTR *attr, LOGIO *lp, int verbosity, int maxlen, int qdepth, BOOL block)
{
ISI_PUSH *ph;
IACP_ATTR default_attr = IACP_DEFAULT_ATTR;

    if (server == NULL || port < 1) {
        errno = EINVAL;
        return NULL;
    }

    if ((ph = (ISI_PUSH *) malloc(sizeof(ISI_PUSH))) == NULL) return NULL;

    strncpy(ph->server, server, MAXPATHLEN);
    ph->port = port;
    ph->attr = (attr == NULL) ? default_attr : *attr;
    ph->buf.maxlen = maxlen;
    if ((ph->buf.data = (UINT8 *) malloc(ph->buf.maxlen + 2*sizeof(ISI_RAW_HEADER))) == NULL) {
        free(ph);
        return NULL;
    }
    ph->log.lp = lp;
    ph->log.level = verbosity;
    MUTEX_INIT(&ph->mutex);
    utilInitTimer(&ph->timer);
    utilStartTimer(&ph->timer, (ph->attr.at_timeo * NANOSEC_PER_MSEC) / 2);
    ph->shutdown = FALSE;
    ph->disabled = FALSE;
    ph->block    = block;
    SEM_INIT(&ph->sem, 0, 1);
    ph->count = 0;

    if (!InitBuf(ph, qdepth, maxlen)) {
        free(ph);
        return NULL;
    }

    if (!THREAD_CREATE(&ph->pkthread, PacketThread, (void *) ph)) {
        free(ph);
        return NULL;
    }

    SEM_WAIT(&ph->sem);

    return ph;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: push.c,v $
 * Revision 1.4  2014/01/27 18:43:39  dechavez
 * changed SendFrame() to void and removed uneeded return
 *
 * Revision 1.3  2013/10/23 19:50:45  dechavez
 * changed errno == ECONNRESET (comparison) to errno = ECONNRESET (assignment)
 *
 * Revision 1.2  2011/10/12 17:28:39  dechavez
 * added internal buffering into push client
 *
 * Revision 1.1  2011/08/04 22:04:22  dechavez
 * initial release
 *
 */
