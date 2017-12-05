#pragma ident "$Id: q330.c,v 1.31 2016/08/26 20:21:13 dechavez Exp $"
/*======================================================================
 *
 * Service a Quanterra Q330
 *
 *====================================================================*/
#include "isidl.h"
#include "qdp.h"

#define MY_MOD_ID ISIDL_MOD_Q330
static SEMAPHORE sem;
static LNKLST *head = NULL;

#define DEFAULT_RETRY_SEC    30
#define DEFAULT_WATCHDOG_SEC 60

static void SetReceiveTstamp(Q330 *q330, UINT64 tstamp, int type)
{
    MUTEX_LOCK(&q330->mutex);
        switch (type) {
          case QDP_DT_DATA: q330->tstamp.dt_data = tstamp; break;
          case QDP_C1_STAT: q330->tstamp.c1_stat = tstamp; break;
          default:          q330->tstamp.other   = tstamp; break;
        }
    MUTEX_UNLOCK(&q330->mutex);
}

static UINT32 MsecSinceLastData(Q330 *q330)
{
UINT32 msec;
UINT64 last, interval;

    MUTEX_LOCK(&q330->mutex);
        last = q330->tstamp.dt_data;
    MUTEX_UNLOCK(&q330->mutex);

    interval = utilTimeStamp() - last;
    msec = interval / NANOSEC_PER_MSEC;

    return msec;
}

/* Toggle debug mode on all Q330 readers */

void ToggleQ330DebugState(void)
{
Q330 *q330;
UINT64 serialno;
LNKLST_NODE *crnt;
static char *fid = "ToggleQ330DebugState";

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        q330 = (Q330 *) crnt->payload;
        MUTEX_LOCK(&q330->mutex);
            q330->par.debug = (q330->par.debug == QDP_TERSE) ? QDP_DEBUG : QDP_TERSE;
            if (q330->qp != NULL) q330->qp->par.debug = q330->par.debug;
            serialno = q330->par.serialno;
        MUTEX_UNLOCK(&q330->mutex);
        LogMsg(LOG_INFO, "%s: %016llx QDP log level %d", fid, serialno, q330->par.debug);
        crnt = listNextNode(crnt);
    }
}

/* called by qdp library each time it gets a packet for the app */

void SaveQ330Packet(void *args, QDP_PKT *pkt)
{
Q330 *q330;
static char *fid = "SaveQ330Packet";

    if (ExitStatus() != 0) return;

    q330 = (Q330 *) args;

/* Note the time */

    SetReceiveTstamp(q330, utilTimeStamp(), pkt->hdr.cmd);

/* Add our serial number to the header */

    InsertQdplusSerialno(&q330->local.raw, q330->par.serialno);

/* Copy in the QDP packet from the digitizer */

    CopyQDPToQDPlus(&q330->local.raw, pkt);

    if (q330->first && pkt->hdr.cmd == QDP_DT_DATA) {
        LogMsg(LOG_INFO, "initial data packet received from %s:%s", q330->par.connect.ident, qdpPortString(q330->par.port.link));
        q330->first = FALSE;
    }

/* Save it */

    ProcessLocalData(&q330->local);
}

/* called by the qdp library each time it gets a new set of handshake meta-data */

void SaveQ330Meta(void *args, QDP_META *meta)
{
Q330 *q330;
static char *fid = "SaveQ330Meta";

    q330 = (Q330 *) args;
    ProcessMetaData(q330, meta);
}

/* Initialize a new Q330 (except for metadata) */

static BOOL InitQ330(ISIDL_PAR *par, Q330 *q330, Q330_CFG *cfg, char *argstr)
{
char *name;
Q330_ADDR addr;
int port, debug = 0;
LNKLST *TokenList;
UINT64 serialno, authcode;
static char *fid = "InitQ330";

/* Parse the argument list */

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }
    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count != 2 && TokenList->count != 3) {
        fprintf(stderr, "incorrect q330 string \"%s\"\n", argstr);
        return FALSE;
    }

    if (TokenList->count == 3) debug = atoi((char *) TokenList->array[2]);

    name = (char *) TokenList->array[0];
    if ((port = qdpDataPortNumber(atoi((char *) TokenList->array[1]))) == QDP_UNDEFINED_PORT) {
        fprintf(stderr, "%s is an incorrect q330 data port number\n", (char *) TokenList->array[1]);
        return FALSE;
    }

/* Get serial number, and auth code from the name */

    if (!q330GetAddr(name, cfg, &addr)) {
        fprintf(stderr, "Unable to locate '%s' in Q330 config file '%s'\n", name, cfg->path.addr);
        return FALSE;
    }

    MUTEX_INIT(&q330->mutex);
    qdpInitPar(&q330->par, port);
    qdpSetConnect(&q330->par, &addr.connect);
    qdpSetSerialno(&q330->par, addr.serialno);
    qdpSetAuthcode(&q330->par, addr.authcode);
    qdpSetMyCtrlPort(&q330->par, port + addr.instance);
    qdpSetMyDataPort(&q330->par, port + addr.instance);
    qdpSetDebug(&q330->par, debug);
    q330->lp = par->lp;
    q330->first = TRUE;
    q330->qp = NULL;
    q330->tstamp.dt_data = q330->tstamp.c1_stat = q330->tstamp.other = utilTimeStamp();

/* Hardcode these for now */

    q330->retry    = DEFAULT_RETRY_SEC * MSEC_PER_SEC;
    q330->watchdog = DEFAULT_WATCHDOG_SEC * MSEC_PER_SEC;

    return TRUE;
}

/* Add a new Q330 to the list */

char *AddQ330(ISIDL_PAR *par, char *argstr, char *root)
{
Q330 new;
static Q330_CFG *cfg = NULL;
static char *fid = "AddQ330";
int i, errcode;

/* Read the config file once, the first time here */

    if (cfg == NULL) {
        if ((cfg = q330ReadCfg(root, &errcode)) == NULL) {
            q330PrintErrcode(stderr, "q330ReadCfg: ", root, errcode);
            return NULL;
        }
    }

    if (!InitQ330(par, &new, cfg, argstr)) return NULL;
    if (!listAppend(&par->q330, &new, sizeof(Q330))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return NULL;
    }

    return cfg->path.addr;
}

/* Read packets from the Q330.  Since we are using the user supplied
 * packet function, we let the library handle the data managment and
 * just monitor the timestamp to ensure the machine is running.
 */

static THREAD_FUNC Q330Thread(void *argptr)
{
Q330 *q330;
int errcode, suberr;
BOOL first = TRUE;
static char *fid = "Q330Thread";

    q330 = (Q330 *) argptr;
    qdpSetUser(&q330->par, (void *) q330, SaveQ330Packet);
    qdpSetMeta(&q330->par, (void *) q330, SaveQ330Meta);
    SEM_POST(&sem);

    while (1) {

        while (q330->qp == NULL) {
            if (first) {
                first = FALSE;
            } else {
                LogMsg(LOG_INFO, "*** ALERT *** restarting %s:%s", q330->par.connect.ident, qdpPortString(q330->par.port.link));
            }
            if ((q330->qp = qdpConnect(&q330->par, q330->lp, &errcode, &suberr)) == NULL) {
                LogMsg(LOG_INFO, "%s:%s registration failed: %s", q330->par.connect.ident, qdpPortString(q330->par.port.link), qdpErrcodeString(errcode));
                utilDelayMsec(q330->retry);
            } else {
                LogMsg(LOG_INFO, "registered with %s:%s", q330->par.connect.ident, qdpPortString(q330->par.port.link));
            }
        }

        LogMsg(LOG_DEBUG, "%s: MsecSinceLastData = %d, q330->watchdog = %d\n", fid, MsecSinceLastData(q330), q330->watchdog);
        if (MsecSinceLastData(q330) > q330->watchdog) {
            LogMsg(LOG_INFO, "*** ALERT *** %s:%s: no data for over %d secs", q330->par.connect.ident, qdpPortString(q330->par.port.link), q330->watchdog / MSEC_PER_SEC);
            MUTEX_LOCK(&q330->mutex);
                qdpShutdown(q330->qp);
                q330->qp = NULL;
                q330->first = TRUE;
                q330->tstamp.dt_data = q330->tstamp.c1_stat = q330->tstamp.other = utilTimeStamp();
            MUTEX_UNLOCK(&q330->mutex);
        }
        utilDelayMsec(q330->watchdog / 4);
    }
}

/* Launch all Q330 readers */

void StartQ330Readers(ISIDL_PAR *par)
{
THREAD tid;
Q330 *q330;
LNKLST_NODE *crnt;
static char *fid = "StartQ330Thread";

    SEM_INIT(&sem, 0, 1);
    head = &par->q330;

    LogMsg(LOG_INFO, "Q330 config file = %s", par->cfgpath);
    LogMsg(LOG_INFO, "Q330 registration retry interval = %d sec", DEFAULT_RETRY_SEC);
    LogMsg(LOG_INFO, "Q330 watchdog interval = %d sec", DEFAULT_WATCHDOG_SEC);

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        q330 = (Q330 *) crnt->payload;
        q330->lp = par->lp;
        if (!THREAD_CREATE(&tid, Q330Thread, (void *) q330)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: Q330Thread: %s", fid, strerror(errno));
            SetExitStatus(MY_MOD_ID + 7);
            return;
        }
        SEM_WAIT(&sem);
        crnt = listNextNode(crnt);
    }
}

/* Revision History
 *
 * $Log: q330.c,v $
 * Revision 1.31  2016/08/26 20:21:13  dechavez
 * track time since DT_DATA, C1_STAT and "other" packets were received,
 * restart machine when there is a DT_DATA timeout
 *
 * Revision 1.30  2016/08/04 22:12:04  dechavez
 * reworked main loop in Q330Thread() to accomodate libqdp 3.13.1 changes
 *
 * Revision 1.29  2014/08/28 22:02:30  dechavez
 * fixed supious alert message from Q330Thread() ("first" was static, now it's not)
 *
 * Revision 1.28  2014/01/30 19:43:07  dechavez
 * rearranged Q330 restarting messages
 *
 * Revision 1.27  2011/02/03 18:02:25  dechavez
 * qdpShutdown() instead of qdpClose() when restarting, fixed error reporting no data interval
 *
 * Revision 1.26  2011/01/25 18:43:43  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.25  2011/01/14 00:33:09  dechavez
 * qdpConnectWithPar() to qdpConnect(), QDP_PAR "host" to "ident"
 *
 * Revision 1.24  2010/12/23 21:41:04  dechavez
 * initialize q330->tstamp
 *
 * Revision 1.23  2010/12/23 00:04:46  dechavez
 * use qdpDataPortNumber() to assign data port parameter
 *
 * Revision 1.22  2010/12/21 20:13:04  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 *
 * Revision 1.21  2010/12/17 19:48:36  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.20  2010/12/06 17:32:38  dechavez
 * watchdog restarts of Q330 I/O, currently using hardcoded retry intervals and
 * timeout threshold timeouts.
 *
 * Revision 1.19  2010/11/24 22:00:25  dechavez
 * q330ReadCfg with error code
 *
 * Revision 1.18  2010/04/02 18:35:27  dechavez
 * Removed deregistration exit handler, and explicilty set return UDP ports derived
 * from instance and data port numbers.  This is to ensure consistent return addresses
 * in order to avoid spurious "server busy" errors when reconnecting after ungraceful
 * shutdowns.  This is restoring the previous behaviour which I had explicitly put
 * in years ago for just this reason.  I removed it after I cleaned up the QDP library
 * code to permit clean disconnects with deregistration, however in real life the
 * application doesn't always get to stay up long enough for it to complete (at
 * FreeBSD system shutdown, for instance).
 *
 * Revision 1.17  2010/04/01 22:02:45  dechavez
 * added code to deregister Q330s
 *
 * Revision 1.16  2010/03/31 22:44:10  dechavez
 * added DeregisterQ330s() exit handler
 *
 * Revision 1.15  2010/03/31 21:03:29  dechavez
 * don't bother with instance numbers or client side ports
 *
 * Revision 1.14  2010/03/22 21:51:41  dechavez
 * Eliminated instance number from q330 specification, abort startup if any
 * q330s are found busy during handsake
 *
 * Revision 1.13  2009/07/27 17:37:06  dechavez
 * using q330GetX instead of q330LookupX (libq3301.1.0)
 *
 * Revision 1.12  2007/10/31 17:36:02  dechavez
 *  Lookup Q330_ADDR instead of serialno and authcode
 *
 * Revision 1.11  2007/09/07 20:05:38  dechavez
 * use Q330 config file to get address parameters via Q330_CFG
 *
 * Revision 1.10  2007/05/11 16:24:50  dechavez
 * fixed incorrect fid
 *
 * Revision 1.9  2007/02/20 02:35:14  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.8  2007/02/08 22:53:17  dechavez
 * LOG_ERR to LOG_INFO, use LOCALPKT handle
 *
 * Revision 1.7  2006/12/12 23:28:10  dechavez
 * use QDP metadata callback for tranparent saving of metadata
 *
 * Revision 1.6  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.5  2006/06/27 00:25:58  dechavez
 * switch to library qdpParseArgString() for parsing command line args
 *
 * Revision 1.4  2006/06/23 18:31:19  dechavez
 * Added client side port parameter to Q330 argument list
 *
 * Revision 1.3  2006/06/15 00:01:42  dechavez
 * added missing return value for InitQ330()
 *
 * Revision 1.2  2006/06/07 22:40:25  dechavez
 * block forever after return from qdpConnectWithPar()
 *
 * Revision 1.1  2006/06/02 21:07:56  dechavez
 * initial release
 *
 */
