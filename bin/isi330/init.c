/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_INIT

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s q330=HostnameOrIP dp=DataPort sn=Q330SerialNumber sitename\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Required:\n");
    fprintf(stderr,"q330=Hostname       => name or IP address of q330 digitizer\n");
    fprintf(stderr,"dp=DataPort         => Q330 data port to connect to (1-4)\n");
    fprintf(stderr,"sn=Q330SerialNumber => Q330 16 char hexidecimal serial number\n");
    fprintf(stderr,"sitename            => Station code\n");
    // fprintf(stderr,"-bd       => run in the background\n");
    fprintf(stderr,"\n");
    exit(1);
}


ISI330_CONFIG *init(char *myname, int argc, char **argv)
{
    static BOOL debug = FALSE;
    static char *dbspec = NULL;
    static char *log = NULL;
    ISI330_CONFIG *cfg;
    char *user = ISI330_DEFAULT_USER;
    int i;
    static BOOL daemon = DEFAULT_DAEMON;
    char *cfgpath = NULL;
    char *station = NULL;
    char *dp_str = NULL;
    UINT16 dp;
    UINT64 sn = 0x0;
    char *q330host = NULL;

    if ((cfg = (ISI330_CONFIG *) malloc(sizeof(ISI330_CONFIG))) == NULL) {
        perror("malloc ISI330_CONFIG");
        exit(MY_MOD_ID + 1);
    }
    cfg->tpc = NULL;
    cfg->dp = 0;        // not valid
    cfg->q330host = NULL;
    memset(cfg->sn_str, 0, sizeof(cfg->sn_str));
    cfg->sn_str_lo = cfg->sn_str + 8;
    cfg->sn_str_hi = cfg->sn_str;
    cfg->sn = 0x0;
    cfg->sn_lo = 0x0;
    cfg->sn_hi = 0x0;

    /* create struct for lib330 thread context creation */
    if ((cfg->tpc = (tpar_create *) malloc(sizeof(tpar_create))) == NULL) {
        perror("malloc tpar_create");
        exit(MY_MOD_ID + 2);
    }

    /* create struct for lib330 registration */
    /* if ((cfg->tpr = (tpar_register *) malloc(sizeof(tpar_register))) == NULL) { */
    /*     perror("malloc tpar_register"); */
    /*     exit(MY_MOD_ID + 3); */
    /* } */

/*  Get command line arguments  */

     listInit(cfg->q330list);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        // } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
        //     log = argv[i] + strlen("log=");
//        } else if (strncmp(argv[i], "dp=", strlen("dp=")) == 0) {
//            dp_str = argv[i] + strlen("dp=");
        } else if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            cfg->q330host = argv[i] + strlen("q330=");
        } else if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            if ((par->cfgpath = AddQ330(par, argv[i] + strlen("q330="), cfgpath)) == NULL) {
                fprintf(stderr, "%s: failed to add Q330: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
            seedlink = NULL; /* SeedLink not applicable for QDP input */
        } else if (strncmp(argv[i], "sn=", strlen("sn=")) == 0) {
            strcpy(cfg->sn_str, argv[i] + strlen("sn="));
         } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
             if (cfgpath != NULL) {
                 fprintf(stderr, "ERROR: multilple instances of cfg argument are not allowed\n");
                 exit(MY_MOD_ID);
             }
             cfgpath = argv[i] + strlen("cfg=");
         } else if (strcmp(argv[i], "-bd") == 0) {
             daemon = TRUE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (cfg->stacode == NULL) {
            if ((cfg->stacode = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

/* Q330s can only be acquired with each other (and barometers) */

    // if (!listSetArrayView(q330list)) {
    //     fprintf(stderr, "%s: listSetArrayView: %s", myname, strerror(errno));
    //     exit(MY_MOD_ID);
    // }

    if (cfg->q330host == NULL) {
        fprintf(stderr,"%s: must specify q330 hostname(s)\n", myname);
        help(myname);
    }

/* Must specify data port */

    if (dp_str == NULL) {
        fprintf(stderr,"%s: missing Data Port\n", myname);
        help(myname);
    }
    cfg->dp = atoi(dp_str);
    if (cfg->dp == 0) {
        fprintf(stderr,"%s: invalid Data Port\n", myname);
        help(myname);
    }

/* Must specify serial number */

    if (cfg->sn_str[0] == 0) {
        fprintf(stderr,"%s: missing Q330 Serial Number\n", myname);
        help(myname);
    }
    cfg->sn = strtoull(cfg->sn_str, NULL, 16);
    /* cfg->sn_hi = strtoul(cfg->sn_str_hi, cfg->sn_str[7], 16); */
    /* cfg->sn_lo = strtoul(cfg->sn_str_lo, cfg->sn_str_lo+7, 16); */
    if (cfg->sn == 0) {
        fprintf(stderr,"%s: invalid Q330 Serial Number\n", myname);
        help(myname);
    }

/* Must specify site name */

    if (cfg->stacode == NULL) {
        fprintf(stderr,"%s: missing station code(s)\n", myname);
        help(myname);
    }

/* Load the database */

    // par->timeout *= MSEC_PER_SEC; /* IACP wants timeout in msec */

    // if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
    //     fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", myname, strerror(errno));
    //     exit(MY_MOD_ID);
    // }
    // if (trecs != 0) glob.trecs = trecs;

    // if (dbg.pkt) glob.debug.pkt = LOG_INFO;
    // if (dbg.ttag) glob.debug.ttag = LOG_INFO;
    // if (dbg.bwd) glob.debug.bwd = LOG_INFO;
    // if (dbg.dl) glob.debug.dl = LOG_INFO;
    // if (dbg.lock) glob.debug.lock = LOG_INFO;
    // if (flags != 0) glob.flags |= flags;

    // utilSetIdentity(user);

/* Connect to the disk loops */

    // if (!OpenDiskLoops(&glob, par, UseGsrasOptions, lax)) {
    //     fprintf(stderr, "%s: unable to open site disk loops\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Build stream control list, if specified */

    // if (!BuildStreamControlList(par)) {
    //     fprintf(stderr, "%s: unable to build stream control list", myname);
    //     exit(MY_MOD_ID);
    // }

/* Initialize the local data processor */

    // if (par->source != SOURCE_ISI && !InitLocalProcessor(par)) {
    //     fprintf(stderr, "%s: uable to init local data processor\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Initialize metadata processor */

    // if (!InitMetaProcessor(par)) {
    //     fprintf(stderr, "%s: uable to init metadata processor\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Go into the background, if applicable */

    // if (daemon && !BackGround(par)) {
    //     perror("BackGround");
    //     exit(MY_MOD_ID);
    // }

/* Start logging facility */

    // if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    // if ((par->lp = InitLogging(myname, log, prefix(par), debug)) == NULL) {
    //     perror("InitLogging");
    //     exit(MY_MOD_ID);
    // }
    // for (i = 0; i < par->nsite; i++) {
    //     par->dl[i]->lp = par->lp;
    //     if (par->dl[i]->nrts != NULL) par->dl[i]->nrts->lp = par->lp;
    // }
    // LogMsg(LOG_INFO, "database = %s", glob.db->dbid);
    // if (flags & ISI_DL_FLAGS_IGNORE_LOCKS) LogMsg(LOG_INFO, "WARNING: no ISI disk loop locks");
    // if (UseGsrasOptions) LogMsg(LOG_INFO, "using GSRAS options for wfdisc management");
    // if (lax) LogMsg(LOG_INFO, "lax GPS status checks enabled");
    // LogMsg(LOG_INFO, "tee file length = %lu records", glob.trecs);
    // if (flags & ISI_DL_FLAGS_ASYNC_WRITE) LogMsg(LOG_INFO, "NOTICE: asynchronous ISI disk loop writes selected");

    /* set rest of tpar_create struct */

    /* utilPackUINT64((UINT8 *)cfg->tpc->q330id_serial, cfg->sn); */
    /* utilPackUINT32((UINT8 *)cfg->tpc->q330id_serial, cfg->sn_lo); */
    /* utilPackUINT32((UINT8 *)cfg->tpc->q330id_serial+1, cfg->sn_hi); */
    UINT32 snhi = cfg->sn >> 32;
    UINT32 snlo = cfg->sn & 0x00000000FFFFFFFF;
    /* utilSwapUINT32(&snlo, 1); */
    /* utilSwapUINT32(&snhi, 1); */
    cfg->tpc->q330id_serial[0] = snlo;
    cfg->tpc->q330id_serial[1] = snhi;
    cfg->tpc->q330id_dataport = cfg->dp;  // TODO: NEED TO CHECK: seems zero-based bnased on LT_TEL constants
    strncpy(cfg->tpc->q330id_station, cfg->stacode, 6);
    cfg->tpc->host_timezone = 0;         // using UTC on host systems
    strcpy(cfg->tpc->host_software, "IDA:isi330");         // host application name
    strcpy(cfg->tpc->opt_contfile, "");       // disable for now
    cfg->tpc->opt_verbose = VERB_SDUMP | VERB_RETRY | VERB_REGMSG | VERB_LOGEXTRA | VERB_AUXMSG | VERB_PACKET;
    cfg->tpc->opt_zoneadjust = 1;        // no need, on UTC
    cfg->tpc->opt_secfilter = OSF_DATASERV;         // not using 1-sec callback
    cfg->tpc->opt_client_msgs = 10;      // set to min, NOT SURE HOW THIS IS USED
    cfg->tpc->opt_minifilter = OMF_ALL;  // send all messages
    cfg->tpc->opt_aminifilter = 0;       // disabling, I think...
    cfg->tpc->opt_compat = 0;            // using flag bits in tokens
    cfg->tpc->amini_exponent = 12;        // not using 'archival' ms, but set to 512 byte records
    cfg->tpc->amini_512highest = 20;     // 40hz, but not relevant
    cfg->tpc->mini_embed = 1;            // embed calibration and event blockettes in miniseed
    cfg->tpc->mini_separate = 0;         // generate sSEPARATE mniniseed records for cal and event blockettes
    cfg->tpc->mini_firchain = NULL;      // just using built-in FIR filters
    cfg->tpc->call_minidata = isi330_miniseed_callback;      // NEED TO HAVE CALLBACK FOR DATA COLLECTION
    cfg->tpc->call_aminidata = NULL;     // not collecting archival miniseed
    cfg->tpc->resp_err = LIBERR_NOERR;
    cfg->tpc->call_state = isi330_state_callback;         // state change callback??? Will probably want this
    cfg->tpc->call_messages = isi330_msg_callback;      // message callback, will want this later
    cfg->tpc->call_secdata = NULL;       // not collecting 1-sec data;
    cfg->tpc->call_lowlatency = NULL;    // NYI
    cfg->tpc->call_baler = NULL;         // not using
    /* cfg->tpc->file_owner = NULL;         // until better understood. Used in libsupport */


    /* set registration struct */
    cfg->tpr.q330id_auth[0] = 0;
    cfg->tpr.q330id_auth[1] = 0;
    strcpy(cfg->tpr.q330id_address, cfg->q330host);
    cfg->tpr.q330id_baseport = 5330;
    cfg->tpr.host_mode = HOST_ETH;
    strcpy(cfg->tpr.host_interface, "");
    cfg->tpr.host_mincmdretry = 2;
    cfg->tpr.host_maxcmdretry = 30;
    cfg->tpr.host_ctrlport = 9999;
    cfg->tpr.host_dataport = 9998;
    // cfg->tpr.serial_flow = 0;
    // cfg->tpr.serial_baud = 9600;
    // cfg->tpr.serial_hostip = "";
    cfg->tpr.opt_latencytarget = 0;
    cfg->tpr.opt_closedloop = 0;
    cfg->tpr.opt_dynamic_ip = 0;
    cfg->tpr.opt_hibertime = 2;
    cfg->tpr.opt_conntime = 10;
    cfg->tpr.opt_connwait = 1;
    cfg->tpr.opt_regattempts = 3;
    cfg->tpr.opt_ipexpire = 0;
    cfg->tpr.opt_buflevel = 10;
    cfg->tpr.opt_q330_cont = 10;
    cfg->tpr.opt_dss_memory = 1024;

    LogMsg("initialization complete");

    return cfg;
}
