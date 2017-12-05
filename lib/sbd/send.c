#pragma ident "$Id: send.c,v 1.2 2014/09/05 22:13:16 dechavez Exp $"
/*======================================================================
 * 
 * Send a message to the peer.
 *
 *====================================================================*/
#include "sbd.h"
#include "util.h"

#define ALREADY_LOCKED TRUE

/* Some errors should be ignored */

static BOOL IgnorableError(int error)
{
    switch (error) {
      case EWOULDBLOCK:
#if (EWOULDBLOCK != EAGAIN)
      case EAGAIN:
#endif
      case EINTR:
        return TRUE;
    }

    return FALSE;
}

/* Local function for doing timeout enabled writes.  This assumes that
 * the file descripter has already been set for non-blocking I/O,
 * which is done as part of sbdOpen() for the clients, and sbdAccept()
 * for the servers.
 */

static BOOL write_to(SBD *sbd, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t WriteResult;
UINT8 *ptr;
int SelectResult;
int LogLevel;
UINT32 timeoutUS;
fd_set writefds;
struct timeval timeout;
static char *fid = "write_to";

    if (want == 0) return TRUE;

    LogLevel = sbd->debug ? LOG_INFO : LOG_DEBUG;

    if (buf == NULL) {
        sbdSetSendError(sbd, errno = EINVAL);
        logioMsg(sbd->lp, LogLevel, "%s: NULL buf!", sbdPeerIdent(sbd), fid);
        return FALSE;
    }

    SelectResult = 0;
    timeoutUS = sbd->attr.at_timeo * USEC_PER_MSEC;
    timeout.tv_sec  = timeoutUS / USEC_PER_SEC;
    timeout.tv_usec = timeoutUS - (timeout.tv_sec * USEC_PER_SEC);

/* Mask out our file descriptor as the only one to look at */

    FD_ZERO(&writefds);
    FD_SET(sbd->sd, &writefds);
    
/*  Write to socket until desired number of bytes sent */

    remain = want;
    ptr    = buf;

    while (remain) {
        SelectResult = select(FD_SETSIZE, NULL, &writefds, NULL, &timeout);
        sbdSetSendError(sbd, errno);
        if (SelectResult == 0) {
            sbdSetSendError(sbd, errno = ETIMEDOUT);
            logioMsg(sbd->lp, LogLevel, "%s: %s: select: %s: to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
            return FALSE;
        } else if (SelectResult < 0) {
            if (!IgnorableError(errno)) {
                logioMsg(sbd->lp, LogLevel, "%s: %s: select: %s: to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
                return FALSE;
            } else {
                logioMsg(sbd->lp, LOG_INFO, "%s: %s: select: %s (IGNORED): to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
            }
        } else {
            WriteResult = send(sbd->sd, (void *) ptr, remain, 0);
            sbdSetSendError(sbd, errno);
            if (WriteResult > 0) {
                remain -= WriteResult;
                ptr    += WriteResult;
            } else if (WriteResult == 0) {
                sbdSetSendError(sbd, errno = ECONNRESET);
                logioMsg(sbd->lp, LogLevel, "%s: %s: send: %s: to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
                return FALSE;
            } else if (!IgnorableError(errno)) {
                logioMsg(sbd->lp, LogLevel, "%s: %s: send: %s: to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
                return FALSE;
            } else {
                logioMsg(sbd->lp, LOG_INFO, "%s: %s: send: %s (IGNORED): to=%d.%06d", sbdPeerIdent(sbd), fid, strerror(errno), timeout.tv_sec, timeout.tv_usec);
            }
        }
    }

    sbdSetSendError(sbd, 0);
    return TRUE;
}

/* Send an SBD message */

BOOL sbdSendMessage(SBD *sbd, SBD_MESSAGE *message)
{
int LogLevel;
UINT8 *ptr;
static char *fid = "sbdSendMessage";

    if (sbd == (SBD *) NULL || message == (SBD_MESSAGE *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    LogLevel = sbd->debug ? LOG_INFO : LOG_DEBUG;

    if (message->protocol != 1) {
        logioMsg(sbd->lp, LOG_INFO, "%s: unsupported protocol '%d'", fid, message->protocol);
        errno = EINVAL;
        return FALSE;
    }

    if (message->len > SBD_MAX_MSGLEN) {
        logioMsg(sbd->lp, LOG_INFO, "%s: illegal length '%d'", fid, message->len);
        errno = EINVAL;
        return FALSE;
    }

    /* the message body includes the 3-byte preamble */

    if (!write_to(sbd, message->body, message->len)) {
        logioMsg(sbd->lp, LogLevel, "%s: %s: error sending %ld byte message: %s", sbdPeerIdent(sbd), fid, message->len, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: send.c,v $
 * Revision 1.2  2014/09/05 22:13:16  dechavez
 * fixed typo spelling "unsupported" incorrectly
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
