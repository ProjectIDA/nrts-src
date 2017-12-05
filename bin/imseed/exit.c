#pragma ident "$Id: exit.c,v 1.4 2015/05/21 21:54:31 dechavez Exp $"
/*======================================================================
 *
 *  Exit handler
 *
 *====================================================================*/
#include "imseed.h"

static UINT32 nrec = 0;
static UINT32 nbad = 0;

void IncrementInputCount(void)
{
    ++nrec;
}

void IncrementBadCount(void)
{
    ++nbad;
}

UINT32 InputCount(void)
{
    return nrec;
}

void GracefulExit(int status)
{
    CloseFiles();
    if (status < 0) {
        status = -status;
        LogMsg("going down on signal %ld\n", status - MOD_SIGNALS);
    }
    LogMsg("%lu input records processed, %d records skipped\n", nrec, nbad);
    if (status == 0) {
        LogMsg("program completed normally\n");
    } else {
        LogMsg("exit %d\n", status);
    }

    exit(status);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.4  2015/05/21 21:54:31  dechavez
 * added IncrementBadCount() and include that in the final log report
 *
 * Revision 1.3  2014/08/11 20:40:45  dechavez
 * added InputCount()
 *
 * Revision 1.2  2014/03/04 21:07:38  dechavez
 * fixed up CVS tags
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 */
