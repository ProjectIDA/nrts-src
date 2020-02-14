#pragma ident "$Id: exit.c,v 1.2 2013/05/11 23:09:09 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "sbds.h"

static MUTEX mutex;

void BlockShutdown(char *fid)
{
    MUTEX_LOCK(&mutex);
}

void UnblockShutdown(char *fid)
{
    MUTEX_UNLOCK(&mutex);
}

void BlockOnShutdown(void)
{
    MUTEX_LOCK(&mutex);

/* if the system is going down, we'll never get here */

    MUTEX_UNLOCK(&mutex);
}

BOOL ShutdownInProgress(void)
{
    if (MUTEX_TRYLOCK(&mutex)) {
        MUTEX_UNLOCK(&mutex);
        return FALSE;
    } else {
        return TRUE;
    }
}

void GracefulExit(INT32 status)
{
int nclients;
static char *fid  = "GracefulExit";

    MUTEX_LOCK(&mutex);
    DisableNewConnections();

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - SBDS_MOD_SIGNALS);
    }

    CloseAllDiskLoops();

    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

void InitGracefulExit()
{
    MUTEX_INIT(&mutex);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.2  2013/05/11 23:09:09  dechavez
 * VOID -> void
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
