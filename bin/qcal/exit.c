#pragma ident "$Id: exit.c,v 1.11 2016/09/01 17:23:09 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits. 
 *
 *====================================================================*/
#include "qcal.h"

static QCAL *qcal = NULL;
static MUTEX mutex;

static BOOL AbortCalFlag = FALSE;
static INT32 ExitFlag = QCAL_STATUS_RUNNING;
static BOOL  DeleteFiles = FALSE;
static UINT32 DelayInterval = 15000;

void SetWatchdogExitDelay(UINT32 value)
{
    DelayInterval = value;
}

static THREAD_FUNC WatchDogExit(void *argptr)
{
UINT32 status;

    status = *((INT32 *) argptr);
    utilDelayMsec(DelayInterval);
    LogMsg("watchdog exit %d", status);
    exit(status);
}

void SetAbortCalFlag()
{
    MUTEX_LOCK(&mutex);
        AbortCalFlag = TRUE;
    MUTEX_UNLOCK(&mutex);
}

void SetDeleteFlag()
{
    MUTEX_LOCK(&mutex);
        DeleteFiles = TRUE;
    MUTEX_UNLOCK(&mutex);
}

void SetExitStatus(INT32 status)
{
    MUTEX_LOCK(&mutex);
        ExitFlag = status;
    MUTEX_UNLOCK(&mutex);
}

INT32 ExitStatus()
{
INT32 retval;

    MUTEX_LOCK(&mutex);
        retval = ExitFlag;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void Exit(INT32 status)
{
THREAD tid;
QDP *ConfigPortConnection;
static INT32 exitstatus;

    MUTEX_LOCK(&mutex);
    exitstatus = status;
    THREAD_CREATE(&tid, WatchDogExit, (void *) &exitstatus);

    if (status < 0) {
        status = -status;
        LogMsg("going down on signal %d", status - QCAL_MOD_SIGNALS);
    }

    CloseQDP(qcal);
    CloseIDA(qcal);
    CloseMSEED(qcal);

    ShutdownDataConnection(qcal);
    if ((ConfigPortConnection = RegisterWithConfigPort(qcal)) != NULL) {
        if (AbortCalFlag) AbortCal(ConfigPortConnection);
        RestoreDataPort(ConfigPortConnection, qcal);
        qdpShutdown(ConfigPortConnection);
    }

    if (DeleteFiles) {
        utilDeleteFile(qcal->name.qdp);
        utilDeleteFile(qcal->name.ida);
        utilDeleteFile(qcal->name.mseed);
        if (status == 0) utilDeleteFile(qcal->name.log);
    }

    LogMsg("exit %d", status);
    exit(status);
}

void InitExit(QCAL *pqcal)
{
    MUTEX_INIT(&mutex);
    qcal = pqcal;
}

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
 * $Log: exit.c,v $
 * Revision 1.11  2016/09/01 17:23:09  dechavez
 * use RegisterWithConfigPort() when managing Q330 clean up tasks
 *
 * Revision 1.10  2016/06/23 20:27:17  dechavez
 * reordered exit handling steps to ensure data files get closed
 *
 * Revision 1.9  2015/12/07 19:04:48  dechavez
 * added MiniSEED support
 *
 * Revision 1.8  2012/06/24 18:28:56  dechavez
 * Modified to use libqdp 3.4.0 where data port and tokens are bound together
 * and main channel output frequencies are forced to agree with token set
 *
 * Revision 1.7  2012/01/11 19:22:52  dechavez
 * added SetWatchdogExitDelay()
 *
 * Revision 1.6  2011/04/14 19:22:09  dechavez
 * added QCAL.token support
 *
 * Revision 1.5  2010/04/26 18:11:24  dechavez
 * allow second cntrl-C to force exit without attempting to abort cal
 *
 * Revision 1.4  2010/04/12 21:09:39  dechavez
 * don't delete log file if not a normal exit
 *
 * Revision 1.3  2010/04/05 21:27:50  dechavez
 * added watchdog exit thread
 *
 * Revision 1.2  2010/04/01 20:31:23  dechavez
 * rework to avoid race conditions, send a calibration abort if ending on a signal
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
