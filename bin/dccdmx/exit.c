#pragma ident "$Id: exit.c,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
/*======================================================================
 *
 * Graceful exits
 *
 *====================================================================*/
#include "dccdmx.h"

static MUTEX mutex;
static INT32 ExitFlag = 0;

INT32 ExitStatus(void)
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
}

void Exit(INT32 status)
{
    if (status < 0) {
        status = -status;
        LogMsg("going down on signal %ld\n", status - DCCDMX_MOD_SIGNALS);
    }

    CloseTS();
    CloseLM();
    CloseCF();
    CloseCA();
    CloseII();

    ReportCounts();
    LogMsg("exit %ld\n", status);
    exit(status);
}

void CheckForShutdown(void)
{
INT32 status;

    if ((status = ExitStatus()) != 0) Exit(status);
}

void InitExit(void)
{
    MUTEX_INIT(&mutex);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
