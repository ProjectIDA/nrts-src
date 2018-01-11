#pragma ident "$Id"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "i10dld.h"

static int ExitStatus;
static BOOL ExitHandlerInitialized = FALSE;
static MUTEX mutex;

void GrabGlobalMutex(char *fid)
{
    MUTEX_LOCK(&mutex);
}

void ReleaseGlobalMutex(char *fid)
{
    MUTEX_UNLOCK(&mutex);
}

void SetExitStatus(int status)
{
    ExitStatus = status; /* no mutex because this is called from signal handler */
}

void CheckExitStatus()
{
    if (ExitStatus != 0) Exit(ExitStatus);
}

void Exit(INT32 status)
{
static char *fid = "Exit";

    if (!ExitHandlerInitialized) exit(status);

    GrabGlobalMutex(fid); // will never release

    if (status < 0) {
        status = -status;
        LogMsg("going down on signal %d", status - MOD_SIGNALS);
    }

    CloseDiskLoop();

    LogMsg("exit %ld", status);
    exit((int) status);
}

void InitExit(void)
{
    MUTEX_INIT(&mutex);
    SetExitStatus(0);

    ExitHandlerInitialized = TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2018 Regents of the University of California            |
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
 * Revision 1.1  2018/01/10 21:20:18  dechavez
 * created
 *
 */
