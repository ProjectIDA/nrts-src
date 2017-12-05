#pragma ident "$Id: exit.c,v 1.1 2012/04/25 21:21:04 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "tristar10.h"

static BOOL ShutdownFlag = FALSE, FlushFlag = FALSE;
static MUTEX mutex;

void SetShutdownFlag(void)
{
BOOL FlagAlreadySet;
static char *fid = "SetShutdownFlag";

    MUTEX_LOCK(&mutex);
        FlagAlreadySet = ShutdownFlag;
        ShutdownFlag = TRUE;
    MUTEX_UNLOCK(&mutex);

    if (!FlagAlreadySet) LogMsg("shutdown initiated");
}

void SetFlushFlag(void)
{
BOOL FlagAlreadySet;
static char *fid = "SetFlushFlag";

    MUTEX_LOCK(&mutex);
        FlagAlreadySet = FlushFlag;
        FlushFlag = TRUE;
    MUTEX_UNLOCK(&mutex);

    if (!FlagAlreadySet) LogMsg("flush flag set");
}

BOOL FlushFlagSet(void)
{
BOOL retval;

/* Return current value and clear if set */

    MUTEX_LOCK(&mutex);
        retval = FlushFlag;
        FlushFlag = FALSE;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void QuitOnShutdown(INT32 status)
{
BOOL done;

    MUTEX_LOCK(&mutex);
        done = ShutdownFlag;
    MUTEX_UNLOCK(&mutex);

    if (done) {
        FlushAllPackets();
        sleep(5); /* so they can get from the queue to the socket */
        Exit(status);
    }
}

void Exit(INT32 status)
{
static char *fid = "Exit";

    LogMsg("exit %ld", status);
    exit(status);
}

void InitExit(void)
{
    MUTEX_INIT(&mutex);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
