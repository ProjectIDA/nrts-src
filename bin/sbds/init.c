#pragma ident "$Id: init.c,v 1.2 2013/05/11 23:08:53 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "sbds.h"

#define MY_MOD_ID SBDS_MOD_INIT

SBD *Init(char *myname, PARAM *par)
{
SBD *server;
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

/* Initialize the ADDOSS packet processor */

    InitADOSSprocessor(par);

/* Open SBD disk loop and initialize IDA10 disk loop manager */

    InitDLmgr(par);

/* If we are testing, then skip the server part */

    if (par->port == STDIN_PORT) return NULL;

/* Otherwise set self up as an SBD server */

    server = sbdServer(par->port, &par->attr, &par->lp, par->debug);
    if (server == (SBD *) NULL) {
        LogMsg(LOG_ERR, "Unable to start SBD server: %s", strerror(errno));
        GracefulExit(MY_MOD_ID + 0);
    }

    return server;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.2  2013/05/11 23:08:53  dechavez
 * port=stdin and ADDOSS/IDA10 support
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
