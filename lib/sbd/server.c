#pragma ident "$Id: server.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 * 
 * Set self up as an SBD server listening on the specified port.
 *
 *====================================================================*/
#define INCLUDE_SBD_DEFAULT_ATTR
#include "sbd.h"
#include "util.h"

static SBD *Fail(SBD *sbd, int sd)
{
    if (sd != INVALID_SOCKET) utilCloseSocket(sd);
    if (sbd != (SBD *) NULL) free(sbd);

    return (SBD *) NULL;
}

SBD *sbdServer(int port, SBD_ATTR *user_attr, LOGIO *lp, int debug)
{
SBD *sbd;
struct sockaddr_in serverAddr;
static int yes = 1;
static struct linger linger = {0, 0};
static int sockAddrSize = sizeof(struct sockaddr_in);
static char *fid = "sbdServer";

/* Create/fill the handle */
 
    if ((sbd = (SBD *) malloc(sizeof(SBD))) == (SBD *) NULL) {
        logioMsg(lp, LOG_INFO, "%s: malloc: error %d", fid, errno);
        return (SBD *) NULL;
    }
    sbdInitHandle(sbd, fid, port, user_attr, lp, debug);
    sbd->attr.at_retry = FALSE;

/* Set up the local address */

    bzero((char *) &serverAddr, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons((short)port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

/* Create the socket */

    if ((sbd->sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        logioMsg(lp, LOG_INFO, "%s: socket: error %d", fid, errno);
        return Fail(sbd, sbd->sd);
    }

/* Set options */

    setsockopt(sbd->sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(int));
    setsockopt(sbd->sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &yes, sizeof(int));
    setsockopt(sbd->sd, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));

/* Bind socket to local address */

    if (bind(sbd->sd, (struct sockaddr *) &serverAddr, sockAddrSize) != 0) {
        logioMsg(lp, LOG_INFO, "%s: bind: error %d", fid, errno);
        return Fail(sbd, sbd->sd);
    }

/* Create queue for client connections */

    if (listen(sbd->sd, 5) != 0) {
        logioMsg(lp, LOG_INFO, "%s: listen: error %d", fid, errno);
        return Fail(sbd, sbd->sd);
    }

/* Return handle for this connection */

    return sbd;
}

/* Revision History
 *
 * $Log: server.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
