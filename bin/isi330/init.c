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
    fprintf(stderr, "usage: %s sitename q330=HostnameOrIP:DataPort dl=server:port \n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required:\n");
    fprintf(stderr, "    sitename               => Station code\n");
    fprintf(stderr, "    q330=name:port[:debug] => Quanterra Q330 input\n");
    fprintf(stderr, "    dl=server:port         => ISI Disk Loop server and port\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    cfg=<path>  => Location of q330.cfg configuration file\n");
    fprintf(stderr,"     net=netid   => set network code (default: 'II' \n");
    fprintf(stderr, "    log=name    => set log file name\n");
    fprintf(stderr, "    -bd         => run in the background\n");
    fprintf(stderr, "\n");
    exit(1);
}

ISI330_CONFIG *init(char *myname, int argc, char **argv)
{
    static char *fid = "init";
    static BOOL debug = FALSE;
    static char *log = NULL;
    static BOOL daemon = DEFAULT_DAEMON;
    ISI330_CONFIG *cfg;
    char *user = ISI330_DEFAULT_USER;
    char *cfgpath = NULL;
    int depth = DEFAULT_PACKET_QUEUE_DEPTH;
    MSEED_HANDLE *mshandle = NULL;
    Q330_CFG *q330cfg = NULL;  /* q330 database info */
    int errcode;


    if ((cfg = (ISI330_CONFIG *) malloc(sizeof(ISI330_CONFIG))) == NULL) {
        perror("malloc ISI330_CONFIG");
        exit(MY_MOD_ID + 1);
    }
    cfg->site = NULL;
    cfg->port = -1;
    memset(cfg->q330HostArgstr, 0, sizeof(cfg->q330HostArgstr));

/*  Get command line arguments  */
    int i;
    for (i = 1; i < argc; i++) {
        printf("arg %d of %d: %s\n", i, argc, argv[i]);

        if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {

            if (strlen(cfg->q330HostArgstr) > 0) {
                 fprintf(stderr, "ERROR: multilple instances of q330 argument are not allowed\n");
                 exit(MY_MOD_ID + 2);
            }
            strcpy(cfg->q330HostArgstr, argv[i] + strlen("q330="));

        } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {

             if (cfgpath != NULL) {
                 fprintf(stderr, "ERROR: multilple instances of cfg argument are not allowed\n");
                 exit(MY_MOD_ID + 3);
             }
             cfgpath = argv[i] + strlen("cfg=");

        } else if (strncasecmp(argv[i], "dl=", strlen("dl=")) == 0) {

            if (!utilParseServer(argv[i]+strlen("dl="), cfg->server, &cfg->port)) {
                fprintf(stderr, "error parsing dl argument: %s\n", strerror(errno));
                exit(MY_MOD_ID + 4);
            }

        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {

            strcpy(cfg->netname, argv[i] + strlen("net="));

        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {

            log = argv[i] + strlen("log=");

        } else if (strcmp(argv[i], "-bd") == 0) {

            daemon = TRUE;

        } else if (strcmp(argv[i], "-debug") == 0) {

            debug = TRUE;

        } else if (cfg->site == NULL) {  // this must be last

            if ((cfg->site = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
                exit(MY_MOD_ID + 5);
            }

        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

    /* Read ida q330 database */

    if ((q330cfg = q330ReadCfg(cfgpath, &errcode)) == NULL) {
        q330PrintErrcode(stderr, "q330ReadCfg: ", cfgpath, errcode);
        exit(MY_MOD_ID + 7);
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
         exit(MY_MOD_ID + 8);
     }

/* Start logging facility */

     if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
     if ((cfg->lp = InitLogging(myname, log, util_ucase(cfg->site), debug)) == NULL) {
         perror("InitLogging");
         exit(MY_MOD_ID + 9);
     }

    /* Initialize lib330 cfg structs */

    LoadQ330Host(cfg, q330cfg);

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(cfg);

    // dump config for review
    /* PrintISI330Config(cfg); */

    /* Missing network ID is OK, but user must specify station name and remote disk loop */

    if (cfg->netname == NULL) strncpy(cfg->netname, DEFAULT_NETID, ISI_NETLEN + 1);

    if (cfg->port < 0) {
        fprintf(stderr, "ERROR: missing or incomplete dl=server:port argument\n");
        help(argv[0]);
    }

    // start record pusher
    StartRecordPusher(cfg->server, cfg->port, cfg->lp, depth, cfg->site, cfg->netname);

    // start Q330 readers
    StartQ330Reader(cfg);

    LogMsg("%s: initialization complete", myname);

    return cfg;
}
