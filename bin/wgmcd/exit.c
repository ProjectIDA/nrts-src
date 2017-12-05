#pragma ident "$Id: exit.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "wgmcd.h"

static MUTEX mutex;

void BlockShutdown(char *fid)
{
    MUTEX_LOCK(&mutex);
}

void UnblockShutdown(char *fid)
{
    MUTEX_UNLOCK(&mutex);
}

VOID BlockOnShutdown(VOID)
{
    MUTEX_LOCK(&mutex);

/* if the system is going down, we'll never get here */

    MUTEX_UNLOCK(&mutex);
}

BOOL ShutdownInProgress(VOID)
{
    if (MUTEX_TRYLOCK(&mutex)) {
        MUTEX_UNLOCK(&mutex);
        return FALSE;
    } else {
        return TRUE;
    }
}

VOID GracefulExit(INT32 status)
{
int nclients;
static char *fid  = "GracefulExit";

    MUTEX_LOCK(&mutex);
    DisableNewConnections();

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - WGMCD_MOD_SIGNALS);
    }
    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

VOID InitGracefulExit()
{
    MUTEX_INIT(&mutex);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
