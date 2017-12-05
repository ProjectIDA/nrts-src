#pragma ident "$Id: action.c,v 1.38 2017/04/13 15:21:13 dechavez Exp $"
/*======================================================================
 * 
 * FSA event handlers for all events except for handling of DT_DATA.
 * That is taken care of in reorder.c, and is where you will find
 * ActionINITDACK() and ActionDATA().
 *
 *====================================================================*/
#include "qdp.h"
#include "action.h"

/* Figure out if a packet is a reply to a FSA action or to user command */

#define FROM_AUTOMATON 0
#define FROM_USER      1
#define FROM_NOBODY    2

static int sender(QDP *qp, QDP_PKT *pkt)
{
QDP_PKT *usr;

    usr = (qp->cmd.msg != NULL) ? (QDP_PKT *) qp->cmd.msg->data : NULL;
    if (usr != NULL && usr->hdr.seqno == pkt->hdr.ack) return FROM_USER;
    if (qp->fsa.pkt.hdr.seqno == pkt->hdr.ack) return FROM_AUTOMATON;

    return FROM_NOBODY;
}

/* Send a deregistration command when aborting the handshake */

static void deregister(QDP *qp)
{
    qdpEncode_C1_DSRV(&qp->fsa.pkt, qp->par.serialno);
    ActionSEND(qp, NULL);
}

/* Start automaton registration timer */

void ActionSTART(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSTART";

    qp->meta.raw.token.nbyte = 0;
    if (qp->par.interval.watchdog > 0) {
        SEM_POST(&qp->fsa.semaphore.WatchdogThread);
    } else {
        qdpDebug(qp, "%s: registration watchdog timer not enabled", fid);
    }
}

/* Send an admin (library generated) packet */

void ActionSEND(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSEND";

    qdpSendPkt(qp, QDP_CTRL, &qp->fsa.pkt);
}

/* Poll for serial number */

void ActionPOLLSN(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionPOLLSN";

    qdpEncode_C1_POLLSN(&qp->fsa.pkt, 0, 0);
    ActionSEND(qp, event);
}

/* Save peer serial number and IP address */

void ActionLDSN(QDP *qp, QDP_EVENT *event)
{
QDP_TYPE_C1_MYSN c1_mysn;
char string[] = "xxx.xxx.xxx.xxx + slop";
static char *fid = "ActionLDSN";
#define DEFAULT_CONSOLE_IP 0x7F646464 /* 127.100.100.100 */

    qdpDecode_C1_MYSN(((QDP_PKT *) event->msg->data)->payload, &c1_mysn);
    qdpReturnPktMsgToHeap(fid, qp, event->msg);

    qp->par.serialno = c1_mysn.serialno;
    snprintf(qp->peer.ident, QDP_IDENT_LEN + 1, "%016llX", qp->par.serialno);
    qp->peer.addr = qp->ctrl.peer.ip ? qp->ctrl.peer.ip : DEFAULT_CONSOLE_IP;
    qdpDebug(qp, "%s: serial number %016llX received from IP %s", fid, qp->par.serialno, utilDotDecimalString(qp->peer.addr, string));
}

/* Issue server request */

void ActionSRVRQ(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSRVRQ";

    qdpEncode_C1_RQSRV(&qp->fsa.pkt, qp->par.serialno);
    qdpDebug(qp, "%s: serialno=0x%016llX\n", fid, qp->par.serialno);
    ActionSEND(qp, event);
    event->next_state = QDP_STATE_RQSRV;
}

/* Decode server challenge */

void ActionDECODECH(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionDECODECH";

    qdpDecode_C1_SRVCH(((QDP_PKT *) event->msg->data)->payload, &qp->fsa.srvch);
    qdpDebug(qp, "%s: challenge=0x%016llX IP=%s port=%d\n", fid, qp->fsa.srvch.challenge, qp->fsa.srvch.dp.dotdecimal, qp->fsa.srvch.dp.port);
    qdpReturnPktMsgToHeap(fid, qp, event->msg);
}

/* Build server challenge response packet */

void ActionBLDRSP(QDP *qp, QDP_EVENT *event)
{
QDP_TYPE_C1_SRVRSP c1_srvrsp;
static char *fid = "ActionBLDRSP";

    c1_srvrsp.serialno = qp->par.serialno;
    c1_srvrsp.challenge = qp->fsa.srvch.challenge;
    c1_srvrsp.dp = qp->fsa.srvch.dp;
    c1_srvrsp.random = utilTimeStamp();
    qdpMD5(&c1_srvrsp, qp->par.authcode);
    qdpEncode_C1_SRVRSP(&qp->fsa.pkt, &c1_srvrsp);
    qdpDebug(qp, "%s: serialno=0x%016llX challenge=0x%016llX IP=%s port=%d\n",
        fid, c1_srvrsp.serialno, c1_srvrsp.challenge, c1_srvrsp.dp.dotdecimal, c1_srvrsp.dp.port
    );
}

/* Decide what to do after Q330 registration is successful.
 * For comand and control sessions we are done, but for data
 * port connections we proceed to request the meta-data necessary
 * to deal with the data packets.
 */

void ActionREGISTERED(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionREGISTERED";

/* All done if this is a command and control session */

    if (!qp->dataLink) {
        ActionTLU(qp, event); /* This Layer Up */
        return;
    }

    ActionRQ_C1_LOG(qp, event);
    event->next_state = QDP_STATE_WAIT_C1_LOG;
}

/* Request C1_FIX */

void ActionRQ_C1_FIX(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionRQFIX";

    qdpInfo(qp, "%s C1_FIX requested", qp->peer.ident);
    qdpEncode_NoParCmd(&qp->fsa.pkt, QDP_C1_RQFIX);
    qp->fsa.pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Save C1_FIX */

void ActionLD_C1_FIX(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionLD_C1_FIX";

    qdpInfo(qp, "%s C1_FIX received", qp->peer.ident);
    pkt = (QDP_PKT *) event->msg->data;
    qdpDecode_C1_FIX(pkt->payload, &qp->c1_fix);
    if (qp->reboots != 0 && qp->reboots != qp->c1_fix.reboots) qdpWarn(qp, "NOTICE: %s reboot detected (S/N %s)", qp->par.connect.ident, qp->peer.ident);
    qp->reboots = qp->c1_fix.reboots;
    qdpReturnPktMsgToHeap(fid, qp, event->msg);

    if (qp->c1_fix.sys_ver.major >= QDP_MIN_EP_VERSION_MAJOR && qp->c1_fix.sys_ver.minor >= QDP_MIN_EP_VERSION_MINOR) {
        qp->flags |= QDP_FLAGS_EP_SUPPORTED;
        qdpInfo(qp, "%s peer supports QEP", qp->peer.ident);
    } else {
        qdpInfo(qp, "%s peer does NOT support QEP", qp->peer.ident);
    }
}

/* Request data port configuration */

void ActionRQ_C1_LOG(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionRQ_C1_LOG";

    qdpInfo(qp, "%s %s parameters (C1_LOG) requested", qp->peer.ident, qdpPortString(qp->par.port.link));

    qdpEncode_C1_RQLOG(&qp->fsa.pkt, (UINT16) qp->par.port.link);
    qp->fsa.pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Load (and verify) data port parameters */

void ActionLD_C1_LOG(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
QDP_TYPE_C1_LOG c1_log;
static char *fid = "ActionLD_C1_LOG";

    qdpInfo(qp, "%s configuration received, flags = 0x%08x", qp->peer.ident, qp->flags);
    pkt = (QDP_PKT *) event->msg->data;
    qdpDecode_C1_LOG(pkt->payload, &c1_log);
    qdpReturnPktMsgToHeap(fid, qp, event->msg);

    if (c1_log.perc == 0) {
        qdpInfo(qp, "%s %s is not enabled", qp->peer.ident, qdpPortString(qp->par.port.link));
        qp->errcode = QDP_ERR_DPORT_DISABLED;
        deregister(qp);
        ActionTLD(qp, event);
        return;
    }

    if (qp->flags & QDP_FLAGS_EP_SUPPORTED) {
        ActionRQ_C2_EPD(qp, event);
        event->next_state = QDP_STATE_WAIT_C2_EPD;
    } else {
        ActionRQ_C1_FLGS(qp, event);
        event->next_state = QDP_STATE_WAIT_C1_FLGS;
    }
}

/* Request environmental processor delays (if applicable) */

void ActionRQ_C2_EPD(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionRQ_C2_EPD";

    qdpInfo(qp, "%s environmental processor delays requested", qp->peer.ident);
    qdpEncode_NoParCmd(&qp->fsa.pkt, QDP_C2_RQEPD);
    ActionSEND(qp, event);
}

/* Save environmental processor delays */

void ActionLD_C2_EPD(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionLD_C2_EPD";

    pkt = (QDP_PKT *) event->msg->data;
    qdpDecode_C2_EPD(pkt->payload, &qp->meta.epd);
    qdpInfo(qp, "%s environmental processor delays received (%d channels)", qp->peer.ident, qp->meta.epd.chancnt);
    memcpy(qp->meta.raw.epd, pkt->payload, pkt->hdr.dlen);
    qdpForwardFullPktMsg(fid, qp, event->msg);
}

/* Request combo packet */

void ActionRQ_C1_FLGS(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionRQ_C1_FLGS";

    qdpInfo(qp, "%s %s combo packet (C1_FLGS) requested", qp->peer.ident, qdpPortString(qp->par.port.link));

    qdpEncode_C1_RQFLGS(&qp->fsa.pkt, (UINT16) qp->par.port.link);
    qp->fsa.pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Load combo packet and initialize reorder buffer */

#define MS100_TO_NANOSEC 
void ActionLD_C1_FLGS(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionLD_C1_FLGS";

    qdpInfo(qp, "%s %s combo packet received", qp->peer.ident, qdpPortString(qp->par.port.link));
    pkt = (QDP_PKT *) event->msg->data;
    memcpy(qp->meta.raw.combo, pkt->payload, pkt->hdr.dlen);
    qdpDecode_C1_COMBO(qp->meta.raw.combo, &qp->meta.combo);

    qp->recv.last_packet = qp->meta.combo.log.dataseq;
    qp->recv.ack_to = ((UINT64) qp->meta.combo.log.ack_to * 100) * NANOSEC_PER_MSEC;
    ActionINITDACK(qp);
    qdpDebug(qp, "%s ack_to set to %llu msec", qp->peer.ident, qp->recv.ack_to / NANOSEC_PER_MSEC);
    qdpDebug(qp, "%s last_packet set to %hu", qp->peer.ident, qp->recv.last_packet);
    qdpForwardFullPktMsg(fid, qp, event->msg);
}

/* Request DP tokens */

void ActionRQ_TOKENS(QDP *qp, QDP_EVENT *event)
{
int i;
QDP_TYPE_C1_RQMEM c1_rqmem;
static char *fid = "ActionRQ_TOKENS";

    if (qp->meta.raw.token.nbyte == 0) {
        qdpInfo(qp, "%s %s DP tokens requested", qp->peer.ident, qdpPortString(qp->par.port.link));
        c1_rqmem.offset = 0;
    } else {
        c1_rqmem.offset = qp->c1_mem.seg * QDP_MAX_C1_MEM_PAYLOAD;
    }
    c1_rqmem.nbyte = 0;
    c1_rqmem.type = qp->meta.raw.token.type;
    qdpDebug(qp, "%s: offset=%lu, nbyte=%hu, type=%s", fid, c1_rqmem.offset, c1_rqmem.nbyte, qdpMemTypeString(c1_rqmem.type));
    for (i = 0; i < 4; i++) c1_rqmem.passwd[i] = 0;
    qdpEncode_C1_RQMEM(&qp->fsa.pkt, &c1_rqmem);
    qp->fsa.pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Save DP tokens */

void ActionLD_TOKENS(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionLDMEM";

    if (qdpSaveMem(&qp->meta.raw.token, &qp->c1_mem)) qdpInfo(qp, "%s %s DP tokens received, nbyte=%hu", qp->peer.ident, qdpPortString(qp->par.port.link), qp->meta.raw.token.nbyte);
    qdpForwardFullPktMsg(fid, qp, event->msg);
}

/* This layer up (ie, newly registered) */

void ActionTLU(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionTLU";

    qp->suberr = QDP_CERR_NOERR;
    qp->errcode = QDP_ERR_NO_ERROR;
    qdpSetStatsTstamp(qp);
    qdpIncrStatsNreg(qp);
    if (qp->dataLink) {
        if (qp->par.meta.func != NULL) (qp->par.meta.func)(qp->par.meta.arg, &qp->meta);
        utilStartTimer(&qp->recv.timer, qp->recv.ack_to);
        utilStartTimer(&qp->open, (UINT64) qp->par.interval.open * NANOSEC_PER_MSEC);
    }
    event->next_state = QDP_STATE_READY;
    qdpInfo(qp, "%s handshake complete", qp->peer.ident);
    SEM_POST(&qp->fsa.semaphore.Automaton);
}

/* This layer down (shutdown automaton) */

void ActionTLD(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
MSGQ_MSG *msg;
static char *fid = "ActionTLD";

    qp->fsa.tld = TRUE;

    /* send a dummy command to tell AppcmdThread running over in fsa.c to suicide */

    if ((msg = msgqGet(&qp->Q.cmd.heap, MSGQ_NOWAIT)) != NULL) { 
        qdpDebug(qp, "%s: send QDP_CX_XXXX 'poison pill' to AppcmdThread", fid);
        pkt = (QDP_PKT *) msg->data;
        qdpInitPkt(pkt, QDP_CX_XXXX, 0, 0);
        msgqPut(&qp->Q.cmd.full, msg);
    } else {
        qdpDebug(qp, "%s: can't grab buffer from Q.cmd.heap", fid);
    }

    /* tell the WatchdogThread to die as well */

    qdpDebug(qp, "%s: SEM_POST(WatchdogThread)", fid);
    SEM_POST(&qp->fsa.semaphore.WatchdogThread);

    /* tell the automaton to turn off */

    qdpResetSessionStats(qp);
    qp->meta.raw.token.nbyte = 0;
    event->next_state = QDP_STATE_OFF;
}

/* Note registration error */

void ActionREGERR(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionREGERR";

    pkt = (QDP_PKT *) event->msg->data;
    qdpReturnPktMsgToHeap(fid, qp, event->msg);

    qp->suberr = qdpErrorCode(pkt);
    qdpDebug(qp, "%s registration error %d (%s)", qp->peer.ident, qp->suberr, qdpErrString(qp->suberr));

    /* Check for the explicitly supported "too many servers" error */

    switch (qp->suberr = qdpErrorCode(pkt)) {
      case QDP_CERR_TMSERV: qp->errcode = QDP_ERR_BUSY; break;
      default: qp->errcode = QDP_ERR_REGERR; break;
    }

    ActionTLD(qp, event);
}

/* Process a C1_CERR on a registered link */

void ActionCERR(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt, *usr;
static char *fid = "ActionCERR";

    pkt = (QDP_PKT *) event->msg->data;
    qp->cmd.err = qdpErrorCode(pkt);

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpError(qp, "%s: %s automaton command error %d (%s)", fid, qp->peer.ident, qp->cmd.err, qdpErrString(qp->cmd.err));
        qdpError(qp, "restarting handshake\n");
        ActionSTART(qp, event);
        ActionSRVRQ(qp, event);
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qp->cmd.ok = FALSE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpReturnPktMsgToHeap(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        qdpWarn(qp, "unexpected qp->cmd.err %d (%s) from %s", qp->cmd.err, qdpErrString(qp->cmd.err), qp->peer.ident);
        ActionDROP(qp, event);
    }
}

/* Process a C1_CACK on a registered link */

void ActionCACK(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt, *usr;
static char *fid = "ActionCACK";

    pkt = (QDP_PKT *) event->msg->data;

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpDebug(qp, "%s heartbeat ACK received", qp->peer.ident);
        qdpReturnPktMsgToHeap(fid, qp, event->msg);
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s -> %s ACK received", qdpCmdString(usr->hdr.cmd), qp->peer.ident);
        qp->cmd.ok = TRUE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpReturnPktMsgToHeap(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        ActionDROP(qp, event);
    }
}

/* Process a non-data packet on a registered link */

void ActionCTRL(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt, *usr;
static char *fid = "ActionCTRL";

    pkt = (QDP_PKT *) event->msg->data;

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpDebug(qp, "%s heartbeat %s received", qp->peer.ident, qdpCmdString(pkt->hdr.cmd));
        if (qp->par.hbeat.forward) {
            qdpForwardFullPktMsg(fid, qp, event->msg);
        } else {
            qdpReturnPktMsgToHeap(fid, qp, event->msg);
        }
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s -> %s returns %s", qdpCmdString(usr->hdr.cmd), qp->peer.ident, qdpCmdString(pkt->hdr.cmd));
        qp->cmd.ok = TRUE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpForwardFullPktMsg(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        ActionDROP(qp, event);
    }
}

/* Open a data port */

void ActionOPEN(QDP *qp, QDP_EVENT *event)
{
QDP_PKT pkt;
static char *fid = "ActionOPEN";

    if (!qp->dataLink) return;

    qdpInitPkt(&pkt, QDP_DT_OPEN, 0, 0);
    qdpSendPkt(qp, QDP_DATA, &pkt);
    utilResetTimer(&qp->open);
}

/* Send DT_OPEN when no data received after specified interval */

void ActionDTO(QDP *qp, QDP_EVENT *event)
{
QDP_PKT pkt;
static char *fid = "ActionDTO";

    if (!qp->dataLink) return;

    if (utilTimerExpired(&qp->open)) ActionOPEN(qp, event);
}

/* Send a heartbeat (status request) */

void ActionHBEAT(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionHBEAT";

    qdpEncode_C1_RQSTAT(&qp->fsa.pkt, qp->par.hbeat.bitmap);
    qp->fsa.pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Send any pending user command to the digitizer */

void ActionAPPCMD(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionAPPCMD";

/* Nothing to do if there is no command pending */

    if (qp->cmd.msg == NULL) return;

    pkt = (QDP_PKT *) qp->cmd.msg->data;

/* Restart the machine if we fail to get a response after "repeated" attempts */

    if (qp->cmd.attempts > QDP_MAX_CMD_ATTEMPTS) {
        qdpInfo(qp, "WARNING: no response to user issued '%s' command\n", qdpCmdString(pkt->hdr.cmd));
        qp->cmd.ok = FALSE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        SEM_POST(&qp->cmd.sem);
        qdpInfo(qp, "restarting handshake\n");
        ActionSTART(qp, event);
        ActionSRVRQ(qp, event);
        return;
    }

/* Send the command */

    if (pkt->hdr.seqno == 0) pkt->hdr.seqno = ++qp->seqno;
    qdpSendPkt(qp, QDP_CTRL, pkt);
    ++qp->cmd.attempts;
}

/* Send DT_DACK if needed */

static BOOL AckRequired(QDP *qp)
{
static char *fid = "AckRequired";

/* Do send an ack if the ACK timeout interval has passed */

    if (utilTimerExpired(&qp->recv.timer)) {
        qdpDebug(qp, "%s ack timer expired with count=%hu, forcing DT_DACK", qp->peer.ident, qp->recv.count);
        return TRUE;
    }

/* Do send an ack if we've exceeded the acknowledge count */

    if (qp->recv.count >= qp->meta.combo.log.ack_cnt) {
        qdpDebug(qp, "%s recv count=%hu, ack_cnt=%hu, DT_DACK required", qp->peer.ident, qp->recv.count, qp->meta.combo.log.ack_cnt);
        return TRUE;
    }

/* Otherwise, we wait */

    qdpDebug(qp, "%s recv count=%hu, ack_cnt=%hu, no DT_DACK needed", qp->peer.ident, qp->recv.count, qp->meta.combo.log.ack_cnt);
    return FALSE;
}

void ActionDACK(QDP *qp, QDP_EVENT *event)
{
int i;
QDP_PKT pkt;
UINT8 *ptr;
static char *fid = "ActionDACK";

    if (!qp->dataLink) return;

    if (!AckRequired(qp)) return;

    qdpInitPkt(&pkt, QDP_DT_DACK, 0, qp->recv.ack_seqno);
    ptr = pkt.payload;
    ptr += utilPackUINT16(ptr, 0); /* DYNAMIC THROTTLE NOT CURRENTLY SUPPORTED */
    ptr += sizeof(UINT16); /* skip over spare */
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, qp->recv.ack[i]);
    ptr += sizeof(UINT32); /* skip over spare */
    pkt.hdr.dlen = (int) (ptr - pkt.payload);
    qdpSendPkt(qp, QDP_DATA, &pkt);
    ActionINITDACK(qp);
    qp->recv.count = 0;
    utilResetTimer(&qp->recv.timer);
    
}

/* DT_FILL packets get tossed */

void ActionFILL(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionFILL";

    qdpDebug(qp, "%s", fid);
    qdpIncrStatsFill(qp);
    qdpReturnPktMsgToHeap(fid, qp, event->msg);
}

/* Drop an unexpected packet */

void ActionDROP(QDP *qp, QDP_EVENT *event)
{
UINT8 cmd;
static char *fid = "ActionDROP";

    cmd = ((QDP_PKT *) event->msg->data)->hdr.cmd;
    qdpInfo(qp, "%s: spurious %s dropped", qp->peer.ident, qdpCmdString(cmd));
    qdpReturnPktMsgToHeap(fid, qp, event->msg);
    qdpIncrStatsDrop(qp);
}

/* Flush the reorder buffer */

void ActionFLUSH(QDP *qp, QDP_EVENT *event)
{
int i, count;
static char *fid = "ActionFLUSH";

    if (!qp->dataLink) return;

    for (i = 0; i < 4; i++) qp->recv.ack[0] = 0;

    for (count = 0, i = 0; i < QDP_MAX_WINDOW_NBUF; i++) {
        if (qp->recv.msg[i] != NULL) {
            qdpReturnPktMsgToHeap(fid, qp, event->msg);
            qp->recv.msg[i] = NULL;
            ++count;
        }
    }
    qp->recv.count = 0;
    qdpDebug(qp, "tossed %d packets from %s reorder buffer", count, qp->peer.ident);
}

/* Note and CRC errors */

void ActionCRCERR(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionCRCERR";

    qdpInfo(qp, "%s CRC error, packet dropped", qp->peer.ident);
    qdpIncrStatsCrc(qp);
}

/* Note I/O error */

void ActionIOERR(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionIOERR";

    qdpInfo(qp, "I/O error reading from %s: %s", qp->peer.ident, strerror(errno));

    ActionTLD(qp, event);
}

/* Note a registration watchdog timeout */

void ActionWATCHDOG(QDP *qp, QDP_EVENT *event)
{
float interval;
QDP_PKT *pkt;
static char *fid = "ActionWATCHDOG";

    interval = (float) qp->par.interval.watchdog / MSEC_PER_SEC;
    qdpInfo(qp, "%s registration watchdog timer expired (%.3f seconds)", qp->peer.ident, interval);
    qp->errcode = QDP_ERR_WATCHDOG;
    qp->suberr = qdpFsaState(qp);

    ActionTLD(qp, event);
}

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
 * $Log: action.c,v $
 * Revision 1.38  2017/04/13 15:21:13  dechavez
 * added deregister when aborting handshake with a disabled data port
 *
 * Revision 1.37  2017/02/01 17:55:12  dechavez
 * fixed bug in ActionLD_C1_LOG() (inroduced in 3.13.0) that was causing QEP filter delays to get lost
 *
 * Revision 1.36  2016/09/07 18:21:15  dechavez
 * Check for QEP support in ActionLD_C1_FIX() instead of ActionLD_C1_LOG()
 * (other wise non-data connections could not query the device).
 * Restart automaton (instead of failing) when an automaton command (such as
 * the automaton "heartbeat" status requests) results in a C1_CERR response
 *
 * Revision 1.35  2016/08/19 17:21:09  dechavez
 * ActionAPPCMD() passes on QDP_EVENT rather than NULL (duh) when invoking
 * ActionSTART() and ActionSRVRQ() in response to failed user commands
 *
 * Revision 1.34  2016/08/19 16:10:17  dechavez
 * ActionTLD() no longer sends C1_DSRV deregistration (that's now done in qdpShutdown())
 * ActionAPPCMD() lots number of times command has been sent and restarts automaton
 * when that exceeds QDP_MAX_CMD_ATTEMPTS (currently 2, set in qdp.h)
 *
 * Revision 1.33  2016/08/15 19:32:48  dechavez
 * added a few more debug messages
 *
 * Revision 1.32  2016/08/04 21:22:32  dechavez
 * fixed problem with comments
 *
 * Revision 1.31  2016/08/04 21:21:47  dechavez
 * various changes related to changes in fsa.h, removed some uneeded debug code, added reboot detector
 *
 * Revision 1.30  2016/07/20 16:37:59  dechavez
 * updated action handlers to match the rework of the state machine
 *
 * Revision 1.29  2016/06/16 15:35:24  dechavez
 * introduced ActionGIVEUP()
 *
 * Revision 1.28  2016/02/12 18:00:27  dechavez
 * changed actionLDFIX() to store C1_FIX in QDP handle's new c1_fix field
 *
 * Revision 1.27  2016/02/11 19:03:11  dechavez
 * changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry, added ActionDECODECH(),
 * added code to ActionBLDRSP to track attempts and fail when excessive
 *
 * Revision 1.26  2016/02/05 16:45:59  dechavez
 * ActionSRVRQ() tracks nsrq and brings down the automaton when it exceeds maxsrq
 *
 * Revision 1.25  2016/02/03 18:49:46  dechavez
 * changed ActionDecide to ActionDECIDE, just to keep with the style
 *
 * Revision 1.24  2016/02/03 17:32:02  dechavez
 * changed ActionRQCNF() to ActionRQCMB(), ActionLDCNF() to ActionLDCMB(),
 * added ActionRQFIX(), ActionLDFIX(), and ActionDecide()
 *
 * Revision 1.23  2016/01/28 00:43:59  dechavez
 * fixed up handling of C2_EPD packet in ActionLDEPD(), forward it onto the
 * application for saving in the QDP disk loop
 *
 * Revision 1.22  2016/01/27 00:18:33  dechavez
 * added ActionRQEPD() and ActionLDEPD()
 *
 * Revision 1.21  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.20  2014/08/11 18:07:39  dechavez
 * MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.19  2011/01/31 21:14:13  dechavez
 * added ActionRQPHY() and ActionLDPHY()
 *
 * Revision 1.18  2011/01/14 00:27:10  dechavez
 * update peer ident with serial number in ActionSAVESN()
 *
 * Revision 1.17  2011/01/13 20:31:58  dechavez
 * added ActionPOLLSN() and ActionSAVESN()
 *
 * Revision 1.16  2010/12/23 21:51:05  dechavez
 * fixed HORRIBLE off by one bug in ActionRQCNF call to qdpEncode_C1_RQFLGS() port parameter,
 *
 * Revision 1.15  2010/03/31 20:40:08  dechavez
 * added qdpShutdown()
 *
 * Revision 1.14  2010/03/22 21:33:22  dechavez
 * added ActionBUSY
 *
 * Revision 1.13  2009/11/13 00:35:33  dechavez
 * use C1_CERR err field in handle to communicate errors back to application (eg, memory busy)
 *
 * Revision 1.12  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
