#pragma ident "$Id: init.c,v 1.18 2015/12/08 18:37:51 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_INIT

IACP *Init(char *myname, PARAM *par)
{
IACP *server;
#ifdef HAVE_RLIMIT
struct rlimit limit;
#endif /* HAVE_RLIMIT */
static char *fid = "Init";

/* (attempt to) set our identity */

    utilSetIdentity(par->user);

/* Set the resource limits for number of open files to the max */

#if (defined(HAVE_RLIMIT) && !defined(DARWIN))
    if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        perror("getrlimit");
        exit(1);
    }
    limit.rlim_cur = limit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        perror("setrlimit");
        exit(1);
    }
#endif /* HAVE_RLIMIT and NOT DARWIN */

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

/* Open all disk loops */

    if ((par->master = isidlOpenDiskLoopSet(NULL, &par->glob, &par->lp, ISI_RDONLY, ISI_OPTION_NONE)) == NULL) {
        LogMsg(LOG_ERR, "ABORT *** isidlOpenDiskLoopSet failed ***");
        exit(1);
    }
    if (par->master->list.count == 0) {
        LogMsg(LOG_ERR, "ABORT *** No valid disk loops found ***");
        exit(1);
    }
    LogMsg(LOG_INFO, "%d disk loops opened", par->master->list.count);

/* Intialize the client list */

    InitClientList(par);

/* Initialize the report generator */

    InitReportGenerator(par);

/* Start up status report server */

    if (par->status) StartStatusServer(par);

/* Start up legacy NRTS server */

    if (par->nrts) StartNrtsServer(par);

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
 * Revision 1.18  2015/12/08 18:37:51  dechavez
 * don't attempt to set resource limits under DARWIN.  I get an error that I
 * can't be bothered tracking down, particularily since the current limit is
 * prettty large (2560 files) and since I really don't expect to ever actually
 * be using Mac OS systems as ISI servers.
 *
 * Revision 1.17  2014/08/28 21:45:30  dechavez
 * include option flags as isidlOpenDiskLoopSet() argument
 *
 * Revision 1.16  2012/09/05 18:32:46  dechavez
 * declare rlimit only if HAVE_RLIMIT defined
 *
 * Revision 1.15  2012/05/30 21:15:27  dechavez
 * added echo service support
 *
 * Revision 1.14  2010/11/10 21:07:40  dechavez
 * abort if no viable disk loops are found
 *
 * Revision 1.13  2010/02/08 19:03:35  dechavez
 * set the resource limits for number of open files to the max
 *
 * Revision 1.12  2009/01/06 20:48:26  dechavez
 * removed tabs
 *
 * Revision 1.11  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.10  2007/01/11 22:02:18  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.9  2006/02/10 02:22:02  dechavez
 * note how many disk loops in the system list were actually opened
 *
 * Revision 1.8  2005/10/17 21:12:05  dechavez
 * Start status server only if port has been explicitly specified
 *
 * Revision 1.7  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.6  2005/07/06 15:52:52  dechavez
 * use utilSetIdentity instead of relying on setuid bit
 *
 * Revision 1.5  2005/06/24 21:54:07  dechavez
 * checkpoint, additional seqno support in place, but not yet ready
 *
 * Revision 1.4  2004/06/30 20:49:33  dechavez
 * improve comments
 *
 * Revision 1.3  2004/06/10 20:43:25  dechavez
 * pass home directory to nrtsOpen
 *
 * Revision 1.2  2004/04/26 21:19:01  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.1  2003/10/16 18:07:23  dechavez
 * initial release
 *
 */
