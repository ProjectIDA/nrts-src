#pragma ident "$Id: accept.c,v 1.2 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 * 
 * Accept incoming SBD client connection
 *
 *====================================================================*/
#define INCLUDE_SBD_DEFAULT_OPTS
#include "sbd.h"
#include "util.h"

SBD *sbdAccept(SBD *server)
{
SBD *sbd;
int LogLevel;
struct sockaddr_in cli_addr;
static struct linger linger = {0, 0};
socklen_t len = sizeof(cli_addr);
static char *fid = "sbdAccept";

    LogLevel = sbdGetDebug(server) ? LOG_INFO : LOG_DEBUG;

    if (server == NULL) {
        logioMsg(server->lp, LogLevel, "%s: invalid argument(s)", fid);
        errno = EINVAL;
        return NULL;
    }
 
/* Create/fill the handle */
 
    if ((sbd = (SBD *) malloc(sizeof(SBD))) == (SBD *) NULL) {
        logioMsg(server->lp, LogLevel, "%s: malloc: error %d", fid, errno);
        return NULL;
    }
    sbdInitHandle(sbd, NULL, server->port, &server->attr, server->lp, server->debug);

/* Accept a new connection */

    sbd->sd = INVALID_SOCKET;
    while (sbd->sd == INVALID_SOCKET) {
        sbd->sd = accept(server->sd, (struct sockaddr *) &cli_addr, &len);
        if (sbd->sd == INVALID_SOCKET && errno != EINTR) {
            logioMsg(server->lp, LogLevel, "%s: accept: error %d", fid, errno);
            return sbdClose(sbd);
        }
    }

/* Verify we are still accepting connections */

    if (sbdGetDisabled(server)) {
        logioMsg(server->lp, LOG_DEBUG, "%s: disabled flag set, breaking connection", fid);
        return sbdClose(sbd);
    }

/* Set socket options and complete the handle */

    utilSetNonBlockingSocket(sbd->sd);
    setsockopt(sbd->sd, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));
    sbd->connect = time(NULL);
    sbd->port = utilPeerPort(sbd->sd);
    sbd->attr = server->attr;
    utilPeerAddr(sbd->sd, sbd->peer.addr, INET_ADDRSTRLEN);
    utilPeerName(sbd->sd, sbd->peer.name, MAXPATHLEN);
    sprintf(sbd->peer.ident, "%s:%d", sbd->peer.name, sbd->port);

/* Return handle for this connection */

    return sbd;
}

/* Revision History
 *
 * $Log: accept.c,v $
 * Revision 1.2  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.1  2013/03/07 21:01:10  dechavez
 * initial release
 *
 */
