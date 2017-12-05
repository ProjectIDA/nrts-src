#pragma ident "$Id: close.c,v 1.1 2013/03/07 21:01:10 dechavez Exp $"
/*======================================================================
 * 
 * Close a connection, and free all resources.
 *
 *====================================================================*/
#include "sbd.h"
#include "util.h"

SBD *sbdClose(SBD *sbd)
{
static char *fid = "sbdClose";

    if (sbd == (SBD *) NULL) return NULL;

/* Shutdown the connection */

    if (sbd->sd != INVALID_SOCKET ) {
        shutdown(sbd->sd, 2);
        utilCloseSocket(sbd->sd);
    }
    logioMsg(sbd->lp, LOG_DEBUG, "%s: %s: connection closed", sbdPeerIdent(sbd), fid);

/* Free resources */

    if (sbd->dbgfp != NULL) {
        fclose(sbd->dbgfp);
        sbd->dbgfp = NULL;
    }
    return sbdFree(sbd);
}

/* Revision History
 *
 * $Log: close.c,v $
 * Revision 1.1  2013/03/07 21:01:10  dechavez
 * initial release
 *
 */
