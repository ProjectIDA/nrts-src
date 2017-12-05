#pragma ident "$Id: signals.c,v 1.6 2016/09/01 17:10:13 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_SIGNALS

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
        SetExitStatus(-(MY_MOD_ID + (INT32) fdwCtrlType));
        retval = TRUE;  
        break;
      default:              
        retval = FALSE; 
    } 
    return retval;
}

VOID StartSignalHandler(QCAL *unused)
{
BOOL status;  
static char *fid = "StartSignalHandler";

/* Register our CtrlHandler function */

    status = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  /* handler function */
        TRUE                             /* add to list */
    );

    if (!status) {
        LogMsg("%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );
        SetExitStatus(MY_MOD_ID);
    }
}

#else 

static THREAD_FUNC SignalHandlerThread(void *arg)
{
QCAL *qcal;
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    qcal = (QCAL *) arg;

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);
        LogMsg("%s", util_sigtoa(sig));
        printf("\n%s\n", util_sigtoa(sig));

        /* process signals */

        switch (sig) {
          case SIGTERM:
          case SIGQUIT:
          case SIGINT:
              SetAbortCalFlag();
              SetExitStatus(-(MY_MOD_ID + (INT32) sig));
            break;

          default:
            LogMsg("signal %d ignored", sig);
            break;
        }
    }
}

VOID StartSignalHandler(QCAL *qcal)
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

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) qcal)) {
        LogMsg("%s: THREAD_CREATE: %s", fid, strerror(errno));
        exit(1);
    }
}

#endif /* ifdef WIN32 */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: signals.c,v $
 * Revision 1.6  2016/09/01 17:10:13  dechavez
 * changed SIGINT handler to call SetAbortCalFlag() intstead of AbortCal()
 *
 * Revision 1.5  2010/04/26 18:11:24  dechavez
 * allow second cntrl-C to force exit without attempting to abort cal
 *
 * Revision 1.4  2010/04/26 17:23:50  dechavez
 * Abort calibration upon receipt of termination signals
 *
 * Revision 1.3  2010/04/12 21:07:41  dechavez
 * removed console printing
 *
 * Revision 1.2  2010/04/01 20:25:11  dechavez
 * don't actually Exit(), just set flag instead (avoids race conditions)
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
