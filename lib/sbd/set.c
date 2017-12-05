#pragma ident "$Id: set.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 *
 *  Insert parameters into the handle
 *
 *====================================================================*/
#include "sbd.h"

VOID sbdSetDebug(SBD *sbd, int value)
{
    if (sbd == NULL) return;

    sbd->debug = value;
}

VOID sbdSetLogio(SBD *sbd, LOGIO *logio)
{
    if (sbd == NULL) return;

    sbd->lp = logio;
}

VOID sbdSetRetryFlag(SBD *sbd, BOOL value)
{
    if (sbd == NULL) return;

    sbd->attr.at_retry = value;
}

VOID sbdSetDisabled(SBD *sbd, BOOL value)
{
    if (sbd == NULL) return;

    sbd->disabled = value;
}

VOID sbdSetSendError(SBD *sbd, int value)
{
    if (sbd == NULL) return;

    sbd->error.send = value;
}

VOID sbdSetRecvError(SBD *sbd, int value)
{
    if (sbd == NULL) return;

    sbd->error.recv = value;
}

/* Revision History
 *
 * $Log: set.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
