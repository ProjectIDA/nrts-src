#pragma ident "$Id: init.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "wgmcd.h"

#define MY_MOD_ID WGMCD_MOD_INIT

IACP *Init(char *myname, PARAM *par)
{
IACP *server;
struct rlimit rlimit;
static char *fid = "Init";

/* (attempt to) set our identity */

    utilSetIdentity(par->user);

/* Set the resource limits for number of open files to the max */

#ifdef HAVE_RLIMIT
    if (getrlimit(RLIMIT_NOFILE, &rlimit) != 0) {
        perror("getrlimit");
        exit(1);
    }
    rlimit.rlim_cur = rlimit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rlimit) != 0) {
        perror("setrlimit");
        exit(1);
    }
#endif /* HAVE_RLIMIT */

/* Start up networking, if applicable */

    if (!utilNetworkInit()) {
        perror("utilNetworkInit");
        exit(1);
    }

/* Go into the background, if applicable (par->daemon always FALSE for WIN32) */

    if (par->daemon && !utilBackGround()) {
        perror("utilBackGround");
        exit(1);
    }

/* Initialize the exit facility */

    InitGracefulExit();

/* Start logging facility */

    if (!InitLogging(myname, par)) {
        perror("InitLogging");
        exit(1);
    }
    LogPar(par);

/* Start signal handling thread */

    StartSignalHandler();

/* Intialize the client list */

    InitClientList(par);

/* Start up echo server, if specified */

    if (par->echo) StartEchoServer(par);

/* Set self up as an ISI server */

    server = iacpServer(par->port, &par->attr, &par->lp, par->debug);
    if (server == (IACP *) NULL) {
        LogMsg(LOG_ERR, "Unable to start ISI server: %s", strerror(errno));
        GracefulExit(MY_MOD_ID + 0);
    }

    return server;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
