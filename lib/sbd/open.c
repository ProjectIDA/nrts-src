#pragma ident "$Id: open.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 * 
 * Establish a connection with an SBD server.  If there are connect
 * errors, then the function will either quit or retry depending on
 * the value of the retry flag in the attributes.
 *
 *====================================================================*/
#include "sbd.h"

SBD *sbdOpen(char *server, int port, SBD_ATTR *attr, LOGIO *lp, int debug)
{
SBD *sbd;
static char *fid = "sbdOpen";

/* Argument check */

    if (server == (char *) NULL || port <= 0) {
        logioMsg(lp, LOG_INFO, "%s: invalid argument(s)", fid);
        errno = EINVAL;
        return (SBD *) NULL;
    }

/* Create handle */

    if ((sbd = (SBD *) malloc(sizeof(SBD))) == NULL) {
        logioMsg(lp, LOG_INFO, "%s: malloc: %s", fid, strerror(errno));
        return sbdClose(sbd);
    }
    sbdInitHandle(sbd, server, port, attr, lp, debug);
    if (debug > 0) logioSetThreshold(lp, LOG_DEBUG);

/* Establish connection with server, including handshake */

    if (!sbdConnect(sbd)) {
        logioMsg(lp, LOG_INFO, "%s: unable to connect to %s:%d", fid, server, port);
        return sbdClose(sbd);
    }

/* Successful connection/handshake, return the handle */

    return sbd;
}

/* Revision History
 *
 * $Log: open.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
