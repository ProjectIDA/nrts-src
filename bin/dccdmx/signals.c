#pragma ident "$Id: signals.c,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_SIGNALS

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
          case SIGQUIT:
          case SIGINT:
            LogMsg("%s\n", strsignal(sig));
            SetExitStatus(-(MY_MOD_ID + sig));
            break;

          default:
            LogMsg("%s ignored", strsignal(sig));
            break;
        }
    }
}

void StartSignalHandler(void)
{
int status;
THREAD tid;
sigset_t set;
static char *fid = "StartSignalHandler";

/* Block all signals */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        fprintf(stderr, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }
}

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
