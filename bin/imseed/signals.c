#pragma ident "$Id: signals.c,v 1.2 2014/03/04 21:06:08 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "imseed.h"

#define MY_MOD_ID MOD_SIGNALS

static SEMAPHORE semaphore;

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    sigfillset(&set); /* catch all signals defined by the system */

    LogMsg("signal handler installed");
    SEM_POST(&semaphore);

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg("SIGTERM");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGQUIT:
            LogMsg("SIGQUIT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGINT:
            LogMsg("SIGINT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            LogMsg("SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg("SIGUSR2");
            LogMsgLevel(LOG_INFO);
            break;

          case SIGPIPE:
            break;

          case SIGSEGV:
            LogMsg("SIGSEGV");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          default:
            LogMsg("signal %d ignored", sig);
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
/* We keep the pthread specific function in place because there is no
 * easy way to emulate this under Windows.  This should not be a problem 
 * because this whole section is ifdef'd out under Windows and is known to
 * work under Solaris/Linux (where the THREAD macros are pthread based).
 */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    SEM_INIT(&semaphore, 0, 0);
    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        LogErr("%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID);
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&semaphore);

}

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.2  2014/03/04 21:06:08  dechavez
 * removed some tabs
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
