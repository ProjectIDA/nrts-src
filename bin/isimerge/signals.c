#pragma ident "$Id: signals.c,v 1.1 2010/09/10 22:56:31 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_SIGNALS

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
        Abort(-(MY_MOD_ID + (INT32) fdwCtrlType));
        retval = TRUE;  
        break;
      default:              
        retval = FALSE; 
    } 
    return retval;
}

VOID StartSignalHandler(VOID)
{
BOOL status;  
static char *fid = "StartSignalHandler";

/* Register our CtrlHandler function */

    status = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  /* handler function */
        TRUE                             /* add to list */
    );

    LogMsg(LOG_DEBUG, "signal handler installed");

    if (!status) {
        LogMsg(LOG_INFO, "%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );
        Abort(MY_MOD_ID);
    }
}

#else 

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    LogMsg(LOG_DEBUG, "signal handler installed");

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg(LOG_INFO, "SIGTERM");
            Abort(-(MY_MOD_ID + sig));
            break;

          case SIGQUIT:
            LogMsg(LOG_INFO, "SIGQUIT");
            Abort(-(MY_MOD_ID + sig));
            break;

          case SIGHUP:
            LogMsg(LOG_INFO, "SIGHUP ignored");
            break;

          case SIGINT:
            LogMsg(LOG_INFO, "SIGINT");
            Abort(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            LogMsg(LOG_INFO, "SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg(LOG_INFO, "SIGUSR2");
            LogMsgLevel(LOG_INFO);
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

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        exit(1);
    }
}

#endif /* ifdef WIN32 */

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
