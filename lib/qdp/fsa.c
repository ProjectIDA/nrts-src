#pragma ident "$Id: fsa.c,v 1.34 2017/02/03 20:46:58 dechavez Exp $"
/*======================================================================
 * 
 * QDP Finite State Automaton
 *
 *====================================================================*/
#define INCLUDE_QDP_STATE_MACHINE
#include "qdp.h"
#include "qdp/fsa.h"
#include "action.h"

#define POLL_INTERVAL 500 /* sleep in half second increments */

static void IncrementThreadCount(QDP *qp)
{
    MUTEX_LOCK(&qp->fsa.mutex);
        ++qp->fsa.nthread;
    MUTEX_UNLOCK(&qp->fsa.mutex);
}

static int DecrementThreadCount(QDP *qp)
{
int nthread;

    MUTEX_LOCK(&qp->fsa.mutex);
        nthread = --qp->fsa.nthread;
    MUTEX_UNLOCK(&qp->fsa.mutex);

    return nthread;
}

static void TerminateSelf(char *caller, QDP *qp)
{
int nthread;
static char *fid = "TerminateSelf";

    nthread = DecrementThreadCount(qp);
    qdpInfo(qp, "%s: %s exits, %d threads pending", qp->peer.ident, caller, nthread);

    if (nthread == 0) {
        qdpDebug(qp, "%s: qioClose(&qp->ctrl)", fid);
        qioClose(&qp->ctrl);
        if (qp->peer.port.data != 0) {
            qdpDebug(qp, "%s: qioClose(&qp->data)", fid);
            qioClose(&qp->data);
        }
        qdpInfo(qp, "%s connection closed", qp->peer.ident);
        SEM_POST(&qp->fsa.semaphore.Automaton);
    }

    THREAD_EXIT((void *) 0);
}

static int TokenEvent(QDP *qp, MSGQ_MSG *msg)
{
QDP_PKT *pkt;

    pkt = (QDP_PKT *) msg->data;
    qdpDecode_C1_MEM(pkt->payload, &qp->c1_mem);
    if (qp->c1_mem.type != qp->meta.raw.token.type) return QDP_EVENT_CTRL;
    return (qp->c1_mem.seg == qp->c1_mem.nseg) ? QDP_EVENT_TOKEN_DONE : QDP_EVENT_TOKEN_SOME;
}

static int CtrlEvent(QDP *qp, MSGQ_MSG *msg)
{
int errcode;
QDP_PKT *pkt;
static char *fid = "CtrlThread:CtrlEvent";

    pkt = (QDP_PKT *) msg->data;

    switch (pkt->hdr.cmd) {
      case QDP_C1_MYSN:  return QDP_EVENT_MYSN;
      case QDP_C1_SRVCH: return QDP_EVENT_SRVCH;
      case QDP_C1_CERR:  return qdpErrorCode(pkt) == QDP_CERR_NOTR ? QDP_EVENT_NOTREG : QDP_EVENT_CERR;
      case QDP_C1_CACK:  return QDP_EVENT_CACK;
      case QDP_C1_LOG:   return QDP_EVENT_C1_LOG;
      case QDP_C1_FIX:   return QDP_EVENT_C1_FIX;
      case QDP_C2_EPD:   return QDP_EVENT_C2_EPD;
      case QDP_C1_FLGS:  return QDP_EVENT_C1_FLGS;
      case QDP_C1_MEM:   return TokenEvent(qp, msg);
    }

    return QDP_EVENT_CTRL;
}

/* Generate event based on what was received on the data port */

static int DataEvent(QDP *qp, MSGQ_MSG *msg)
{
    switch (((QDP_PKT *) msg->data)->hdr.cmd) {

      case QDP_DT_DATA:
        return QDP_EVENT_DATA;

      case QDP_DT_FILL:
        return QDP_EVENT_FILL;

      default:
        return QDP_EVENT_UKNDATA;
    }
}

/* Return the current state */

int qdpFsaState(QDP *qp)
{
int retval;

    if (qp == NULL) {
        errno = EINVAL;
        return -1;
    }

    MUTEX_LOCK(&qp->fsa.mutex);
        retval = qp->fsa.state.current;
    MUTEX_UNLOCK(&qp->fsa.mutex);

    return retval;
}

/* Logged state transitions */

static int SetState(QDP *qp, int state)
{
    MUTEX_LOCK(&qp->fsa.mutex);
        qp->fsa.state.previous = qp->fsa.state.current;
        qp->fsa.state.current = state;
    MUTEX_UNLOCK(&qp->fsa.mutex);

    return state;
}

static void ChangeState(char *fid, QDP *qp, int state)
{
    if (state < QDP_MIN_STATE || state > QDP_MAX_STATE) {
        qdpError(qp, "%s:ChangeState: FATAL ERROR: invalid state code '%d' encountered", fid, state);
        state = QDP_STATE_OFF;
    }
    SetState(qp, state);
    qdpDebug(qp, "%s: transition to state '%s'", fid, qdpStateString(state));;
}

/* Process one event */

static void EventHandler(QDP *qp, QDP_EVENT *event)
{
int i, state, action;
static char *fid = "EventHandler";

    state = qdpFsaState(qp);
    qdpDebug(qp, "%s (%s): %s (current state: %s)", fid, qdpTerseEventString(event->code), qdpEventString(event->code), qdpStateString(state));

    event->next_state = QDP_STATE_MACHINE[state].event[event->code].next_state; /* might get changed by action handler */

    for (i = 0; i < MAX_ACTION_PER_EVENT; i++) {

    /* grap the action code from the fsa.h automaton state transition table */

        action = QDP_STATE_MACHINE[state].event[event->code].action[i];

    /* encountering a -1 in the transition table means it wasn't thought through well enough or there is a typo */

        if (action < QDP_MIN_ACTION || action > QDP_MAX_ACTION) {
            qdpError(qp, "%s: FATAL ERROR: invalid action code '%d' encountered", fid, action);
            qdpError(qp, "%s:             state = %2d = %s (%s)\n", fid, state, qdpTerseStateString(state), qdpStateString(state));
            qdpError(qp, "%s:       event->code = %2d = %s (%s)\n", fid, event->code, qdpTerseEventString(event->code), qdpEventString(event->code));
            qdpError(qp, "%s: event->next_state = %2d = %s (%s)\n", fid, event->next_state, qdpTerseStateString(event->next_state), qdpStateString(event->next_state));
            ChangeState(fid, qp, QDP_STATE_OFF);
            return;
        }

    /* an action of QDP_ACTION_done means we are done with this event */

        if (action != QDP_ACTION_done) {
            qdpDebug(qp, "%s: %s", fid, qdpActionString(action));
        } else {
            break;
        }

        switch (action) {

          case QDP_ACTION_START:      ActionSTART(qp, event);      break;
          case QDP_ACTION_SEND:       ActionSEND(qp, event);       break;
          case QDP_ACTION_POLLSN:     ActionPOLLSN(qp, event);     break;
          case QDP_ACTION_LDSN:       ActionLDSN(qp, event);       break;
          case QDP_ACTION_SRVRQ:      ActionSRVRQ(qp, event);      break;
          case QDP_ACTION_DECODECH:   ActionDECODECH(qp, event);   break;
          case QDP_ACTION_BLDRSP:     ActionBLDRSP(qp, event);     break;
          case QDP_ACTION_REGISTERED: ActionREGISTERED(qp, event); break;
          case QDP_ACTION_RQ_C1_FIX:  ActionRQ_C1_FIX(qp, event);  break;
          case QDP_ACTION_LD_C1_FIX:  ActionLD_C1_FIX(qp, event);  break;
          case QDP_ACTION_RQ_C1_LOG:  ActionRQ_C1_LOG(qp, event);  break;
          case QDP_ACTION_LD_C1_LOG:  ActionLD_C1_LOG(qp, event);  break;
          case QDP_ACTION_RQ_C2_EPD:  ActionRQ_C2_EPD(qp, event);  break;
          case QDP_ACTION_LD_C2_EPD:  ActionLD_C2_EPD(qp, event);  break;
          case QDP_ACTION_RQ_C1_FLGS: ActionRQ_C1_FLGS(qp, event); break;
          case QDP_ACTION_LD_C1_FLGS: ActionLD_C1_FLGS(qp, event); break;
          case QDP_ACTION_RQ_TOKENS:  ActionRQ_TOKENS(qp, event);  break;
          case QDP_ACTION_LD_TOKENS:  ActionLD_TOKENS(qp, event);  break;
          case QDP_ACTION_TLU:        ActionTLU(qp, event);        break;
          case QDP_ACTION_TLD:        ActionTLD(qp, event);        break;
          case QDP_ACTION_REGERR:     ActionREGERR(qp, event);     break;
          case QDP_ACTION_CERR:       ActionCERR(qp, event);       break;
          case QDP_ACTION_CACK:       ActionCACK(qp, event);       break;
          case QDP_ACTION_CTRL:       ActionCTRL(qp, event);       break;
          case QDP_ACTION_OPEN:       ActionOPEN(qp, event);       break;
          case QDP_ACTION_DTO:        ActionDTO(qp, event);        break;
          case QDP_ACTION_HBEAT:      ActionHBEAT(qp, event);      break;
          case QDP_ACTION_APPCMD:     ActionAPPCMD(qp, event);     break;
          case QDP_ACTION_DACK:       ActionDACK(qp, event);       break;
          case QDP_ACTION_DATA:       ActionDATA(qp, event);       break;
          case QDP_ACTION_FILL:       ActionFILL(qp, event);       break;
          case QDP_ACTION_DROP:       ActionDROP(qp, event);       break;
          case QDP_ACTION_FLUSH:      ActionFLUSH(qp, event);      break;
          case QDP_ACTION_CRCERR:     ActionCRCERR(qp, event);     break;
          case QDP_ACTION_IOERR:      ActionIOERR(qp, event);      break;
          case QDP_ACTION_WATCHDOG:   ActionWATCHDOG(qp, event);   break;

          default:
            qdpError(qp, "%s: FATAL ERROR: '%s' action not implemented", fid, qdpActionString(action));
            ChangeState(fid, qp, QDP_STATE_OFF);
            return;
        }

    /* Stop processing if an error was encountered */

        if (qp->errcode != QDP_ERR_NO_ERROR) break;
    }

/* Transition to the next state */

    if (event->next_state != QDP_STATE_nochange) {
        ChangeState(fid, qp, event->next_state);
    } else {
        qdpDebug(qp, "%s: remain in state: %s", fid, qdpStateString(qdpFsaState(qp)));
    }
}

/* Post an event */

static BOOL PostEvent(QDP *qp, int code, MSGQ_MSG *pktmsg)
{
int type;
char *string;
MSGQ_MSG *msg;
QDP_EVENT *event;
static char *fid = "PostEvent";

/* This will block while another event is currently being processed */

    msg = msgqGet(&qp->Q.event.heap, MSGQ_WAIT);

/* Can't post events if the machine is off */

    if (qdpFsaState(qp) == QDP_STATE_OFF) {
        msgqPut(&qp->Q.event.heap, msg);
        return FALSE;
    }

/* Load in our event parameters and post */

    event = (QDP_EVENT *) msg->data;
    event->code = code;
    event->msg = pktmsg;

    msgqPut(&qp->Q.event.full, msg);

    return TRUE;
}

/* Dispatch events */

static THREAD_FUNC EventThread(void *argptr)
{
QDP *qp;
MSGQ_MSG *msg;
QDP_EVENT *event;
static char *fid = "EventThread";

    qp = (QDP *) argptr;

    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);   /* let automaton proceed with initialization */

    while (1) {
        msg = msgqGet(&qp->Q.event.full, MSGQ_WAIT);
        event = (QDP_EVENT *) msg->data;
        if (event->code < QDP_MIN_EVENT || event->code > QDP_MAX_EVENT) {
            qdpError(qp, "%s: FATAL ERROR: illegal event code '%d' received", fid);
            ChangeState(fid, qp, QDP_STATE_OFF);
        } else {
            EventHandler(qp, event);
        }
        msgqPut(&qp->Q.event.heap, msg);
        if (qdpFsaState(qp) == QDP_STATE_OFF) TerminateSelf(fid, qp);
    }
}

/* Receive packets on the control port. */

static THREAD_FUNC CtrlThread(void *argptr)
{
QDP *qp;
int status;
MSGQ_MSG *msg;
static char *fid = "CtrlThread";

    qp = (QDP *) argptr;

    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);   /* let automaton proceed with initialization */

/* Generate events from whatever comes (or doesn't come) over the wire */

    while (1) {
        msg = qdpRecvPkt(qp, &qp->ctrl, &status);
        if (qp->fsa.tld || qdpFsaState(qp) == QDP_STATE_OFF) TerminateSelf(fid, qp);
        switch (status) {
          case QDP_OK:
            if (!PostEvent(qp, CtrlEvent(qp, msg), msg)) TerminateSelf(fid, qp);
            break;
          case QDP_TIMEOUT:
            if (!PostEvent(qp, QDP_EVENT_CTO, NULL)) TerminateSelf(fid, qp);
            break;
          case QDP_CRCERR:
            if (!PostEvent(qp, QDP_EVENT_CRCERR, NULL)) TerminateSelf(fid, qp);
            break;
          default:
            if (!PostEvent(qp, QDP_EVENT_IOERR, NULL)) TerminateSelf(fid, qp);
            break;
        }
    }
}

/* Receive packets on the data port */

static THREAD_FUNC DataThread(void *argptr)
{
QDP *qp;
int status;
MSGQ_MSG *msg;
static char *fid = "DataThread";

    qp = (QDP *) argptr;

    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);   /* let automaton proceed with initialization */

    while (1) {
        msg = qdpRecvPkt(qp, &qp->data, &status);
        if (qp->fsa.tld || qdpFsaState(qp) == QDP_STATE_OFF) TerminateSelf(fid, qp);
        switch (status) {
          case QDP_OK:
            if (!PostEvent(qp, DataEvent(qp, msg), msg)) TerminateSelf(fid, qp);
            break;
          case QDP_TIMEOUT:
            if (!PostEvent(qp, QDP_EVENT_DTO, NULL)) TerminateSelf(fid, qp);
            break;
          case QDP_CRCERR:
            if (!PostEvent(qp, QDP_EVENT_CRCERR, NULL)) TerminateSelf(fid, qp);
            break;
          default:
            if (!PostEvent(qp, QDP_EVENT_IOERR, NULL)) TerminateSelf(fid, qp);
            break;
        }
    }
}

/* Accept and process commands from the application */

static THREAD_FUNC AppcmdThread(void *argptr)
{
QDP *qp;
int status;
QDP_PKT *pkt;
BOOL shutdown = FALSE;
static char *fid = "AppcmdThread";

    qp = (QDP *) argptr;

    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);    /* let automaton proceed with initialization */

/* Command queue has a depth of one, so at most one command will be pending at any given time */

    while (1) {

        /* block until a command is received (can be from the app or automaton) */

        qp->cmd.msg = msgqGet(&qp->Q.cmd.full, MSGQ_WAIT);
        qp->cmd.attempts = 0;

        /* commit suicide if we were woken up by ActionTLD() */

        if (qp->fsa.tld) TerminateSelf(fid, qp);

        /* otherwise, process the command */

        pkt = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s: %s received from app", fid, qdpCmdString(pkt->hdr.cmd));
        pkt->hdr.seqno = 0;
        if (!PostEvent(qp, QDP_EVENT_APPCMD, NULL)) TerminateSelf(fid, qp);
    }
}

/* Generate heartbeat events at a regular interval */

static THREAD_FUNC HeartbeatThread(void *argptr)
{
QDP *qp;
UTIL_TIMER timer;
static char *fid = "HeartbeatThread";

    qp = (QDP *) argptr;

    utilInitTimer(&timer);
    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);       /* let automaton proceed with initialization */

    while (1) {
        utilStartTimer(&timer, qp->par.hbeat.interval * NANOSEC_PER_MSEC);
        while (!utilTimerExpired(&timer)) {
            utilDelayMsec(POLL_INTERVAL);
            if (qp->fsa.tld || qdpFsaState(qp) == QDP_STATE_OFF) TerminateSelf(fid, qp);
        }
        if (!PostEvent(qp, QDP_EVENT_HBEAT, NULL)) TerminateSelf(fid, qp);
    }
}

/* Watchdog thread will bring things down if registration doesn't complete in time */

static THREAD_FUNC WatchdogThread(void *argptr)
{
QDP *qp;
int state;
UTIL_TIMER timer;
static char *fid = "WatchdogThread";

    qp = (QDP *) argptr;

    utilInitTimer(&timer);
    IncrementThreadCount(qp);
    qdpDebug(qp, "%s ready", fid);
    SEM_POST(&qp->fsa.semaphore.Automaton);      /* let automaton proceed with initialization */

    while (1) {
        qdpDebug(qp, "%s: waiting for launch command", fid);
        SEM_WAIT(&qp->fsa.semaphore.WatchdogThread); /* wait until automaton starts me up */
        if (qp->fsa.tld) TerminateSelf(fid, qp);     /* suicide if we got woken up by ActionTLD() */

        qdpDebug(qp, "%s: sleep for %d msec", fid, qp->par.interval.watchdog);
        utilStartTimer(&timer, qp->par.interval.watchdog * NANOSEC_PER_MSEC);
        while (!utilTimerExpired(&timer)) {
            utilDelayMsec(POLL_INTERVAL);
            if (qp->fsa.tld) TerminateSelf(fid, qp); /* suicide if shit happpened while asleep */
        }

        state = qdpFsaState(qp);
        qdpDebug(qp, "%s: awaken in state = %s", fid, qdpStateString(state));
        if (qp->fsa.tld) TerminateSelf(fid, qp);     /* ditto */

        switch (state) {
          case QDP_STATE_OFF: TerminateSelf(fid, qp); break;
          case QDP_STATE_READY: break;
          default:
            qdpDebug(qp, "%s: post watchdog event", fid);
            PostEvent(qp, QDP_EVENT_WATCHDOG, NULL);
        }
    }
}

/* Try to figure out the real reason the handshake failed.
 *
 * A watchdog timeout while in state QDP_STATE_SRVRSP means that we were able to 
 * send the server request and receive the server challenge, so I/O must be OK.
 * We will interpret this condtion as indicative of a bad auth code.  It also
 * happens that the Q330 will not ignore a bad auth code but instead respond
 * with a C1_CERR packet with code QDP_CERR_INVREG (yep, seen both behaviors
 * occur on the same digitizer).  In either case we replace the more generic
 * error code (QDP_ERR_WATCHDOG or QDP_ERR_REGERR) with the more explicit
 * QDP_ERR_BADAUTH, for spare the user having to think about what to fix.
 *
 * The other case we check for is the case where we never get any replies
 * from the Q330.  That could be simply bad comm, bad IP address, bad
 * console cable, Q330 in the midst of a reboot, Q330 absent, who knows.
 * But, the main thing is to flag that *nothing* was ever received from
 * the Q330 and let the user suss that one out.
 */

static void SetErrcode(QDP *qp)
{
static char *fid = "SetErrcode";

    qdpDebug(qp, "%s: errcode = %d (%s), suberr = %d", fid, qp->errcode, qdpErrcodeString(qp->errcode), qp->suberr);

    switch (qp->fsa.state.previous) {
      case QDP_STATE_SRVRSP:
        /* If we are in QDP_STATE_SRVRSP, that means that we've sent a server request and
         * received a server challenge, so I/O must be OK.  A bad auth code can cause
         * the Q330 to go silent or respond with a QDP_CERR_INVREG (I've seen both).
         * We check for both these and if either is found we'll explicitly claim that
         * the auth code was bad.
         */
        if (qp->errcode == QDP_ERR_WATCHDOG || (qp->errcode == QDP_ERR_REGERR && qp->suberr == QDP_CERR_INVREG)) {
            qdpDebug(qp, "%s: set errcode = %d (%s)", fid, QDP_ERR_BADAUTH, qdpErrcodeString(QDP_ERR_BADAUTH));
            qp->errcode = QDP_ERR_BADAUTH;
        }
        break;
      case QDP_STATE_WAITSN:
      case QDP_STATE_RQSRV:
        if (qp->errcode == QDP_ERR_WATCHDOG) {
            qdpDebug(qp, "%s: set errcode = %d (%s)", fid, QDP_ERR_NORESPONSE, qdpErrcodeString(QDP_ERR_NORESPONSE));
            qp->errcode = QDP_ERR_NORESPONSE;
        }
    }
}

BOOL qdpStartFSA(QDP *qp)
{
int state;
THREAD tid;
static char *fid = "qdpStartFSA";

    if (qp == NULL) {
        qdpError(qp, "%s: NULL input not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    qp->fsa.nthread = 0;

/* Start the event dispatcher */

    if (!THREAD_CREATE(&tid, EventThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: EventThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    qdpDebug(qp, "%s: SEM_WAIT() for EventThread launch\n", fid);
    SEM_WAIT(&qp->fsa.semaphore.Automaton);

/* Start the control thread */

    if (!THREAD_CREATE(&tid, CtrlThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: CtrlThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    qdpDebug(qp, "%s: SEM_WAIT() for CtrlThread launch\n", fid);
    SEM_WAIT(&qp->fsa.semaphore.Automaton);

/* Start the app command processor */

    if (!THREAD_CREATE(&tid, AppcmdThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: AppcmdThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    qdpDebug(qp, "%s: SEM_WAIT() for AppcmdThread launch\n", fid);
    SEM_WAIT(&qp->fsa.semaphore.Automaton);

/* Start the data link handler, if appropriate */

    if (qp->dataLink) {
        if (!THREAD_CREATE(&tid, DataThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: DataThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        qdpDebug(qp, "%s: SEM_WAIT() for DataThread launch\n", fid);
        SEM_WAIT(&qp->fsa.semaphore.Automaton);
    }

/* Start the heartbeat thread if configured */

    if (qp->par.hbeat.interval != 0) {
        if (!THREAD_CREATE(&tid, HeartbeatThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: HeartbeatThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        qdpDebug(qp, "%s: SEM_WAIT() for HeartbeatThread launch\n", fid);
        SEM_WAIT(&qp->fsa.semaphore.Automaton);
    }

/* Start the watchdog thread, if applicable */

    if (qp->par.interval.watchdog > 0) {
        if (!THREAD_CREATE(&tid, WatchdogThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: WatchdogThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        qdpDebug(qp, "%s: SEM_WAIT() for WatchdogThread launch\n", fid);
        SEM_WAIT(&qp->fsa.semaphore.Automaton);
    }

/* Do the registration handshake */

    SetState(qp, qp->fsa.state.initial);
    PostEvent(qp, QDP_EVENT_UP, NULL);

    qdpDebug(qp, "%s: SEM_WAIT() on automaton (up event posted)\n", fid);
    SEM_WAIT(&qp->fsa.semaphore.Automaton); /* this will block until TLU or TLD occurs */

    if (qp->errcode != QDP_ERR_NO_ERROR) {
        SetErrcode(qp);
        qdpDebug(qp, "%s: FAIL with error: %s", fid, qdpErrcodeString(qp->errcode));
        return FALSE;
    }

    qdpDebug(qp, "%s: return TRUE\n", fid);
    return TRUE;
}

/* Shutdown the automaton */

BOOL qdpShutdown(QDP *qp)
{
static char *fid = "qdpShutdown";

    if (qp == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    qdpDeregister(qp, TRUE);

    if (!PostEvent(qp, QDP_EVENT_OFF, NULL)) return FALSE;
    SEM_WAIT(&qp->fsa.semaphore.Automaton);

    qdpDestroyHandle(qp);

    return TRUE;
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
 * $Log: fsa.c,v $
 * Revision 1.34  2017/02/03 20:46:58  dechavez
 * extended invalid action code message to include information about context,
 * so I can then go back and actually understand where it got hit
 *
 * Revision 1.33  2016/09/23 21:06:30  dechavez
 * added missing QDP_ACTION_RQ_C1_LOG case to EvenHandler() dispatcher
 *
 * Revision 1.32  2016/08/19 16:16:29  dechavez
 * eliminated CloseSockets(), now TerminateSelf() closes all the sockets when last
 * thread exits. TerminateSelf() is now the *only* place where these sockets can get closed.
 * qdpShutdown() now calls qdpDeregister() (fixes some persistent "busy server" problems)
 * AppcmdThread() initializes new QDP_CMD 'attempts' field to zero when new command received
 *
 * Revision 1.31  2016/08/15 19:40:01  dechavez
 * removed a whole bunch of text that inadvertently got saved as a CVS comment
 * when I did a :r fsa.c to look at some context and then forgot to undo.
 *
 * Revision 1.30  2016/08/15 19:36:48  dechavez
 * Replaced extended sleeps with 5msec polling loops, thus eliminating the need
 * for QDP_PAR_OPTION_QUICK_EXITS option. TerminateSelf() now only posts to
 * the automaton semaphore when last thread is gone, leaving it to qdpShutdown()
 * to wait on said semaphore and to close sockets after all threads are finished.
 *
 * Revision 1.29  2016/08/04 21:43:17  dechavez
 * a whole lot more debugging done... seems ready now
 *
 * Revision 1.28  2016/07/20 16:58:34  dechavez
 * Hoo Boy.  (Checkpoint commit following major redesign of state machine.  Things are
 * working under normal conditions but common failure modes have yet to be fully tested)
 * Reworked how all the various threads get started (introducing the "launch" feature from the new START
 * action handler, which I'll probably back out now that I started thinking about how machine restarts should work).
 * Added smarts to figure out if the machine failed due to bad auth code (QDP_ERR_BADAUTH) or lack of a
 * responding peer (QDP_ERR_NORESPONSE).  That I'll keep.  Added retry logic to optionally deal with busy
 * servers (which previously was being handled at the application level).  That I'll keep too.
 *
 * Revision 1.27  2016/06/16 15:36:22  dechavez
 * added QDP_ACTION_GIVEUP branch to EventHandler()
 *
 * Revision 1.26  2016/02/11 19:01:28  dechavez
 * changed QDP nsrq to ntry and QDP_PAR maxresp to maxtry, added ActionDECODECH(),
 * added code to ActionBLDRSP to track attempts and fail when excessive
 *
 * Revision 1.25  2016/02/05 16:50:27  dechavez
 * qdpStartFSA() checks qp->errcode instead of qp->regerr for machine start-up status
 *
 * Revision 1.24  2016/02/03 18:49:46  dechavez
 * changed ActionDecide to ActionDECIDE, just to keep with the style
 *
 * Revision 1.23  2016/02/03 17:37:55  dechavez
 * changed ActionRQCNF() to ActionRQCMB(), ActionLDCNF() to ActionLDCMB(),
 * added QDP_ACTION_DECIDE, QDP_ACTION_RQFIX, QDP_ACTION_LDFIX support to EventHandler()
 *
 * Revision 1.22  2016/01/27 00:23:49  dechavez
 * added code to generate QDP_EVENT_EPD and invoke QDP_ACTION_RQEPD, QDP_ACTION_LDEPD
 *
 * Revision 1.21  2015/12/03 18:08:54  dechavez
 * Added various qdpDebug() statements
 *
 * Revision 1.20  2014/08/11 18:07:39  dechavez
 * MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.19  2011/02/03 17:52:15  dechavez
 * qdpStartFSA() brings down machine if registration fails,
 * while last instance of TerminateSelf() closes all sockets
 *
 * Revision 1.18  2011/02/01 19:38:44  dechavez
 * fixed bug preventing delivery of C1_MYSN and C1_PHY after registration
 *
 * Revision 1.17  2011/01/31 21:16:04  dechavez
 * added support for determining console port IP address
 *
 * Revision 1.16  2011/01/13 20:29:52  dechavez
 * added QDP_EVENT_MYSN, QDP_ACTION_POLLSN, and QDP_ACTION_SAVESN support
 *
 * Revision 1.15  2010/12/06 17:27:49  dechavez
 * fixed (benign?) nthread initialization oversight in qdpStartFSA(), changed
 * TerminateSelf() thread decrement messages to qdpInfo.
 *
 * Revision 1.14  2010/03/31 20:36:00  dechavez
 * changed qdpState() to qdpFsaState(), added QDP_SHUTDOWN support, removed link trasher
 *
 * Revision 1.13  2010/03/22 21:40:20  dechavez
 * added QDP_ACTION_BUSY branch, terminate registration if busy and QDP_PAR_OPTION_QUITBUSY set
 *
 * Revision 1.12  2009/03/20 21:16:31  dechavez
 * Fixed silly typo in THREAD_DETACH macro
 *
 * Revision 1.11  2009/03/17 18:23:53  dechavez
 * THREAD_DETACH all threads
 *
 * Revision 1.10  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
