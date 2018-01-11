/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "isi330.h"

static MUTEX mutex;
static INT32 ExitFlag = 0;
static ISI330_CONFIG *cfg = NULL;

void BlockShutdown(char *fid)
{
    MUTEX_LOCK(&mutex);
}

void UnblockShutdown(char *fid)
{
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

void SetExitStatus(INT32 status)
{
    MUTEX_LOCK(&mutex);
    ExitFlag = status;
    MUTEX_UNLOCK(&mutex);
    LogMsg("set exit status %ld", status);
}


void GracefulExit(INT32 status)
{
static char *fid = "Exit";

    BlockShutdown(fid);

    if (status < 0) {
        status = -status;
        LogMsg("shutting down on signal %ld", status - ISI330_MOD_SIGNALS);
    }

    ShutdownQ330Reader(cfg);

// TODO   isidlCloseDiskLoop(cfg->dl);

    exit(status);

}

void InitExit(ISI330_CONFIG *config)
{
    MUTEX_INIT(&mutex);
    cfg = config;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
