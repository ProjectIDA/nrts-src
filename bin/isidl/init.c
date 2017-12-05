#pragma ident "$Id: init.c,v 1.52 2016/02/12 21:56:27 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_INIT

static THREAD_FUNC WatchdogThread(void *argptr)
{
UINT32 maxdur;
THREAD_FUNC unreached;

    maxdur = *((UINT32 *) argptr);
    LogMsg(LOG_INFO, "Watchdog timer set to %lu seconds", maxdur);
    utilDelayMsec(maxdur * MSEC_PER_SEC);
    LogMsg(LOG_INFO, "Watchdog timer expired, initiating shutdown");
    Exit(MY_MOD_ID + 1);
    return unreached; /* to calm DARWIN builds */
}

static BOOL LaunchWatchdog(UINT32 maxdur)
{
static UINT32 value;
THREAD tid;
static char *fid = "LaunchWatchdog";

    value = maxdur;
    if (!THREAD_CREATE(&tid, WatchdogThread, (void *) &value)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: WatchdogThread: %s", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

static char *prefix(ISIDL_PAR *par)
{
char *src, *string;

    if (par->nsite == 1 || par->source != SOURCE_ISI) {
        src = par->site.local;
    } else {
        src = par->input;
    }

    if ((string = strdup(src)) == NULL) {
        perror("strdup");
        exit(MY_MOD_ID);
    } else {
        util_ucase(string);
    }

    return string;
}

static void SetGsrasDefaults(NRTS_DL *nrts)
{
    nrts->flags |= NRTS_DL_FLAGS_USE_BWD_DB;
    nrts->flags |= NRTS_DL_FLAGS_NO_BWD_FF;
    nrts->flags |= NRTS_DL_FLAGS_ALWAYS_TESTWD;
}

static BOOL OpenDiskLoops(ISI_GLOB *glob, ISIDL_PAR *par, BOOL UseGsrasOptions, BOOL lax)
{
#define ISIDL_MAX_NSITE       256
#define ISIDL_SITE_DELIMITERS "+,;/:- "
int i, errors;
char *site;
LNKLST *SiteList;
UINT32 options;
static char *fid = "OpenDiskLoops";

    options = (par->source == SOURCE_ISI) ? ISI_OPTION_NONE : ISI_OPTION_GENERATE_SEQNO;

    if ((SiteList = utilStringTokenList(par->site.local, "+", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }
    if (!listSetArrayView(SiteList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }
    if (SiteList->count < 1) {
        fprintf(stderr, "%s: illegal site list '%s'\n", fid, par->site.local);
        listDestroy(SiteList);
        return FALSE;
    }

    par->nsite = SiteList->count;
    if (par->source != SOURCE_ISI && par->nsite != 1) {
        fprintf(stderr, "illegal site string '%s' for non-ISI input (nsite == %d != 1)\n", par->site.local, par->nsite);
        listDestroy(SiteList);
        return FALSE;
    }

    if ((par->dl = (ISI_DL **) malloc(par->nsite * sizeof(ISI_DL *))) == NULL) {
        fprintf(stderr, "%s: malloc: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if ((par->first = (BOOL *) malloc(par->nsite * sizeof(BOOL))) == NULL) {
        fprintf(stderr, "%s: malloc: %s\n", fid, strerror(errno));
        return FALSE;
    }

    for (errors = 0, i = 0; i < par->nsite; i++) {
        site = (char *) SiteList->array[i];
        if ((par->dl[i] = isidlOpenDiskLoop(glob, site, par->lp, ISI_RDWR, options)) == NULL) {
            fprintf(stderr, "unable to open disk loop at %s\n", site);
            ++errors;
        } else {
            isidlLogDL(par->dl[i], LOG_DEBUG);
            if (par->dl[i]->sys->state != ISI_IDLE) {
                fprintf(stderr, "ERROR: disk loop is not idle!\n");
                ++errors;
            }
            if (par->dl[i]->nrts != NULL) {
                if (UseGsrasOptions) SetGsrasDefaults(par->dl[i]->nrts);
                if (lax) par->dl[i]->nrts->flags |= NRTS_DL_FLAGS_LAX_TIME;
            }
        }
        par->first[i] = TRUE;
    }
    return errors ? FALSE : TRUE;
}

ISIDL_PAR *init(char *myname, int argc, char **argv)
{
int i;
char *InputString = NULL, *site = NULL;
static char *dbspec = NULL;
static char *log = NULL;
ISIDL_PAR *par;
static ISI_GLOB glob;
static char *user = DEFAULT_USER;
static BOOL daemon = DEFAULT_DAEMON;
static BOOL debug = DEFAULT_DEBUG;
static BOOL lax = FALSE;
BOOL UseGsrasOptions = FALSE;
int depth = DEFAULT_QDEPTH;
char *GPSstring = NULL;
static char *seedlink = ISI_DL_DEFAULT_SLINK_CFGSTR;
struct {
    BOOL pkt;
    BOOL ttag;
    BOOL bwd;
    BOOL dl;
    BOOL lock;
} dbg = { FALSE, FALSE, FALSE, FALSE, FALSE };
UINT32 trecs = 0;
UINT16 flags = 0;
Q330 *q330;
char *cfgpath = NULL;
THREAD tid;
UINT32 maxdur = 0;
int slinkdebug = ISI_DL_DEFAULT_SLINK_DEBUG;

    if ((par = (ISIDL_PAR *) malloc(sizeof(ISIDL_PAR))) == NULL) {
        perror("malloc");
        exit(MY_MOD_ID);
    }

    if (!utilNetworkInit()) {
        perror("utilNetworkInit");
        exit(MY_MOD_ID);
    }

/*  Get command line arguments  */

    par->timeout = DEFAULT_TIMEOUT;
    par->site.local = par->site.remote = NULL;
    par->input = NULL;
    par->begstr = NULL;
    par->endstr = NULL;
    par->lp = NULL;
    par->tol.clock = NRTS_DEFAULT_CLOCK_TOLERANCE;
    par->tol.tic = NRTS_DEFAULT_TIC_TOLERANCE;
    par->dbgpath = DEFAULT_DBGPATH;
    par->source = SOURCE_UNDEFINED;
    par->sndbuf = DEFAULT_SNDBUF;
    par->rcvbuf = DEFAULT_RCVBUF;
    par->compress = DEFAULT_COMPRESS;
    par->flags = 0;
    par->scl.spec = NULL;
    par->scl.list = NULL;
    par->net = 0;
    par->netid = DEFAULT_NETID;
    par->rename.chan = NULL;
    listInit(&par->q330);
    cfgpath = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncmp(argv[i], "rechan:", strlen("rechan:")) == 0) {
            if (par->rename.chan != NULL) {
                fprintf(stderr, "%s: multiple `rechan' definitions are not permitted\n", myname);
                exit(MY_MOD_ID);
            }
            if ((par->rename.chan = ida10BuildChannelMap(argv[i]+strlen("rechan:"))) == NULL) {
                fprintf(stderr, "%s: ", myname);
                perror("ida10BuildChannelMap");
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "netid=", strlen("netid=")) == 0) {
            if ((par->netid = strdup(argv[i] + strlen("netid="))) == NULL) {
                fprintf(stderr, "%s: ", myname);
                perror("strdup");
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            if ((par->dbgpath = strdup(argv[i] + strlen("dbgpath="))) == NULL) {
                fprintf(stderr, "%s: ", myname);
                perror("strdup");
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
            if (cfgpath != NULL) {
                fprintf(stderr, "ERROR: multilple instances of cfg argument are not allowed\n");
                exit(MY_MOD_ID);
            }
            cfgpath = argv[i] + strlen("cfg=");
        } else if (strncmp(argv[i], "scl=", strlen("scl=")) == 0) {
            if (par->scl.spec != NULL) {
                fprintf(stderr, "ERROR: multilple instances of scl argument are not allowed\n");
                exit(MY_MOD_ID);
            }
            par->scl.spec = argv[i] + strlen("scl=");
        } else if (strcmp(argv[i], "-gsras") == 0) {
            UseGsrasOptions = TRUE;
        } else if (strcmp(argv[i], "-lax") == 0) {
            lax = TRUE;
        } else if (strcmp(argv[i], "-intercept") == 0) {
            par->flags |= ISIDL_FLAG_INTERCEPT;
        } else if (strcmp(argv[i], "-dbgpkt") == 0) {
            dbg.pkt = TRUE;
        } else if (strcmp(argv[i], "-dbgttag") == 0) {
            dbg.ttag = TRUE;
        } else if (strcmp(argv[i], "-dbgbwd") == 0) {
            dbg.bwd = TRUE;
        } else if (strcmp(argv[i], "-dbgdl") == 0) {
            dbg.dl = TRUE;
        } else if (strcmp(argv[i], "-dbglock") == 0) {
            dbg.lock = TRUE;
        } else if (strcmp(argv[i], "-timeops") == 0) {
            flags |= ISI_DL_FLAGS_TIME_DL_OPS;
        } else if (strcmp(argv[i], "-nosync") == 0) {
            flags |= ISI_DL_FLAGS_ASYNC_WRITE;
        } else if (strcmp(argv[i], "-nolock") == 0) {
            flags |= ISI_DL_FLAGS_IGNORE_LOCKS;
        } else if (strncmp(argv[i], "liss=", strlen("liss=")) == 0) {
            if (par->input != NULL) {
                fprintf(stderr, "%s: multiple primary sources not allowed", myname);
                help(myname);
            }
            par->input = argv[i] + strlen("liss=");
            par->source = SOURCE_LISS;
        } else if (strncmp(argv[i], "isi=", strlen("isi=")) == 0) {
            if (par->input != NULL) {
                fprintf(stderr, "%s: multiple primary sources not allowed", myname);
                help(myname);
            }
            par->input = argv[i] + strlen("isi=");
            par->source = SOURCE_ISI;
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            if ((par->begstr = strdup(argv[i] + strlen("beg="))) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            if ((par->endstr = strdup(argv[i] + strlen("end="))) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            if ((par->cfgpath = AddQ330(par, argv[i] + strlen("q330="), cfgpath)) == NULL) {
                fprintf(stderr, "%s: failed to add Q330: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
            seedlink = NULL; /* SeedLink not applicable for QDP input */
        } else if (strncmp(argv[i], "gps=", strlen("gps=")) == 0) {
            if ((GPSstring = strdup(argv[i] + strlen("gps="))) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "sndbuf=", strlen("sndbuf=")) == 0) {
            par->sndbuf = atoi(argv[i] + strlen("sndbuf="));
        } else if (strncmp(argv[i], "rcvbuf=", strlen("rcvbuf=")) == 0) {
            par->rcvbuf = atoi(argv[i] + strlen("rcvbuf="));
        } else if (strcmp(argv[i], "comp=none") == 0) {
            par->compress = ISI_COMP_NONE;
        } else if (strncmp(argv[i], "tictol=", strlen("tictol=")) == 0) {
            par->tol.tic = (UINT32) atoi(argv[i] + strlen("tictol="));
        } else if (strncmp(argv[i], "clocktol=", strlen("clocktol=")) == 0) {
            par->tol.clock = (UINT32) atoi(argv[i] + strlen("clocktol="));
        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            par->timeout = atoi(argv[i] + strlen("to="));
        } else if (strncmp(argv[i], "trecs=", strlen("trecs=")) == 0) {
            trecs = atoi(argv[i] + strlen("trecs="));
        } else if (strncmp(argv[i], "trec=", strlen("trec=")) == 0) {
            trecs = atoi(argv[i] + strlen("trec="));
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            if (par->net != 0) {
                fprintf(stderr, "%s: multiple `net' servers not allowed", myname);
                help(myname);
            }
            par->net = atoi(argv[i] + strlen("net="));
            if (par->net < 1) {
                fprintf(stderr, "%s: illegal parameter `%s'", myname, argv[i]);
                help(myname);
            }
        } else if (strncmp(argv[i], "seedlink=", strlen("seedlink=")) == 0) {
            seedlink = argv[i] + strlen("seedlink=");
        } else if (strcmp(argv[i], "-noseedlink") == 0) {
            seedlink = NULL;
        } else if (strncmp(argv[i], "slinkdebug=", strlen("slinkdebug=")) == 0) {
            slinkdebug = atoi(argv[i] + strlen("slinkdebug="));
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strncmp(argv[i], "maxdur=", strlen("maxdur=")) == 0) {
            maxdur = atoi(argv[i] + strlen("maxdur="));
        } else if (par->site.local == NULL) {
            if ((par->site.local = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

/* Initialize intercept processor buffer, if applicable */

    if ((par->flags & ISIDL_FLAG_INTERCEPT) && par->source != SOURCE_ISI) {
        fprintf(stderr, "intercept processor only available for ISI input sources\n");
        help(myname);
    } else if (!InitInterceptBuffer(&par->ib)) {
        fprintf(stderr, "%s: InitInterceptBuffer: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }

/* Verify LISS parameters, if applicable */

    if (par->source == SOURCE_LISS && !ParseLissArgs(par->input)) {
        fprintf(stderr, "illegal LISS parameter list\n");
        help(myname);
    }

/* Q330s can only be acquired with each other (and barometers) */

    if (!listSetArrayView(&par->q330)) {
        fprintf(stderr, "%s: listSetArrayView: %s", myname, strerror(errno));
        exit(MY_MOD_ID);
    }

    if (par->q330.count > 0) {
        if (par->source != SOURCE_UNDEFINED) {
            fprintf(stderr, "%s: multiple primary sources not allowed", myname);
            help(myname);
        } else {
            par->source = SOURCE_Q330;
        }
    }

    if (par->source == SOURCE_UNDEFINED && par->net == 0) {
        fprintf(stderr,"%s: missing input specifier\n", myname);
        help(myname);
    }

/* Must specify site name */

    if (par->site.local == NULL) {
        fprintf(stderr,"%s: missing site code(s)\n", myname);
        help(myname);
    }

/* Load the database */

    par->timeout *= MSEC_PER_SEC; /* IACP wants timeout in msec */

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }
    if (trecs != 0) glob.trecs = trecs;

    if (dbg.pkt) glob.debug.pkt = LOG_INFO;
    if (dbg.ttag) glob.debug.ttag = LOG_INFO;
    if (dbg.bwd) glob.debug.bwd = LOG_INFO;
    if (dbg.dl) glob.debug.dl = LOG_INFO;
    if (dbg.lock) glob.debug.lock = LOG_INFO;
    if (flags != 0) glob.flags |= flags;

    utilSetIdentity(user);

/* Connect to the disk loops */

    if (!OpenDiskLoops(&glob, par, UseGsrasOptions, lax)) {
        fprintf(stderr, "%s: unable to open site disk loops\n", myname);
        exit(MY_MOD_ID);
    }

/* Build stream control list, if specified */

    if (!BuildStreamControlList(par)) {
        fprintf(stderr, "%s: unable to build stream control list", myname);
        exit(MY_MOD_ID);
    }

/* Initialize the local data processor */

    if (par->source != SOURCE_ISI && !InitLocalProcessor(par)) {
        fprintf(stderr, "%s: uable to init local data processor\n", myname);
        exit(MY_MOD_ID);
    }

/* Initialize metadata processor */

    if (!InitMetaProcessor(par)) {
        fprintf(stderr, "%s: uable to init metadata processor\n", myname);
        exit(MY_MOD_ID);
    }

/* Go into the background, if applicable */

    if (daemon && !BackGround(par)) {
        perror("BackGround");
        exit(MY_MOD_ID);
    }

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((par->lp = InitLogging(myname, log, prefix(par), debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID);
    }
    for (i = 0; i < par->nsite; i++) {
        par->dl[i]->lp = par->lp;
        if (par->dl[i]->nrts != NULL) par->dl[i]->nrts->lp = par->lp;
    }
    LogMsg(LOG_INFO, "database = %s", glob.db->dbid);
    if (flags & ISI_DL_FLAGS_IGNORE_LOCKS) LogMsg(LOG_INFO, "WARNING: no ISI disk loop locks");
    if (UseGsrasOptions) LogMsg(LOG_INFO, "using GSRAS options for wfdisc management");
    if (lax) LogMsg(LOG_INFO, "lax GPS status checks enabled");
    LogMsg(LOG_INFO, "tee file length = %lu records", glob.trecs);
    if (flags & ISI_DL_FLAGS_ASYNC_WRITE) LogMsg(LOG_INFO, "NOTICE: asynchronous ISI disk loop writes selected");

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(par);

/* Configure SeedLink feeder, now that we have log pointer set */

    if (seedlink != NULL && !isidlSetSeedLinkOption(par->dl[0], seedlink, argv[0], slinkdebug)) Exit(MY_MOD_ID);
    isidlLogSeedLinkOption(par->lp, LOG_INFO, par->dl[0]);

/* Launch packet server, if configured */

    if (par->net != 0 && !isidlPacketServer(par->dl[0], par->net, par->lp, WritePacketToDisk)) {
        LogMsg(LOG_ERR, "init: isidlPacketServer: %s", strerror(errno));
        Exit(MY_MOD_ID);
    }

/* Launch appropriate station backend, if applicable */

    switch (par->source) {
      case SOURCE_ISI:
        StartIsiReader(par);
        break;
      case SOURCE_LISS:
        StartLissReader(par);
        break;
      case SOURCE_Q330:
        StartQ330Readers(par);
        break;
    }

    LogMsg(LOG_DEBUG, "initialization complete");

/* Launch the watchdog timer, if selected */

    if (maxdur != 0 && !LaunchWatchdog(maxdur)) Exit(MY_MOD_ID + 2);

    return par;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.52  2016/02/12 21:56:27  dechavez
 * removed old code related to barometer, ars, and pre-IDA10 packet support
 *
 * Revision 1.51  2015/12/09 18:34:54  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.50  2014/08/28 23:16:43  dechavez
 * fixed ISI source sequence number bug introduced in 1.49
 *
 * Revision 1.49  2014/08/28 22:12:57  dechavez
 * removed support for deprecated options, reworked to support move of ISI_DL_OPTIONS
 * into ISI_DL handled, removed INCLUDE_Q330 conditionals
 *
 * Revision 1.48  2014/08/19 18:13:18  dechavez
 * added slinkdebug option
 *
 * Revision 1.47  2013/03/08 18:42:28  dechavez
 * use new (libisidl 1.22.0) isidlSetSeedLinkOption() and isidlLogSeedLinkOption() args
 *
 * Revision 1.46  2013/01/23 23:36:51  dechavez
 * include LOGIO in call to isidlLogSeedLinkOption()
 *
 * Revision 1.45  2013/01/17 17:47:26  dechavez
 * disable seedlink option for QDP input
 *
 * Revision 1.44  2013/01/16 23:03:40  dechavez
 * Enabled SeedLink support by default, added -noseedlink option to suppress
 * BUD option removed
 *
 * Revision 1.43  2013/01/14 22:39:16  dechavez
 * SeedLink support introduced
 *
 * Revision 1.42  2012/12/13 21:53:56  dechavez
 * added support for changing station and channel names
 *
 * Revision 1.41  2012/07/30 16:58:51  dechavez
 * added -nosync option for ISI disk loop writes without O_SYNC (libisidl 1.19.1)
 *
 * Revision 1.40  2011/12/26 20:55:59  dechavez
 * added maxdur watchdog timer
 *
 * Revision 1.39  2011/11/07 17:49:20  dechavez
 * accomodated move of mseed to ISI_DL_OPTIONS
 *
 * Revision 1.38  2011/10/19 16:21:07  dechavez
 * changed static 91011bug to variable daybug to accomodate new behavior in these crazy clocks
 *
 * Revision 1.37  2011/10/13 18:12:01  dechavez
 * 91011bug support, launch isidlPacketServer() if net option selected
 *
 * Revision 1.36  2011/08/04 20:53:15  dechavez
 * added support for network push
 *
 * Revision 1.35  2011/04/25 23:18:43  dechavez
 * added comp=none argument, and error exit on unrecognized argument
 *
 * Revision 1.34  2011/01/25 22:44:58  dechavez
 * fixed typo
 *
 * Revision 1.33  2011/01/25 21:31:04  dechavez
 * added trec override option
 *
 * Revision 1.32  2010/04/01 22:04:50  dechavez
 * added StartIsiReader() and changed StartQ330Reader() to StartQ330Readers() (plural)
 * and have it launch all the individual digitizer threads
 *
 * Revision 1.31  2008/03/05 23:26:22  dechavez
 * added intercept processing and ARS leap year bug check support
 *
 * Revision 1.30  2008/01/25 22:04:38  dechavez
 * added support for scl (stream control list)
 *
 * Revision 1.29  2007/09/07 20:04:41  dechavez
 * use cfg file for Q330 address parameters
 *
 * Revision 1.28  2007/06/23 15:53:07  dechavez
 * added -lax option to set NRTS_DL_FLAGS_LAX_TIME
 *
 * Revision 1.27  2007/05/03 20:27:58  dechavez
 * added LISS support
 *
 * Revision 1.26  2007/04/18 23:21:52  dechavez
 * Added -gsras option to set NRTS DL flags for default wfdisc managment in Obninsk
 *
 * Revision 1.25  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.24  2007/02/20 02:42:58  dechavez
 * Set log tag for single station isi sites to site instead of server
 *
 * Revision 1.23  2007/02/08 22:54:44  dechavez
 * LOG_ERR, LOG_WARN to LOG_INFO
 *
 * Revision 1.22  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.21  2006/12/12 23:29:05  dechavez
 * init new metadata callback support
 *
 * Revision 1.20  2006/11/13 23:42:11  dechavez
 * added various -dbgxxx options, updated disk loop log pointers after log init
 * so that these options will actually take effect
 *
 * Revision 1.19  2006/09/29 19:51:43  dechavez
 * added support for socket send/receive buffers, with 64Kbyte default
 *
 * Revision 1.18  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.17  2006/06/23 18:30:39  dechavez
 * pause between successive Q330 launches
 *
 * Revision 1.16  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.15  2006/06/02 21:04:14  dechavez
 * q330 and metadata support added
 *
 * Revision 1.14  2006/04/20 23:03:25  dechavez
 * replace generic utilBackGround() with a local BackGround() that handles
 * lock (process) id transition from parent to child
 *
 * Revision 1.13  2006/03/30 22:05:23  dechavez
 * barometer support
 *
 * Revision 1.12  2006/03/13 23:08:52  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.11  2006/02/14 17:05:01  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.10  2006/02/09 22:40:48  dechavez
 * log database being used
 *
 * Revision 1.9  2005/09/30 22:34:53  dechavez
 * Fixed msec vs sec confusion in specifying timeout (user specifies seconds)
 *
 * Revision 1.8  2005/09/13 00:37:57  dechavez
 * added dbgpath support
 *
 * Revision 1.7  2005/09/10 22:07:49  dechavez
 * set default log spec depending of daemon state
 *
 * Revision 1.6  2005/07/26 18:37:15  dechavez
 * prefix log messages with site or source tag
 *
 * Revision 1.5  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
