#pragma ident "$Id: util.c,v 1.4 2015/12/04 22:53:02 dechavez Exp $"
/*======================================================================
 *
 *  SBD convenience functions
 *
 *====================================================================*/
#define INCLUDE_SBD_DEFAULT_ATTR
#include "sbd.h"
#include "util.h"

/* Initialize all fields in handle */

VOID sbdInitHandle(SBD *sbd, char *peer, int port, SBD_ATTR *user_attr, LOGIO *lp, int debug)
{
SBD_ATTR attr;
static char *fid = "sbdInitHandle";

    if (sbd == NULL) return;

/* Set default attributes if none provided */

    if (user_attr != (SBD_ATTR *) NULL) {
        attr = *user_attr;
    } else {
        attr = SBD_DEFAULT_ATTR;
    }

/* Silently force timeout to be at least our minimum */

    if (attr.at_timeo < SBD_MINTIMEO) attr.at_timeo = SBD_MINTIMEO;

/* Fill it in */

    sbd->sd = INVALID_SOCKET;
    sbd->connect = 0;
    sbd->port = port;
    sbd->attr = attr;
    sbd->lp = lp;
    sbd->debug = debug;
    sbd->disabled = FALSE;
    sbd->error.send = sbd->error.recv = 0;
    if (peer != NULL) {
        if ( strlen( peer ) > MAXPATHLEN-1 ) {
            strncpy( sbd->peer.name, peer, MAXPATHLEN-1);
            sbd->peer.name[MAXPATHLEN-1] = '\0';
        } else {
            strlcpy(sbd->peer.name, peer, MAXPATHLEN);
        }
    } else {
        memset(sbd->peer.name, 0, sizeof(sbd->peer.name));
    }
    memset(sbd->peer.addr, 0, sizeof(sbd->peer.addr));
    if (attr.at_dbgpath != NULL) {
        if ((sbd->dbgfp = fopen(attr.at_dbgpath, "ab")) == NULL) {
            logioMsg(lp, LOG_WARN, "%s: fopen: %s: %s (ignored)", fid, attr.at_dbgpath, strerror(errno));
        } else {
            logioMsg(lp, LOG_INFO, "logging traffic to %s", attr.at_dbgpath);
        }
    } else {
        sbd->dbgfp = NULL;
    }
}

/* Determine error severity */

int sbdErrorLevel(int ErrorCode)
{
    switch (ErrorCode) {
#ifdef _WIN32
      case WSAEINTR:
      case WSAETIMEDOUT:
      case WSAECONNREFUSED:
      case WSAECONNRESET:
      case WSAENETDOWN:
      case WSAENETRESET:
      case WSAENETUNREACH:
      case WSAEHOSTUNREACH:
      case WSAEHOSTDOWN:
#else
      case EINTR:
      case ETIMEDOUT:
      case ECONNREFUSED:
      case ECONNRESET:
      case ENETDOWN:
      case ENETRESET:
      case ENETUNREACH:
      case EHOSTUNREACH:
      case EHOSTDOWN:
#endif
        return SBD_ERR_TRANSIENT;
    }
    return SBD_ERR_FATAL;
}

/* Get a sure to be printable peer string */

char *sbdPeerIdent(SBD *sbd)
{
static char *nil = "NULL";

    return (sbd == NULL) ? nil : sbd->peer.ident;
}

/* Convert at_maxerr attribute values into string equivalents */

char *sbdMaxerrString(UINT32 value)
{
static char *none      = "NONE";
static char *transient = "TRANSIENT";
static char *nonfatal  = "NON-FATAL";
static char *fatal     = "FATAL";
static char *unknown   = "UNKNOWN?";

    switch (value) {
      case SBD_ERR_NONE:
        return none;
      case SBD_ERR_TRANSIENT:
        return transient;
      case SBD_ERR_NONFATAL:
        return nonfatal;
      case SBD_ERR_FATAL:
        return fatal;
      default:
        return unknown;
    }
}

VOID sbdPrintAttr(FILE *fp, SBD_ATTR *attr)
{

    fprintf(fp, "Socket I/O timeout, msecs (at_timeo)  = %u\n", attr->at_timeo);
    fprintf(fp, "TCP/IP transmit buffer    (at_sndbuf) = %u\n", attr->at_sndbuf);
    fprintf(fp, "TCP/IP receive  buffer    (at_rcvbuf) = %u\n", attr->at_rcvbuf);
    fprintf(fp, "I/O error tolerance       (at_maxerr) = %s\n", sbdMaxerrString(attr->at_maxerr));
    fprintf(fp, "Retry flag                (at_retry)  = %s\n", utilBoolToString(attr->at_retry));
    fprintf(fp, "Retry interval, msecs     (at_wait)   = %u\n", attr->at_wait);

}

static void ClearMO(SBD_MO *mo)
{
    mo->header.valid   = FALSE;
    mo->payload.valid  = FALSE;
    mo->location.valid = FALSE;
    mo->confirm.valid  = FALSE;
}

static void ClearMT(SBD_MT *mt)
{
    mt->header.valid   = FALSE;
    mt->payload.valid  = FALSE;
    mt->confirm.valid  = FALSE;
    mt->priority.valid = FALSE;
}

void sbdClearMessageIEIs(SBD_MESSAGE *message)
{
    if (message == NULL) return;
    ClearMO(&message->mo);
    ClearMT(&message->mt);
}

VOID sbdClearMessage(SBD_MESSAGE *message)
{
    if (message == NULL) return;
    message->protocol = 1;
    message->len = 0;
    memset(message->body, 0, SBD_MAX_MSGLEN);
    sbdClearMessageIEIs(message);
}

/* Revision History
 *
 * $Log: util.c,v $
 * Revision 1.4  2015/12/04 22:53:02  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.3  2013/03/13 21:28:09  dechavez
 * introduced sbdClearMessageIEIs()
 *
 * Revision 1.2  2013/03/11 22:56:12  dechavez
 * added sbdClearMessage(), sbdUnpackMT_CONFIRM()
 *
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
