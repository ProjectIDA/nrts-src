/*======================================================================
 *
 * Axquisition from a Q330 using Quanterra Lib330 library
 *
 *====================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_Q330

#define DEFAULT_RETRY_SEC    30
#define DEFAULT_WATCHDOG_SEC 60


/* Initialize a new Q330 (except for metadata) */

BOOL InitQ330(ISI330_CONFIG *cfg, Q330_CFG *q330db)
{
    char *name;
    int dp;
    Q330_ADDR qcfg; // from q330 cfg database
//    int debug = 0;
    LNKLST *TokenList;
    static char *fid = "InitQ330";

    /* Parse the argument list */
    /* should have hostname/ip plus q330 DP to talk to ':' separated */

    if ((TokenList = utilStringTokenList(cfg->q330HostArgstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }
    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count != 2) {
        fprintf(stderr, "%s: incorrect q330 string \"%s\"\n", fid, cfg->q330HostArgstr);
        return FALSE;
    }

    name = (char *) TokenList->array[0];
    dp = atoi((char *) TokenList->array[1]);
    if ((dp < LP_TEL1 + 1) || (dp > LP_TEL4 + 1)) {
        fprintf(stderr, "%s: %s is an incorrect q330 data port number\n", fid, (char *) TokenList->array[1]);
        return FALSE;
    }

    /* create isi330 Q330 struct */

    if ((cfg->q330 = (ISI330_Q330 *) malloc(sizeof(ISI330_Q330))) == NULL) {
        fprintf(stderr, "%s malloc ISI330_Q330", fid);
        return FALSE;
    }

    /* Get serial number, and auth code from the name */

    if (!q330GetAddr(name, q330db, &qcfg)) {
        fprintf(stderr, "Unable to locate '%s' in Q330 config file '%s'\n", name, q330db->path.addr);
        return FALSE;
    }

    MUTEX_INIT(&cfg->q330->mutex);
    cfg->q330->host = name;
    cfg->q330->lp = cfg->lp;
    cfg->q330->first = TRUE;
    cfg->q330->dp = (UINT16)dp;
    cfg->q330->sn = qcfg.serialno;
    cfg->q330->authcode = qcfg.authcode;
    cfg->q330->debug = 0; //debug;

    cfg->q330->retry    = DEFAULT_RETRY_SEC * MSEC_PER_SEC;
    cfg->q330->watchdog = DEFAULT_WATCHDOG_SEC * MSEC_PER_SEC;

    /* initialize tpar_create struct */

    UINT32 snhi = (UINT32) (qcfg.serialno >> 32);
    UINT32 snlo = (UINT32) (qcfg.serialno & 0x00000000FFFFFFFF);
    cfg->q330->tpc.q330id_serial[0] = snlo;
    cfg->q330->tpc.q330id_serial[1] = snhi;
    cfg->q330->tpc.q330id_dataport = (UINT16)dp - 1; // lib330 uses 0-based dataport enumeration
    strncpy(cfg->q330->tpc.q330id_station, cfg->site, 6);
    cfg->q330->tpc.host_timezone = 0;         // using UTC on host systems
    strcpy(cfg->q330->tpc.host_software, "IDA:isi330");         // host application name
    strcpy(cfg->q330->tpc.opt_contfile, "");       // disable for now
    cfg->q330->tpc.opt_verbose = VERB_SDUMP | VERB_RETRY | VERB_REGMSG | VERB_LOGEXTRA | VERB_AUXMSG | VERB_PACKET;
    cfg->q330->tpc.opt_verbose = VERB_RETRY | VERB_REGMSG | VERB_AUXMSG;
    cfg->q330->tpc.opt_zoneadjust = 1;        // no need, on UTC
    cfg->q330->tpc.opt_secfilter = OSF_ALL;   // no callback specified, not using 1-sec callback
    cfg->q330->tpc.opt_client_msgs = 10;      // set to min, NOT SURE HOW THIS IS USED
    cfg->q330->tpc.opt_minifilter = OMF_ALL;  // send all messages
    cfg->q330->tpc.opt_aminifilter = 0;       // disabling, I think...
    cfg->q330->tpc.opt_compat = 0;            // using flag bits in tokens
    cfg->q330->tpc.amini_exponent = 12;       // not using 'archival' ms, but set to 512 byte records
    cfg->q330->tpc.amini_512highest = 20;     // 40hz, but not relevant
    cfg->q330->tpc.mini_embed = 1;            // embed calibration and event blockettes in miniseed
    cfg->q330->tpc.mini_separate = 0;         // generate sSEPARATE mniniseed records for cal and event blockettes
    cfg->q330->tpc.mini_firchain = NULL;      // just using built-in FIR filters
    cfg->q330->tpc.call_minidata = isi330_miniseed_callback;      // NEED TO HAVE CALLBACK FOR DATA COLLECTION
    cfg->q330->tpc.call_aminidata = NULL;     // not collecting archival miniseed
    cfg->q330->tpc.resp_err = LIBERR_NOERR;
    cfg->q330->tpc.call_state = isi330_state_callback;         // state change callback??? Will probably want this
    cfg->q330->tpc.call_messages = isi330_msg_callback;      // message callback, will want this later
    cfg->q330->tpc.call_secdata = NULL;       // not using 1-sec data callback;
    cfg->q330->tpc.call_lowlatency = NULL;    // NYI
    cfg->q330->tpc.call_baler = NULL;         // not using
    cfg->q330->tpc.file_owner = NULL;         // until better understood. Used in libsupport

    /* initialize registration struct */

    cfg->q330->tpr.q330id_auth[0] = 0;
    cfg->q330->tpr.q330id_auth[1] = 0;
    strcpy(cfg->q330->tpr.q330id_address,cfg->q330->host);
    cfg->q330->tpr.q330id_baseport = 5330;
    cfg->q330->tpr.host_mode = HOST_ETH;
    strcpy(cfg->q330->tpr.host_interface, "");
    cfg->q330->tpr.host_mincmdretry = 2;
    cfg->q330->tpr.host_maxcmdretry = 30;
    cfg->q330->tpr.host_ctrlport = ISI330_HOST_CTRLPORT_BASE + qcfg.instance;
    cfg->q330->tpr.host_dataport = ISI330_HOST_DATAPORT_BASE + qcfg.instance;
    LogMsg("CTRL PORT: %d\n", cfg->q330->tpr.host_ctrlport);
    LogMsg("DATA PORT: %d\n", cfg->q330->tpr.host_dataport);
    // cfg->tpr.serial_flow = 0;
    // cfg->tpr.serial_baud = 9600;
    // cfg->tpr.serial_hostip = "";
    cfg->q330->tpr.opt_latencytarget = 0;
    cfg->q330->tpr.opt_closedloop = 0;
    cfg->q330->tpr.opt_dynamic_ip = 0;
    cfg->q330->tpr.opt_hibertime = 0;
    cfg->q330->tpr.opt_conntime = 0;
    cfg->q330->tpr.opt_connwait = 0;
    cfg->q330->tpr.opt_regattempts = 0;
    cfg->q330->tpr.opt_ipexpire = 0;
    cfg->q330->tpr.opt_buflevel = 0;
    cfg->q330->tpr.opt_q330_cont = 0;
    cfg->q330->tpr.opt_dss_memory = 0;

    return TRUE;
}

void LoadQ330Host(ISI330_CONFIG *cfg, Q330_CFG *q330cfg)
{
    static char *fid = "LoadQ330Hosts";

    if (cfg->q330HostArgstr == NULL) {
        LogMsg("%s: cfg->q330HostArgstr == NULL", fid);
        exit(MY_MOD_ID + 7);
    }

    if (!InitQ330(cfg, q330cfg)) exit(MY_MOD_ID + 8);
}

/* Read packets from the Q330.  Since we are using the user supplied
 * packet function, we let the library handle the data managment and
 * just monitor the timestamp to ensure the machine is running.
 */

static THREAD_FUNC Q330Thread(void *argptr)
{
    ISI330_Q330 *q330;

    enum tliberr liberr;
    enum tlibstate libstate, new_state;
    topstat op_stat;
    string63 state_str;
    INT32 status = 0;

    static char *fid = "Q330Thread";

    q330 = argptr;

    /* create station thread context */
    if ((q330->ct = malloc(sizeof(tcontext))) == NULL) {
        LogMsg("%s: malloc(tconext): %s", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }
    lib_create_context(q330->ct, &q330->tpc);
    if (q330->tpc.resp_err != 0) {
        LogMsg("%s: lib_create_context %s:%d; resp_err=%d", fid,
               q330->host, q330->dp, q330->tpc.resp_err);
        exit(MY_MOD_ID + 2);
    }
    LogMsg("station context created for host %s:%d", q330->host, q330->dp);

//    strcpy(msg_buf, "COMPLETED: lib_create_context\n");
//    lib_msg_add(q330->ct, LIBMSG_USER, 0, &msg_buf);

    libstate = lib_get_state(*q330->ct, &liberr, &op_stat);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        LogMsg("libstate: %s\n", lib_get_statestr(libstate, &state_str));
    }
    /* ping q330 to see if can register with q330 */

    /* liberr = lib_unregistered_ping(ct, cfg->tpr); */
    /* if (liberr != LIBERR_NOERR) { */
    /*     PrintLib330Tliberr(liberr); */
    /* } else { */
    /*     printf("lib_unregistered_ping successful\n"); */
    /* } */

    liberr = lib_register(*q330->ct, &q330->tpr);
    if (liberr != LIBERR_NOERR) {
        PrintLib330Tliberr(liberr);
    } else {
        LogMsg("lib_register successful\n");
    }

    libstate = lib_get_state(*q330->ct, &liberr, &op_stat);
    while (1) {

        if ((status = ExitStatus()) == 0) {
            sleep(1);
//            LogMsg("CUR State for %s:%d [%d]: %s\n", q330->host, q330->dp, (int)libstate, lib_get_statestr(libstate, &state_str));

            new_state = lib_get_state(*q330->ct, &liberr, &op_stat);
            if (new_state != libstate) {
                libstate = new_state;
                LogMsg("NEW State for %s:%d [%d]: %s\n", q330->host, q330->dp, (int)libstate, lib_get_statestr(libstate, &state_str));
                switch (libstate) {
                    case LIBSTATE_IDLE : break;
                    case LIBSTATE_TERM : break;
                    case LIBSTATE_PING : break;
                    case LIBSTATE_CONN : break;
                    case LIBSTATE_ANNC : break;
                    case LIBSTATE_REG : break;
                    case LIBSTATE_READCFG : break;
                    case LIBSTATE_READTOK : break;
                    case LIBSTATE_DECTOK : break;
                    case LIBSTATE_RUNWAIT :
                        LogMsg("REQ State change for %s:%d TO state [%d]: %s\n",
                               q330->host, q330->dp, (int)libstate, lib_get_statestr(LIBSTATE_RUN, &state_str));
                        lib_change_state(*q330->ct, LIBSTATE_RUN, liberr);
                        break;
                    case LIBSTATE_RUN : break;
                    case LIBSTATE_DEALLOC : break;
                    case LIBSTATE_DEREG : break;
                    case LIBSTATE_WAIT : break;
                    default:
                        break;
                }
            }
        } else {
            LogMsg("shutdown initiated: exit flag = %ld", status);
            GracefulExit(status);
        }
    }

}

/* Launch all Q330 readers */

void StartQ330Reader(ISI330_CONFIG *cfg)
{
    THREAD tid;
    ISI330_Q330 *q330;
    static char *fid = "StartQ330Reader";


    LogMsg("Q330 Argstr = %s", cfg->q330HostArgstr);
    LogMsg("Q330 config file = %s", cfg->cfgpath);
    LogMsg("Q330 site = %s", cfg->site);
    LogMsg("Q330 registration retry interval = %d sec", DEFAULT_RETRY_SEC);
    LogMsg("Q330 watchdog interval = %d sec", DEFAULT_WATCHDOG_SEC);

    if (cfg->q330 != NULL) {
        if (!THREAD_CREATE(&tid, Q330Thread, (void *) cfg->q330)) {
            LogMsg("%s: THREAD_CREATE: Q330Thread: %s", fid, strerror(errno));
            SetExitStatus(MY_MOD_ID + 7);
            return;
        }
    } else {
        LogMsg("%s: crnt->payload == NULL", fid);
        SetExitStatus(MY_MOD_ID + 7);
        return;
    }
}

void ShutdownQ330Reader(ISI330_CONFIG *cfg)
{
    enum tlibstate curstate;
    enum tliberr liberr;
    topstat op_stat;
    struct timespec reqtp, remtp;
    struct timeval tv1, tv2, tv3, tv4;
    struct timezone tz;

    reqtp.tv_sec = 0;
    reqtp.tv_nsec = 500000000; // half a second
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;

    if (cfg->q330 != NULL) {
        LogMsg("Deregistering from site %s Q330: %s:%d\n", cfg->site, cfg->q330->host, cfg->q330->dp);

        gettimeofday(&tv1, &tz);

        lib_change_state(*cfg->q330->ct, LIBSTATE_IDLE, LIBERR_NOERR);

        // give library a change to change state. Empirically measured to take < .5 secs
        // even if it takes longer and deregister isn't clean,
        // reconnection using same ports will works
        nanosleep(&reqtp, &remtp);

        lib_change_state(*cfg->q330->ct, LIBSTATE_TERM, LIBERR_NOERR);
        nanosleep(&reqtp, &remtp);

        lib_destroy_context(cfg->q330->ct);

        LogMsg("Disconnected from site %s Q330: %s:%d\n", cfg->site, cfg->q330->host, cfg->q330->dp);
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
