/*======================================================================
 *
 * Axquisition from a Q330 using Quanterra Lib330 library
 *
 *====================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_Q330
//static SEMAPHORE sem;
static LNKLST *head = NULL;

//#define DEFAULT_RETRY_SEC    30
//#define DEFAULT_WATCHDOG_SEC 60

//
//static UINT32 MsecSinceLastData(Q330 *q330)
//{
//UINT32 msec;
//UINT64 last, interval;
//
//    MUTEX_LOCK(&q330->mutex);
//        last = q330->tstamp.dt_data;
//    MUTEX_UNLOCK(&q330->mutex);
//
//    interval = utilTimeStamp() - last;
//    msec = interval / NANOSEC_PER_MSEC;
//
//    return msec;
//}

/* Toggle debug mode on all Q330 readers */

//void ToggleQ330DebugState(void)
//{
//Q330 *q330;
//UINT64 serialno;
//LNKLST_NODE *crnt;
//static char *fid = "ToggleQ330DebugState";
//
//    crnt = listFirstNode(head);
//    while (crnt != NULL) {
//        q330 = (Q330 *) crnt->payload;
//        MUTEX_LOCK(&q330->mutex);
//            q330->par.debug = (q330->par.debug == QDP_TERSE) ? QDP_DEBUG : QDP_TERSE;
//            if (q330->qp != NULL) q330->qp->par.debug = q330->par.debug;
//            serialno = q330->par.serialno;
//        MUTEX_UNLOCK(&q330->mutex);
//        LogMsg(LOG_INFO, "%s: %016llx QDP log level %d", fid, serialno, q330->par.debug);
//        crnt = listNextNode(crnt);
//    }
//}


/* Initialize a new Q330 (except for metadata) */

static BOOL InitQ330(ISI330_CONFIG *cfg, Q330 *q330, Q330_CFG *q330cfg, char *argstr)
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

    if (!q330GetAddr(name, q330cfg, &addr)) {
        fprintf(stderr, "Unable to locate '%s' in Q330 config file '%s'\n", name, q330cfg->path.addr);
        return FALSE;
    }

    MUTEX_INIT(&q330->mutex);



    return TRUE;
}

/* Add a new Q330 to the list */

char *AddQ330(ISI330_CONFIG *cfg, char *argstr, char *root)
{
Q330 new;
static Q330_CFG *q330cfg = NULL;
static char *fid = "AddQ330";
int i, errcode;

/* Read the config file once, the first time here */

    if (q330cfg == NULL) {
        if ((q330cfg = q330ReadCfg(root, &errcode)) == NULL) {
            q330PrintErrcode(stderr, "q330ReadCfg: ", root, errcode);
            return NULL;
        }
    }

    if (!InitQ330(cfg, &new, q330cfg, argstr)) return NULL;
    if (!listAppend(&cfg->q330list, &new, sizeof(Q330))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return NULL;
    }

    return q330cfg->path.addr;
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

void StartQ330Readers(ISI330_CONFIG *cfg)
{
THREAD tid;
Q330 *q330;
LNKLST_NODE *crnt;
static char *fid = "StartQ330Thread";

    SEM_INIT(&sem, 0, 1);
    head = &cfg->q330;

    LogMsg(LOG_INFO, "Q330 config file = %s", cfg->cfgpath);
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