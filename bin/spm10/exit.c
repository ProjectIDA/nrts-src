#pragma ident "$Id: exit.c,v 1.1 2016/04/28 23:01:49 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "spm10.h"

static MUTEX mutex;
static FILE *pipeFP = NULL;

BOOL ShutdownInProgress()
{
    MUTEX_LOCK(&mutex);
    MUTEX_UNLOCK(&mutex);

    return FALSE;
}

void SetPipeFILE(FILE *fp)
{
    MUTEX_LOCK(&mutex);
        pipeFP = fp;
    MUTEX_UNLOCK(&mutex);
}

void GracefulExit(INT32 status)
{
    MUTEX_LOCK(&mutex);

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - SPM10_MOD_SIGNALS);
    }

    if (pipeFP != NULL) pclose(pipeFP);

    LogMsg(LOG_INFO, "exit %ld", status);
    exit(status);
}

VOID InitExit(void)
{
    MUTEX_INIT(&mutex);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * Revision 1.1  2016/04/28 23:01:49  dechavez
 * initial release
 *
 */
