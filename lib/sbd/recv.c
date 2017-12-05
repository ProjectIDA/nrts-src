#pragma ident "$Id: recv.c,v 1.2 2014/09/05 22:13:16 dechavez Exp $"
/*======================================================================
 * 
 * Receive a message from the peer.
 *
 *====================================================================*/
#include "sbd.h"
#include "util.h"

#define NOT_LOCKED FALSE

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

/* Attempt to read some bytes */

static size_t ReadBytes(SBD *sbd, int LogLevel, void *ptr, size_t count)
{
size_t ReadResult;

#ifdef  _WIN32
    ReadResult = recv(sbd->sd, ptr, count,0);
#else
    ReadResult = read(sbd->sd, ptr, count);
#endif

    if (ReadResult == 0) errno = ECONNRESET;
    sbdSetRecvError(sbd, errno);

    if (ReadResult > 0) return ReadResult;
    logioMsg(sbd->lp, LogLevel, "%s: read: %s", sbdPeerIdent(sbd), strerror(errno));

    return 0;
}

/* Local function for doing timeout enabled reads.  This assumes that
 * the file descripter has already been set for non-blocking I/O,
 * which is done as part of sbdOpen() and sbdAccept().
 */

static BOOL read_to(SBD *sbd, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t ReadResult;
UINT8 *ptr;
int SelectResult;
int LogLevel;
fd_set readfds;
UINT32 timeoutUS;
struct timeval timeout;
int loops;        /*count up EAGAINS */
static char *fid = "read_to";

    LogLevel = sbdGetDebug(sbd) ? LOG_INFO : LOG_DEBUG;

    if (want == 0) return TRUE;

    SelectResult = 0;
    timeoutUS = sbdGetTimeoutInterval(sbd) * USEC_PER_MSEC;
    timeout.tv_sec  = timeoutUS / USEC_PER_SEC;
    timeout.tv_usec = timeoutUS - (timeout.tv_sec * USEC_PER_SEC);

/* Mask out our file descriptor as the only one to look at */

    FD_ZERO(&readfds);
    FD_SET(sbd->sd, &readfds);

/* Read from socket until desired number of bytes acquired  */

    loops=0;
    remain = want;
    ptr    = buf;

    while (remain) {
        SelectResult = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
        sbdSetRecvError(sbd, errno);
        if (SelectResult > 0) {
            ReadResult = ReadBytes(sbd, LogLevel, (void *) ptr, (size_t) remain);
            if (ReadResult > 0) {
                remain -= ReadResult;
                ptr    += ReadResult;
            } else if (!IgnorableError(errno)) {
                logioMsg(sbd->lp, LogLevel, "%s: %s: ReadBytes: %s, return FALSE", sbdPeerIdent(sbd), fid, strerror(errno));
                return FALSE;
            } else {
                logioMsg(sbd->lp, LOG_INFO, "%s: %s: ReadBytes: %s (IGNORED), return FALSE", sbdPeerIdent(sbd), fid, strerror(errno));
#ifndef WIN32
                /* dck - found many EAGAINs causing disconnects. Just try again after a brief delay. 
                 * If we get 10 seconds or so of continuous problem, return FALSE and re-connect()
                 */
                usleep(10000);
                loops++;
                if(loops > 1000) return FALSE;
#endif /* !WIN32 */
            }
        } else if (SelectResult == 0) {
            sbdSetRecvError(sbd, errno = ETIMEDOUT);
            return FALSE;
        } else if (!IgnorableError(errno)) {
            logioMsg(sbd->lp, LogLevel, "%s: %s: select: %s", sbdPeerIdent(sbd), fid, strerror(errno));
            return FALSE;
        } else {
            logioMsg(sbd->lp, LOG_INFO, "%s: %s: select: %s (IGNORED)", sbdPeerIdent(sbd), fid, strerror(errno));
        }
    }
    if(loops >0) logioMsg(sbd->lp, LOG_INFO, "%s: %s: loops on EAGAIN=%d", sbdPeerIdent(sbd), fid, loops);

    return TRUE;
}

/* Receive an SBD message */

BOOL sbdRecvMessage(SBD *sbd, SBD_MESSAGE *message)
{
int parseCode;
UINT8 *ptr;
static char *fid = "sbdRecvMessage";

    if (sbd == (SBD *) NULL || message == (SBD_MESSAGE *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Read the preamble */

    if (!read_to(sbd, message->body, SBD_PROTOCOL_1_PREAMBLE_LEN)) {
        if (errno != ETIMEDOUT && errno != ECONNRESET) {
            logioMsg(sbd->lp, LOG_INFO, "%s: ERROR: read_to (preamble): %s", fid, strerror(errno));
        }
        return FALSE;
    }

/* Decode the preamble */

    ptr = message->body;
    message->protocol = *ptr++;
    ptr += utilUnpackUINT16(ptr, &message->len);

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

/* Read the message */

    if (!read_to(sbd, message->body + SBD_PROTOCOL_1_PREAMBLE_LEN, message->len)) {
        logioMsg(sbd->lp, LOG_INFO, "%s: ERROR: read_to (body): %s", fid, strerror(errno));
        return FALSE;
    }
    message->len += SBD_PROTOCOL_1_PREAMBLE_LEN; /* since we include the preamble in the body */

/* Parse the message */

    if ((parseCode = sbdParseMessageBody(message)) != SBD_OK) {
        logioMsg(sbd->lp, LOG_INFO, "%s: %d SBD parse error %d\n", fid, parseCode);
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: recv.c,v $
 * Revision 1.2  2014/09/05 22:13:16  dechavez
 * fixed typo spelling "unsupported" incorrectly
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
