#pragma ident "$Id: signals.c,v 1.2 2013/05/11 23:04:52 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "sbds.h"

#define MY_MOD_ID SBDS_MOD_SIGNALS

static SEMAPHORE semaphore;

#ifdef WIN32

/* Windows creates a thread that runs our signal handler */

static BOOL CtrlHandler(DWORD fdwCtrlType)
{     
BOOL retval;

    switch (fdwCtrlType) { 
      case CTRL_C_EVENT:  
      case CTRL_SHUTDOWN_EVENT:          
      case CTRL_CLOSE_EVENT:              
      case CTRL_BREAK_EVENT:          
        GracefulExit(-(MY_MOD_ID + (INT32) fdwCtrlType));
        retval = TRUE;  
        break;
      default:              
        retval = FALSE; 
    } 
    return retval;
}

void StartSignalHandler(void)
{
BOOL status;  
static char *fid = "StartSignalHandler";

/* Register our CtrlHandler function */

    status = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  /* handler function */
        TRUE                             /* add to list */
    );

    LogMsg(LOG_INFO, "signal handler installed");
    SEM_POST(&semaphore);

    if (!status) {
        LogMsg(LOG_ERR, "%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );
        GracefulExit(MY_MOD_ID);
    }
}

#else 

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    sigfillset(&set); /* catch all signals defined by the system */

    LogMsg(LOG_INFO, "signal handler installed");
    SEM_POST(&semaphore);

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg(LOG_INFO, "SIGTERM");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGQUIT:
            LogMsg(LOG_INFO, "SIGQUIT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGINT:
            LogMsg(LOG_INFO, "SIGINT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            LogMsg(LOG_INFO, "SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg(LOG_INFO, "SIGUSR2");
            LogMsgLevel(LOG_INFO);
            break;

          case SIGPIPE:
            break;

          case SIGSEGV:
            LogMsg(LOG_INFO, "SIGSEGV");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          default:
            LogMsg(LOG_INFO, "signal %d ignored", sig);
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
        LogMsg(LOG_ERR, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID);
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&semaphore);

}

#endif /* ifdef WIN32 */

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.2  2013/05/11 23:04:52  dechavez
 * VOID -> void
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
