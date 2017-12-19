/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_INIT

void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s sitename q330=HostnameOrIP:DataPort[:debug] [q330=HostnameOrIP:DataPort[:debug]] \n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required:\n");
    fprintf(stderr, "    sitename               => Station code\n");
    fprintf(stderr, "    q330=name:port[:debug] => Quanterra Q330 input (may be repeated)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "       cfg=???? => ????????????????????????\n");
    fprintf(stderr, "        db=spec => set database to `spec'\n");
//    fprintf(stderr, "       trec=int => override records/tee file parameter\n");
//    fprintf(stderr, "        to=secs => set I/O timeout interval\n");
    fprintf(stderr, "       log=name => set log file name\n");
//    fprintf(stderr, "    maxdur=secs => force exit after 'secs' seconds\n");
//    fprintf(stderr, "seedlink=cfgstr => set SeedLink configuration (where cfgstr is srv:port:len:depth:net)\n");
//    fprintf(stderr, "    -noseedlink => disable SeedLink support\n");
//    fprintf(stderr, " slinkdebug=int => set SeedLink debug level\n");
    fprintf(stderr, "            -bd => run in the background\n");
    fprintf(stderr, "\n");
    exit(1);
}

static char *prefix(ISI330_CONFIG *cfg)
{
    char *src, *string;

    src = cfg->site;

    if ((string = strdup(src)) == NULL) {
        perror("strdup");
        exit(MY_MOD_ID);
    } else {
        util_ucase(string);
    }

    return string;
}



ISI330_CONFIG *init(char *myname, int argc, char **argv)
{
    static BOOL debug = FALSE;
    static char *dbspec = NULL;
    static char *log = NULL;
    static BOOL daemon = DEFAULT_DAEMON;
    ISI330_CONFIG *cfg;
    char *user = ISI330_DEFAULT_USER;
    char *cfgpath = NULL;
//    char *station = NULL;
//    char *q330host = NULL;
    struct {
        BOOL pkt;
        BOOL ttag;
        BOOL bwd;
        BOOL dl;
        BOOL lock;
    } dbg = { FALSE, FALSE, FALSE, FALSE, FALSE };


    if ((cfg = (ISI330_CONFIG *) malloc(sizeof(ISI330_CONFIG))) == NULL) {
        perror("malloc ISI330_CONFIG");
        exit(MY_MOD_ID + 1);
    }

/*  Get command line arguments  */

     listInit(&cfg->q330list);

    for (int i = 1; i < argc; i++) {
printf("argv[%d] = %s\n", i, argv[i]);
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            if ((cfg->cfgpath = AddQ330(cfg, argv[i] + strlen("q330="), cfgpath)) == NULL) {
                fprintf(stderr, "%s: failed to add Q330: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
// TODO         seedlink = NULL; /* SeedLink not applicable for QDP input */
        } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
             if (cfgpath != NULL) {
                 fprintf(stderr, "ERROR: multilple instances of cfg argument are not allowed\n");
                 exit(MY_MOD_ID);
             }
             cfgpath = argv[i] + strlen("cfg=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
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
        } else if (cfg->site == NULL) {
            if ((cfg->site = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

/* Q330s can only be acquired with each other (and barometers) */

     if (!listSetArrayView(&cfg->q330list)) {
         fprintf(stderr, "%s: listSetArrayView: %s", myname, strerror(errno));
         exit(MY_MOD_ID);
     }

/* Must specify site name */

    if (cfg->site == NULL) {
        fprintf(stderr,"%s: missing station code(s)\n", myname);
        help(myname);
    }

/* Load the database */
// TODO
//     cfg->timeout *= MSEC_PER_SEC; /* IACP wants timeout in msec */
//
//     if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
//         fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", myname, strerror(errno));
//         exit(MY_MOD_ID);
//     }
//     if (trecs != 0) glob.trecs = trecs;
//
//     if (dbg.pkt) glob.debug.pkt = LOG_INFO;
//     if (dbg.ttag) glob.debug.ttag = LOG_INFO;
//     if (dbg.bwd) glob.debug.bwd = LOG_INFO;
//     if (dbg.dl) glob.debug.dl = LOG_INFO;
//     if (dbg.lock) glob.debug.lock = LOG_INFO;
//     if (flags != 0) glob.flags |= flags;

     utilSetIdentity(user);

/* Connect to the disk loops */
// TODO
    // if (!OpenDiskLoops(&glob, par, UseGsrasOptions, lax)) {
    //     fprintf(stderr, "%s: unable to open site disk loops\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Build stream control list, if specified */
// TODO
    // if (!BuildStreamControlList(par)) {
    //     fprintf(stderr, "%s: unable to build stream control list", myname);
    //     exit(MY_MOD_ID);
    // }

/* Initialize the local data processor */
// TODO
    // if (par->source != SOURCE_ISI && !InitLocalProcessor(par)) {
    //     fprintf(stderr, "%s: uable to init local data processor\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Initialize metadata processor */
// TODO
    // if (!InitMetaProcessor(par)) {
    //     fprintf(stderr, "%s: uable to init metadata processor\n", myname);
    //     exit(MY_MOD_ID);
    // }

/* Go into the background, if applicable */

     if (daemon && !BackGround(cfg)) {
         perror("BackGround");
         exit(MY_MOD_ID);
     }

/* Start logging facility */

     if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
     if ((cfg->lp = InitLogging(myname, log, prefix(cfg), debug)) == NULL) {
         perror("InitLogging");
         exit(MY_MOD_ID);
     }
     // TODO
     /* cfg->dl->lp = cfg->lp; */
    /* if (cfg->dl->nrts != NULL) cfg->dl->nrts->lp = cfg->lp; */

// TODO
//     LogMsg(LOG_INFO, "database = %s", glob.db->dbid);
//     if (flags & ISI_DL_FLAGS_IGNORE_LOCKS) LogMsg(LOG_INFO, "WARNING: no ISI disk loop locks");
//     if (UseGsrasOptions) LogMsg(LOG_INFO, "using GSRAS options for wfdisc management");
//     if (lax) LogMsg(LOG_INFO, "lax GPS status checks enabled");
//     LogMsg(LOG_INFO, "tee file length = %lu records", glob.trecs);
//     if (flags & ISI_DL_FLAGS_ASYNC_WRITE) LogMsg(LOG_INFO, "NOTICE: asynchronous ISI disk loop writes selected");

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(cfg);

/* Configure SeedLink feeder, now that we have log pointer set */
// TODO
//    if (seedlink != NULL && !isidlSetSeedLinkOption(par->dl[0], seedlink, argv[0], slinkdebug)) Exit(MY_MOD_ID);
//    isidlLogSeedLinkOption(par->lp, LOG_INFO, par->dl[0]);

/* Launch packet server, if configured */

//    if (par->net != 0 && !isidlPacketServer(par->dl[0], par->net, par->lp, WritePacketToDisk)) {
//        LogMsg(LOG_ERR, "init: isidlPacketServer: %s", strerror(errno));
//        Exit(MY_MOD_ID);
//    }

    // dump config for review
    PrintISI330Config(cfg);

    // start Q330 readers
    StartQ330Readers(cfg);


    LogMsg("initialization complete");

    return cfg;
}
