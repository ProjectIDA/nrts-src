#pragma ident "$Id: main.c,v 1.3 2012/10/31 17:25:35 dechavez Exp $"
/*======================================================================
 *
 *  Read data from one or more Paroscientific barometers and push IDA10.8
 *  to a remote ISI disk loop.
 *
 *  Uses host clock for timing.
 *
 *====================================================================*/
#include "paro10.h"

#define MY_MOD_ID PARO10_MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s sname [ options ] Device:Speed:Chan:Sint [ Device:Speed:Chan:Sint ... ] dl=server:port [ options ]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"net=netid => set network code\n");
    fprintf(stderr,"log=name  => set log file name\n");
    fprintf(stderr,"-bd       => run in the background\n");
    fprintf(stderr,"\n");
    exit(1);
}

static void StartReadAndPacketThreads(BAROMETER *bp)
{
THREAD tid;
static char *fid = "StartReadAndPacketThreads";

/* Launch the barometer reader */

    if (!THREAD_CREATE(&tid, ReadThread, (void *) bp)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: ReadThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 6);
    }
    THREAD_DETACH(tid);

/* Launch the packet builder thread */

    if (!THREAD_CREATE(&tid, PacketThread, (void *) bp)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: PacketThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
    THREAD_DETACH(tid);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
char *myname, **argv;
int argc;
int i;
char *dbgpath = DEFAULT_DBGPATH;
BOOL daemon = DEFAULT_DAEMON;
LOGIO *lp = NULL;
int depth = DEFAULT_PACKET_QUEUE_DEPTH;
BOOL debug = DEFAULT_DEBUG;
LNKLST list;
LNKLST_NODE *crnt;
BAROMETER *bp;
char server[MAXPATHLEN+1];
int port = -1;
char *sname = NULL, *nname = NULL, *log = NULL, *SITE, *user = DEFAULT_USER;
static char *default_nname = DEFAULT_NETID;

    myname = cmdline->myname;
    argc   = cmdline->argc;
    argv   = cmdline->argv;

    listInit(&list);

    if (argc < 2) help(myname);

    sname = argv[1];
    if (strncmp(sname, "sta=", strlen("sta=")) == 0) sname = sname + strlen("sta=");

/*  Get command line arguments  */

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            nname = argv[i] + strlen("net=");
        } else if (strncasecmp(argv[i], "dl=", strlen("dl=")) == 0) {
            if (!utilParseServer(argv[i]+strlen("dl="), server, &port)) {
                fprintf(stderr, "error parsing dl argument: %s\n", strerror(errno));
                exit(1);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            if ((dbgpath = strdup(argv[i] + strlen("dbgpath="))) == NULL) {
                fprintf(stderr, "%s: ", myname);
                perror("strdup");
                exit(MY_MOD_ID);
            }
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (!AddBarometer(&list, argv[i], sname)) {
            fprintf(stderr, "%s: failed to add barometer: %s\n", myname, strerror(errno));
            exit(MY_MOD_ID);
        }
    }

    if (list.count == 0) {
        fprintf(stderr,"%s: must specify at least one barometer\n", myname);
        help(myname);
    }

/* Missing network ID is OK, but user must specify station name and remote disk loop */

    if (sname == NULL) {
        fprintf(stderr,"%s: missing station name (first arg)\n", myname);
        help(myname);
    }

    if (nname == NULL) nname = default_nname;

    if (port < 0) {
        fprintf(stderr, "ERROR: missing or incomplete dl=server:port argument\n");
        help(argv[0]);
    }

    if ((SITE = strdup(sname)) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }
    util_ucase(SITE);

/* Run as the specified user */

    utilSetIdentity(user);

/* Go into the background, if applicable */

    if (daemon && !utilBackGround()) {
        perror("utilBackGround");
        exit(MY_MOD_ID);
    }

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(myname, log, SITE, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID);
    }

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit();

/* Start the packet pusher */

    StartPacketPusher(server, port, lp, depth, sname, nname);

/* Launch a pair of read and packetizer threads for each barometer */

    crnt = listFirstNode(&list);
    while (crnt != NULL) {
        bp = (BAROMETER *) crnt->payload;
        bp->lp = lp;
        bp->debug = debug;
        ida10SintToFactMult((REAL64) bp->MsecSint / (REAL64) MSEC_PER_SEC, &bp->srfact, &bp->srmult);
        strlcpy(bp->sname, sname, IDA105_SNAME_LEN+1);
        strlcpy(bp->nname, nname, IDA105_NNAME_LEN+1);
        StartReadAndPacketThreads((BAROMETER *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    LogMsg(LOG_DEBUG, "initialization complete");

/* Never return */

#ifndef WIN32
    while (1) pause();  /* all the action happens in the read threads */
#endif /* !WIN32 */
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
VOID main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2012/10/31 17:25:35  dechavez
 * strip off leading "sta=" if user specified station name with a prefix
 *
 * Revision 1.2  2012/10/25 22:15:37  dechavez
 * initial release
 *
 */
