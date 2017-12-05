#pragma ident "$Id: exit.c,v 1.1 2010/09/10 22:56:31 dechavez Exp $"
/*======================================================================
 *
 *  Exit handler
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISIMERGE_MOD_EXIT

static BOOL initialized = FALSE;
static MUTEX     mutex;
static int status = -1;

int ExitStatus()
{
    return status;
}

BOOL InitExitHandler()
{
    MUTEX_INIT(&mutex);
    status = 0;
    return TRUE;
}

void Abort(int value)
{
    LockGlobalMutex(); /* never release so we can only be called once */
    status = value;
    WakeMainThread();
    while (1) pause();
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
