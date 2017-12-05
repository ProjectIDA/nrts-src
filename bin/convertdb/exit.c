#pragma ident "$Id: exit.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Exit handler
 *
 *====================================================================*/
#include "convertdb.h"

void GracefulExit(int status)
{
    CloseOldDatabase();
    CloseNewDatabase();
    LogMsg(1, "exit %d\n", status);
    exit(status);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
