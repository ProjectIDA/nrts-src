#pragma ident "$Id: main.c,v 1.2 2013/05/11 23:04:33 dechavez Exp $"
/*======================================================================
 * 
 * sbds - Iridium Short Burst Data Server
 *
 *====================================================================*/
#include "sbds.h"

#define MY_MOD_ID SBD_MOD_MAIN

static SBD *server;

static void StdinTest()
{
gzFile *gz;
SBD_MESSAGE message;
static char *fid = "StdinTest";

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        LogMsg(LOG_ERR, "*** ERROR *** %s: gzdopen: %s", fid, strerror(errno));
        exit(1);
    }

    while (sbdReadMessage(gz, &message)) {
        LogMsg(LOG_INFO, "%d bytes received from stdin\n", message.len);
        SaveMessage(&message);
    }
}

void DisableNewConnections(void)
{
    sbdSetDisabled(server, TRUE);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
PARAM *par;
SBD *client, *server;
static char buf[128];

/* Load the run time parameters */

    par = LoadPar(cmdline->myname, cmdline->argc, cmdline->argv);

/* Initialize everything */

    server = Init(cmdline->myname, par);

/* NULL server means to test with standard input */

    if (server == NULL) {
        StdinTest();
        GracefulExit(0);
    }

/* Ready to begin */

    LogMsg(LOG_INFO, "listening for SBD messages at port %d\n", par->port);

    while (1) {
        BlockOnShutdown();
        if ((client = sbdAccept(server)) != (SBD *) NULL) {
            if (!ShutdownInProgress()) {
                LogMsg(LOG_DEBUG, "%s: connection established", client->peer.ident);
                ServiceConnection(client);
            } else {
                sbdClose(client);
            }
        } else {
            LogMsg(LOG_WARN, "incoming SBD message failed: %s (ignored)", strerror(errno));
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
 * Revision 1.2  2013/05/11 23:04:33  dechavez
 * added stdin test support
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
