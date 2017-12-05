#pragma ident "$Id: connect.c,v 1.2 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 * 
 * Connect to server with optional retry
 *
 *====================================================================*/
#include "sbd.h"
#include "util.h"

static VOID SetErrorLevel(int ErrorCode, int *ErrorLevel, int *LogLevel)
{
    *ErrorLevel = sbdErrorLevel(ErrorCode);
    *LogLevel = (*ErrorLevel == SBD_ERR_TRANSIENT) ? LOG_DEBUG : LOG_INFO;
}

BOOL sbdConnect(SBD *sbd)
{
BOOL retry;
int ErrorLevel, LogLevel;
int keepalive = 1;
int ilen = sizeof(int);
unsigned int RetryInterval;
struct sockaddr_in serverAddr;
static char *fid = "sbdConnect";

    if (sbd == (SBD *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    retry = sbdGetRetry(sbd);

/* Set the server address */

    if (!utilSetHostAddr(&serverAddr, sbd->peer.name, sbd->port)) {
        logioMsg(sbd->lp, LOG_INFO, "%s: server %s: %s", fid, sbd->peer.name, strerror(errno));
        return FALSE;
    }

    sbd->sd = INVALID_SOCKET;
    while (1) {

/* create the socket */

        if ((sbd->sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            logioMsg(sbd->lp, LOG_INFO, "%s: socket: %s", fid, strerror(errno));
            return FALSE;
        }

/* set initial socket options */

        setsockopt(sbd->sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &keepalive, ilen);
        if (sbd->attr.at_sndbuf > 0) {
            setsockopt(sbd->sd, SOL_SOCKET, SO_SNDBUF, (char *) &sbd->attr.at_sndbuf, ilen);
        }
        if (sbd->attr.at_rcvbuf > 0) {
            setsockopt(sbd->sd, SOL_SOCKET, SO_RCVBUF, (char *) &sbd->attr.at_rcvbuf, ilen);
        }

/* attempt to connect */

        if (connect(sbd->sd,(struct sockaddr *)&serverAddr,sizeof(struct sockaddr_in)) < 0) {
            SetErrorLevel(errno, &ErrorLevel, &LogLevel);
            logioMsg(sbd->lp, LogLevel, "%s: connect: %s", fid, strerror(errno));
            sbd->sd = utilCloseSocket(sbd->sd);
        }

/* if connected OK, update handle */

        if (sbd->sd != INVALID_SOCKET) {
            utilSetNonBlockingSocket(sbd->sd);
            sbd->connect = time(NULL);
            utilPeerAddr(sbd->sd, sbd->peer.addr, INET_ADDRSTRLEN);
            utilPeerName(sbd->sd, sbd->peer.name, MAXPATHLEN);
            sprintf(sbd->peer.ident, "pid?@%s", sbd->peer.name);
            SetErrorLevel(errno, &ErrorLevel, &LogLevel);
            return TRUE;
        }

/* must have encountered an error */

        if (!retry || ((UINT32) ErrorLevel > sbdGetMaxError(sbd))) {
            return FALSE;
        } else {
            sbd->sd = utilCloseSocket(sbd->sd);
            RetryInterval = sbdGetRetryInterval(sbd);
            logioMsg(sbd->lp, LOG_DEBUG, "%s: delay %lu msec", fid, RetryInterval);
            utilDelayMsec(RetryInterval);
        }
    }
}

BOOL sbdReconnect(SBD *sbd)
{
    sbd->sd = utilCloseSocket(sbd->sd);
    return sbdConnect(sbd);
}

/* Revision History
 *
 * $Log: connect.c,v $
 * Revision 1.2  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
