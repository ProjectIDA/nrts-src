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
    cfg->site = NULL;

/*  Get command line arguments  */

     listInit(&cfg->q330list);

    for (int i = 1; i < argc; i++) {
        printf("arg %d of %d: %s\n", i, argc, argv[i]);

        if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            if ((cfg->cfgpath = AddQ330(cfg, argv[i] + strlen("q330="), cfgpath)) == NULL) {
                fprintf(stderr, "%s: failed to add Q330: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
             if (cfgpath != NULL) {
                 fprintf(stderr, "ERROR: multilple instances of cfg argument are not allowed\n");
                 exit(MY_MOD_ID);
             }
             cfgpath = argv[i] + strlen("cfg=");
        } else if (cfg->site == NULL) {
            if ((cfg->site = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

/* make list of Q330s */

     if (!listSetArrayView(&cfg->q330list)) {
         fprintf(stderr, "%s: listSetArrayView: %s", myname, strerror(errno));
         exit(MY_MOD_ID);
     }

/* Must specify site name */

    if (cfg->site == NULL) {
        fprintf(stderr,"%s: missing station code(s)\n", myname);
        help(myname);
    }


     utilSetIdentity(user);

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

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(cfg);

    // dump config for review
    PrintISI330Config(cfg);

    // start Q330 readers
    StartQ330Readers(cfg);


    LogMsg("Initialization complete");

    return cfg;
}
