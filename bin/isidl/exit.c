#pragma ident "$Id: exit.c,v 1.13 2012/08/02 17:03:53 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "isidl.h"

static MUTEX mutex;
static ISIDL_PAR *par = NULL;
static INT32 ExitFlag = 0;

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
    LogMsg(LOG_DEBUG, "set exit status %ld", status);
}

void Hold(INT32 status)
{
    SetExitStatus(status);
#ifndef WIN32
    while (1) pause();
#else
    while (1) Sleep(1000);
#endif /* WIN32 */
}

void Exit(INT32 status)
{
int i;
static char *fid = "Exit";

    BlockShutdown(fid);

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - ISIDL_MOD_SIGNALS);
    }

    if (par != NULL) {
        for (i = 0; i < par->nsite; i++) isidlCloseDiskLoop(par->dl[i]);
    }

    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

VOID InitExit(ISIDL_PAR *ptr)
{
    MUTEX_INIT(&mutex);
    par = ptr;
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.13  2012/08/02 17:03:53  dechavez
 * added WIN32 Sleep() instead of pause() (aap)
 *
 * Revision 1.12  2010/04/02 18:37:47  dechavez
 * removed DeregisterQ330s() call, added debug log message to SetExitStatus()
 *
 * Revision 1.11  2010/04/01 22:05:29  dechavez
 * added code to deregister Q330s on exit, and new set and hold functions
 *
 * Revision 1.10  2010/03/31 22:44:42  dechavez
 * deregister Q330s on exit
 *
 * Revision 1.9  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.8  2006/06/23 18:31:05  dechavez
 * removed Q330 deregistration
 *
 * Revision 1.7  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.6  2006/06/07 22:41:17  dechavez
 * Deregister all Q330 connections on shutdown
 *
 * Revision 1.5  2006/03/14 20:31:48  dechavez
 * pass fid to block/unlock exit mutex calls, for debugging use
 *
 * Revision 1.4  2006/03/13 23:07:13  dechavez
 * added BlockShutdown(), UnblockShutdown()
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
