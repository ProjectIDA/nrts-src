#pragma ident "$Id: main.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 * 
 * wgmcd - Wave Glider "Mission Control" Daemon
 *
 *====================================================================*/
#include "wgmcd.h"

#define MY_MOD_ID WGMCD_MOD_MAIN

static IACP *server;

VOID DisableNewConnections(VOID)
{
    iacpSetDisabled(server, TRUE);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
PARAM *par;
IACP *client, *server;
static char buf[128];

/* Load the run time parameters */

    par = LoadPar(cmdline->myname, cmdline->argc, cmdline->argv);

/* Initialize everything */

    server = Init(cmdline->myname, par);

/* Ready to begin */

    LogMsg(LOG_INFO, "listening for WG connections at port %d\n", par->port);

    while (1) {
        BlockOnShutdown();
        if ((client = iacpAccept(server)) != (IACP *) NULL) {
            if (!ShutdownInProgress()) {
                LogMsg(LOG_DEBUG, "%s: connection established", client->peer.ident);
                ServiceConnection(client);
            } else {
                iacpDisconnect(client, IACP_ALERT_SHUTDOWN);
            }
        } else {
            LogMsg(LOG_WARN, "incoming WG connection failed: %s (ignored)", strerror(errno));
        }
    }
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

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
