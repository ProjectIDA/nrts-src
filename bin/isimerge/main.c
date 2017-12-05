#pragma ident "$Id: main.c,v 1.3 2010/09/17 20:04:22 dechavez Exp $"
/*======================================================================
 *
 *  Read data from multiple ISI disk loops and merge into one
 *  Requires that the maximum packet size on the remote disk loops be
 *  equal to (or less than) that of the merged disk loop.
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_MAIN

static SEMAPHORE semaphore;
static MUTEX mutex;

static void InitGlobalMutex()
{
    MUTEX_INIT(&mutex);
} 

void LockGlobalMutex()
{
    MUTEX_LOCK(&mutex);
}

void ReleaseGlobalMutex()
{
    MUTEX_UNLOCK(&mutex);
}

void WakeMainThread()
{
    SEM_POST(&semaphore);
}

static void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s remote@server[:arg=value...] site [ options ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Command line options\n");
    fprintf(stderr,"db=spec  => set database to `spec'\n");
    fprintf(stderr,"log=name => set log file name\n");
    fprintf(stderr,"-bd      => run in the background\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The remote@server specifier may be repeated any number of times. The required items are\n");
    fprintf(stderr, "remote  => remote site name\n");
    fprintf(stderr, "server  => remote server name or dot decimal IP address\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The following colon delimited, connection specific, options are supported:\n");
    fprintf(stderr, ":port=value => port number (default = %d)\n", ISI_DEFAULT_PORT);
    fprintf(stderr, ":beg=seqno  => begin seqno (or 'oldest' or 'newest')\n");
    fprintf(stderr, ":end=seqno  => end seqno (or 'newest' or 'never')\n");
    fprintf(stderr, ":to=secs    => set read timeout (default=%d)\n", IACP_DEF_AT_TIMEO / MSEC_PER_SEC);
    fprintf(stderr, "\n");

    exit(MY_MOD_ID + 1);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
int i;
LOGIO *lp;
ISI_GLOB glob;
LNKLST *head;
INT32 status;
LNKLST_NODE *crnt;
char *log = NULL;
char *name = NULL;
char *user = DEFAULT_USER;
char *dbspec = NULL;
BOOL debug = DEFAULT_DEBUG;
BOOL daemon = DEFAULT_DAEMON;
SITE_PAR *site;
static char *fid = "MainThread";

    InitGlobalMutex();
    InitDataThreadMutex();

/* This semaphore gets set when going down due to signals */

    SEM_INIT(&semaphore, 0, 1);

/* Start network plumbing for those systems that need it (ie, Windows) */

    if (!utilNetworkInit()) {
        perror("utilNetworkInit");
        exit(MY_MOD_ID + 2);
    }

/* Initialize the linked list to hold all the remote sites */

    if ((head = listCreate()) == NULL) {
        perror("listCreate");
        exit(MY_MOD_ID + 3);
    }

/* Scan the command line */

    for (i = 1; i < cmdline->argc; i++) {
        if (strcmp(cmdline->argv[i], "-h") == 0) {
            help(cmdline->myname);
        } else if (strcmp(cmdline->argv[i], "-help") == 0) {
            help(cmdline->myname);
        } else if (strcmp(cmdline->argv[i], "--help") == 0) {
            help(cmdline->myname);
        } else if (strcmp(cmdline->argv[i], "--version") == 0) {
            printf("%s %s\n", cmdline->myname, VersionIdentString);
            exit(0);
        } else if (strncmp(cmdline->argv[i], "db=", strlen("db=")) == 0) {
            dbspec = cmdline->argv[i] + strlen("db=");
        } else if (strncmp(cmdline->argv[i], "isi=", strlen("isi=")) == 0) {
            if (!QueueSiteThread(head, cmdline->argv[i] + strlen("isi="))) exit(MY_MOD_ID + 4);
        } else if (strncmp(cmdline->argv[i], "user=", strlen("user=")) == 0) {
            user = cmdline->argv[i] + strlen("user=");
        } else if (strncmp(cmdline->argv[i], "log=", strlen("log=")) == 0) {
            log = cmdline->argv[i] + strlen("log=");
        } else if (strcmp(cmdline->argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(cmdline->argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (!QueueSiteThread(head, cmdline->argv[i])) {
            if (name == NULL) {
                if ((name = strdup(cmdline->argv[i])) == NULL) {
                    perror("strdup");
                    exit(MY_MOD_ID + 5);
                }
            } else {
                fprintf(stderr, "%s: unrecognized argument '%s'\n", cmdline->myname, cmdline->argv[i]);
                help(cmdline->myname);
            }
        }
    }

/* Ensure the minimum input has been given */

    if (name == NULL) {
        fprintf(stderr, "%s: missing site name\n", cmdline->myname);
        help(cmdline->myname);
    }

    if (head->count < MINIMUM_COUNT) {
        fprintf(stderr, "%s: a minimum of %d remote sites must be specified", cmdline->myname, MINIMUM_COUNT);
        help(cmdline->myname);
    }

/* Start the logging facility */

    //if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if (log == NULL) log = DEFAULT_BACKGROUND_LOG;
    if (!InitLogging(cmdline->myname, log, name, debug)) {
        fprintf(stderr, "*** FATAL ERROR *** %s: InitLogging failed\n", cmdline->myname);
        exit(MY_MOD_ID + 6);
    }

/* Load the database */

    if (!isidlSetGlobalParameters(dbspec, cmdline->myname, &glob)) {
        LogMsg(LOG_ERR, "*** FATAL ERROR *** %s: isidlSetGlobalParameters: %s", cmdline->myname, strerror(errno));
        exit(MY_MOD_ID + 7);
    }

/* Change user in the event we are running as root (ie, at boot time) */

    utilSetIdentity(user);

    StartSignalHandler();

/* Set up exit and signal handlers */

    InitExitHandler();

/* Open output disk loop and initialize the packet writer */

    if (!OpenDiskLoop(&glob, name, LogHandle())) {
        LogMsg(LOG_ERR, "*** FATAL ERROR *** %s: InitSavePacket failed", cmdline->myname);
        exit(MY_MOD_ID + 8);
    }

/* Update the data threads with disk loop derived parameters */

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        if (!UpdateSitePar((SITE_PAR *) crnt->payload)) {
            LogMsg(LOG_ERR, "*** FATAL ERROR *** %s: UpdateSitePar failed", cmdline->myname);
            CloseDiskLoop();
            exit(MY_MOD_ID + 9);
        }
        crnt = listNextNode(crnt);
    }

/* Go into the background, if applicable */

    if (daemon) {
        if (!BackGround()) {
            LogMsg(LOG_ERR, "*** FATAL ERROR *** %s: BackGround: %s\n", cmdline->myname, strerror(errno));
            exit(MY_MOD_ID + 10);
        } else {
            LogMsg(LOG_INFO, "running as daemon");
        }
    } else {
        LogMsg(LOG_INFO, "running as foreground process");
    }

/* Launch the data threads */

    LogMsg(LOG_INFO, "launching data threads");
    crnt = listFirstNode(head);
    while (crnt != NULL) {
        site = (SITE_PAR *) crnt->payload;
        SEM_POST(&site->semaphore);
        crnt = listNextNode(crnt);
    }

/* Wait for the program to end (either via signal or error in one of the data threads) */

    SEM_WAIT(&semaphore);
    if ((status = ExitStatus()) < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - ISIMERGE_MOD_SIGNALS);
    }
    CloseDiskLoop();
    LogMsg(LOG_INFO, "exit %ld", status);
    exit(status);
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
void main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2010/09/17 20:04:22  dechavez
 * reordered stuff in failed attempt to figure out why the threads hang when run as a daemon,
 * changed site@server specification to eliminate the isi= prefix
 *
 * Revision 1.2  2010/09/10 23:34:44  dechavez
 * set default log as appropriate for foreground/background
 *
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
