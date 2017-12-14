/*======================================================================
 *
 * Axquisition from a Q330 using Quanterra Lib330 library
 *
 *====================================================================*/
#include <q330.h>
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_Q330
//static SEMAPHORE sem;
static LNKLST *head = NULL;

#define DEFAULT_RETRY_SEC    30
#define DEFAULT_WATCHDOG_SEC 60

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

static BOOL InitQ330(ISI330_CONFIG *cfg, Q330 *newq330, Q330_CFG *q330cfg, char *argstr)
{
    char *name;
    int dp;
    Q330_ADDR qcfg;
    int debug = 0;
    LNKLST *TokenList;
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
    dp = atoi((char *) TokenList->array[1]);
    if ((dp < LP_TEL1) || (dp > LP_TEL4)) {
        fprintf(stderr, "%s is an incorrect q330 data port number\n", (char *) TokenList->array[1]);
        return FALSE;
    }

/* Get serial number, and auth code from the name */

    if (!q330GetAddr(name, q330cfg, &qcfg)) {
        fprintf(stderr, "Unable to locate '%s' in Q330 config file '%s'\n", name, q330cfg->path.addr);
        return FALSE;
    }

    MUTEX_INIT(&newq330->mutex);
    newq330->host = name;
    newq330->lp = cfg->lp;
    newq330->first = TRUE;
    newq330->dp = (UINT16)dp;
    newq330->sn = qcfg.serialno;
    newq330->authcode = qcfg.authcode;
    newq330->debug = debug;

    newq330->retry    = DEFAULT_RETRY_SEC * MSEC_PER_SEC;
    newq330->watchdog = DEFAULT_WATCHDOG_SEC * MSEC_PER_SEC;

    /* initialize tpar_create struct */

    UINT32 snhi = (UINT32) (qcfg.serialno >> 32);
    UINT32 snlo = (UINT32) (qcfg.serialno & 0x00000000FFFFFFFF);
    newq330->tpc.q330id_serial[0] = snlo;
    newq330->tpc.q330id_serial[1] = snhi;
    newq330->tpc.q330id_dataport = (UINT16)dp;
    strncpy(newq330->tpc.q330id_station, cfg->site, 6);
    newq330->tpc.host_timezone = 0;         // using UTC on host systems
    strcpy(newq330->tpc.host_software, "IDA:isi330");         // host application name
    strcpy(newq330->tpc.opt_contfile, "");       // disable for now
    newq330->tpc.opt_verbose = VERB_SDUMP | VERB_RETRY | VERB_REGMSG | VERB_LOGEXTRA | VERB_AUXMSG | VERB_PACKET;
    newq330->tpc.opt_zoneadjust = 1;        // no need, on UTC
    newq330->tpc.opt_secfilter = OSF_DATASERV;         // not using 1-sec callback
    newq330->tpc.opt_client_msgs = 10;      // set to min, NOT SURE HOW THIS IS USED
    newq330->tpc.opt_minifilter = OMF_ALL;  // send all messages
    newq330->tpc.opt_aminifilter = 0;       // disabling, I think...
    newq330->tpc.opt_compat = 0;            // using flag bits in tokens
    newq330->tpc.amini_exponent = 12;        // not using 'archival' ms, but set to 512 byte records
    newq330->tpc.amini_512highest = 20;     // 40hz, but not relevant
    newq330->tpc.mini_embed = 1;            // embed calibration and event blockettes in miniseed
    newq330->tpc.mini_separate = 0;         // generate sSEPARATE mniniseed records for cal and event blockettes
    newq330->tpc.mini_firchain = NULL;      // just using built-in FIR filters
    newq330->tpc.call_minidata = isi330_miniseed_callback;      // NEED TO HAVE CALLBACK FOR DATA COLLECTION
    newq330->tpc.call_aminidata = NULL;     // not collecting archival miniseed
    newq330->tpc.resp_err = LIBERR_NOERR;
    newq330->tpc.call_state = isi330_state_callback;         // state change callback??? Will probably want this
    newq330->tpc.call_messages = isi330_msg_callback;      // message callback, will want this later
    newq330->tpc.call_secdata = NULL;       // not collecting 1-sec data;
    newq330->tpc.call_lowlatency = NULL;    // NYI
    newq330->tpc.call_baler = NULL;         // not using
    newq330->tpc.file_owner = NULL;         // until better understood. Used in libsupport

    /* initialize registration struct */

    newq330->tpr.q330id_auth[0] = 0;
    newq330->tpr.q330id_auth[1] = 0;
    strcpy(newq330->tpr.q330id_address, newq330->host);
    newq330->tpr.q330id_baseport = 5330;
    newq330->tpr.host_mode = HOST_ETH;
    strcpy(newq330->tpr.host_interface, "");
    newq330->tpr.host_mincmdretry = 2;
    newq330->tpr.host_maxcmdretry = 30;
    newq330->tpr.host_ctrlport = 9999;
    newq330->tpr.host_dataport = 9998;
    // cfg->tpr.serial_flow = 0;
    // cfg->tpr.serial_baud = 9600;
    // cfg->tpr.serial_hostip = "";
    newq330->tpr.opt_latencytarget = 0;
    newq330->tpr.opt_closedloop = 0;
    newq330->tpr.opt_dynamic_ip = 0;
    newq330->tpr.opt_hibertime = 2;
    newq330->tpr.opt_conntime = 10;
    newq330->tpr.opt_connwait = 1;
    newq330->tpr.opt_regattempts = 3;
    newq330->tpr.opt_ipexpire = 0;
    newq330->tpr.opt_buflevel = 10;
    newq330->tpr.opt_q330_cont = 10;
    newq330->tpr.opt_dss_memory = 1024;

    return TRUE;
}

/* Add a new Q330 to the list */

char *AddQ330(ISI330_CONFIG *cfg, char *argstr, char *root)
{
Q330 newq330;
static Q330_CFG *q330cfg = NULL;
static char *fid = "AddQ330";
int errcode;

/* Read the config file once, the first time here */

    if (q330cfg == NULL) {
        if ((q330cfg = q330ReadCfg(root, &errcode)) == NULL) {
            q330PrintErrcode(stderr, "q330ReadCfg: ", root, errcode);
            return NULL;
        }
    }

    if (!InitQ330(cfg, &newq330, q330cfg, argstr)) return NULL;
    if (!listAppend(&cfg->q330list, &newq330, sizeof(Q330))) {
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
//    int errcode, suberr;
//    BOOL first = TRUE;

//    tcontext ct;
    enum tliberr liberr;
    enum tlibstate libstate, new_state;
    topstat op_stat;
    string63 state_str;
//    string95 msg_buf;
    INT32 status = 0;

    static char *fid = "Q330Thread";

    q330 = argptr;

    /* create station thread context */
    lib_create_context(q330->ct, &q330->tpc);
    if (q330->tpc.resp_err != 0) {
        LogMsg(LOG_INFO, "%s: lib_create_context %s:%d; resp_err=%d", fid,
               q330->host, q330->dp, q330->tpc.resp_err);
        exit(MY_MOD_ID + 1);
    }
    LogMsg(LOG_INFO, "station context created for host %s:%d", q330->host, q330->dp);

//    strcpy(msg_buf, "COMPLETED: lib_create_context\n");
//    lib_msg_add(q330->ct, LIBMSG_USER, 0, &msg_buf);

    libstate = lib_get_state(q330->ct, &liberr, &op_stat);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        printf("libstate: %s\n", lib_get_statestr(libstate, &state_str));
    }
    /* ping q330 to see if can register with q330 */

    /* liberr = lib_unregistered_ping(ct, cfg->tpr); */
    /* if (liberr != LIBERR_NOERR) { */
    /*     PrintLib330Tliberr(liberr); */
    /* } else { */
    /*     printf("lib_unregistered_ping successful\n"); */
    /* } */

    liberr = lib_register(q330->ct, &q330->tpr);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        printf("lib_register successful\n");
    }


    libstate = lib_get_state(q330->ct, &liberr, &op_stat);
    while (1) {
        if ((status = ExitStatus()) == 0) {
            sleep(1);

            new_state = lib_get_state(q330->ct, &liberr, &op_stat);
            if (new_state != libstate) {
                libstate = new_state;
                printf("NEW State [%d]: %s\n", (int)libstate, lib_get_statestr(libstate, &state_str));
                switch (libstate) {
                    case LIBSTATE_IDLE :
                    case LIBSTATE_TERM :
                    case LIBSTATE_PING :
                    case LIBSTATE_CONN :
                    case LIBSTATE_ANNC :
                    case LIBSTATE_REG :
                    case LIBSTATE_READCFG :
                    case LIBSTATE_READTOK :
                    case LIBSTATE_DECTOK :
                    case LIBSTATE_RUNWAIT :
                    case LIBSTATE_RUN :
                    case LIBSTATE_DEALLOC :
                    case LIBSTATE_DEREG :
                    case LIBSTATE_WAIT :
                    default:
                        break;
                }
            }
        } else {
            LogMsg(LOG_INFO, "shutdown initiated: exit flag = %ld", status);
            GracefulExit(status);
        }
    }

}

/* Launch all Q330 readers */

void StartQ330Readers(ISI330_CONFIG *cfg)
{
    THREAD tid;
    Q330 *q330;
    LNKLST_NODE *crnt;
    static char *fid = "StartQ330Thread";

//    SEM_INIT(&sem, 0, 1);
    head = &cfg->q330list;

    LogMsg(LOG_INFO, "Q330 config file = %s", cfg->cfgpath);
    LogMsg(LOG_INFO, "Q330 site = %s", cfg->site);
    LogMsg(LOG_INFO, "Q330 registration retry interval = %d sec", DEFAULT_RETRY_SEC);
    LogMsg(LOG_INFO, "Q330 watchdog interval = %d sec", DEFAULT_WATCHDOG_SEC);

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        if (crnt->payload == NULL) {
            LogMsg(LOG_INFO, "%s: crnt->payload == NULL", fid);
            SetExitStatus(MY_MOD_ID + 7);
            return;
        }
        q330 = (Q330 *) crnt->payload;
        q330->lp = cfg->lp;
        if (!THREAD_CREATE(&tid, Q330Thread, (void *) q330)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: Q330Thread: %s", fid, strerror(errno));
            SetExitStatus(MY_MOD_ID + 7);
            return;
        }
//        SEM_WAIT(&sem);
        crnt = listNextNode(crnt);
    }
}

void ShutdownQ330Readers(ISI330_CONFIG *cfg)
{
    Q330 *q330;
    LNKLST_NODE *crnt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        q330 = (Q330 *) crnt->payload;

        lib_change_state(*q330->ct, LIBSTATE_IDLE, LIBERR_NOERR);
        lib_change_state(*q330->ct, LIBSTATE_TERM, LIBERR_NOERR);
        lib_destroy_context(q330->ct);
        LogMsg(LOG_INFO, "Disconnected from site %s Q330: %s:%d\n", cfg->site, q330->host, q330->dp);

        crnt = listNextNode(crnt);
    }

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
