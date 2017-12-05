#pragma ident "$Id: init.c,v 1.5 2016/06/15 21:20:55 dechavez Exp $"
/*======================================================================
 * 
 * Init a QIO handle
 *
 *====================================================================*/
#include "qio.h"

#define UDP_PREFIX "udp:"
#define TTY_PREFIX "tty:"
#define DEV_PREFIX "/dev/"

static int InitTTY(char *iostr, char *ident, TTYIO_ATTR *attr)
{
#define MAXTOKEN 5
#define DELIMITERS ":"
char *token[MAXTOKEN];
int i, ntoken;

    if ((ntoken = utilParse(iostr, token,  DELIMITERS, MAXTOKEN, 0)) < 1) return QIO_ERR;
    if (ident != NULL) snprintf(ident, MAXPATHLEN, "%s%s", TTY_PREFIX, token[0]);

    for (i = 1; i < ntoken; i++) {
        switch (i) {
          case 1:
            if (attr != NULL) attr->at_ibaud = attr->at_obaud = (INT32) atoi(token[i]);
            break;
          case 2:
            if (attr != NULL) if ((attr->at_parity = ttyioStringToParity(token[i])) == TTYIO_PARITY_BAD) return FALSE;
            break;
          case 3:
            if (attr != NULL) if ((attr->at_flow = ttyioStringToFlow(token[i])) == TTYIO_FLOW_BAD) return FALSE;
            break;
          default:
            return QIO_ERR;
        }
    }
//DEBUG//printf("InitTTY: ident=%s, iostr=%s\n", ident, iostr);

    return QIO_TTY;
}

int qioConnectionType(char *iostr, char *ident, TTYIO_ATTR *attr)
{
//DEBUG//printf("qioConnectionType: ident=%s, iostr=%s\n", ident, iostr);
    if (strncmp(iostr, DEV_PREFIX, strlen(DEV_PREFIX)) == 0) {
        return (ident != NULL && attr != NULL) ? InitTTY(iostr, ident, attr) : QIO_TTY;
    } else {
        return QIO_UDP;
    }
}

BOOL qioInit(QIO *qio, UINT32 myIp, int myPort, char *iostr, int to, LOGIO *lp, int debug)
{
char *device;
TTYIO_ATTR attr = QIO_DEFAULT_TTY_ATTR;
char strbuf[MAXPATHLEN];
static char *fid = "qioInit";

    if (qio == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    qio->debug = debug;

    MUTEX_INIT(&qio->mutex);
    qio->lp = lp;

    qio->xmit.ident = 0;
    qioInitStats(&qio->xmit.stats);
    qio->my.port = myPort;

    qio->recv.nread = qio->recv.index = -1;
    qio->recv.timeout = to * NANOSEC_PER_MSEC;
    utilInitTimer(&qio->recv.timer);
    qioInitStats(&qio->recv.stats);

    if (iostr == NULL) {
        qio->type = QIO_UDP;
        qio->my.ip = myIp ? myIp : utilMyIpAddr();
        snprintf(qio->ident, MAXPATHLEN, "%s%d", UDP_PREFIX, qio->my.port);
        qioDebug(qio, QIO_DEBUG_TERSE, "%s: myIP=%s myPort=%d, iostr=%s, ident=%s, type=UDP\n", fid, utilDotDecimalString(qio->my.ip, strbuf), qio->my.port, iostr, qio->ident);
        return udpioInit(&qio->method.udp, qio->my.port, to, lp);
    }

    qio->my.ip = myIp ? myIp : QIO_DEFAULT_MY_IP;
    switch (qio->type = qioConnectionType(iostr, qio->ident, &attr)) {
      case QIO_TTY:
        device = qio->ident + strlen(TTY_PREFIX);
        qio->method.tty = ttyioOpen(
            device,
            attr.at_lock,
            attr.at_ibaud,
            attr.at_obaud,
            attr.at_parity,
            attr.at_flow,
            attr.at_sbits,
            attr.at_to,
            attr.at_pipe,
            qio->lp
        );
        qioDebug(qio, QIO_DEBUG_TERSE, "%s: device=%s ibaud=%d obaud=%d parity=%s, flow=%s, sbits=%d to=%d\n",
            fid,
            device,
            attr.at_ibaud,
            attr.at_obaud,
            ttyioParityToString(attr.at_parity),
            ttyioFlowToString(attr.at_flow),
            attr.at_sbits,
            attr.at_to
        );
        return (qio->method.tty == NULL) ? FALSE : TRUE;
    }

    errno = EINVAL;
    return FALSE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.5  2016/06/15 21:20:55  dechavez
 * debug argument added to qioInit()
 *
 * Revision 1.4  2014/08/11 18:01:19  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.3  2011/02/07 19:56:35  dechavez
 * require "/dev/" instead of just "/" prefix for determining if the connection is to a serial port
 *
 * Revision 1.2  2011/01/31 18:18:22  dechavez
 * use my real ip
 *
 * Revision 1.1  2011/01/25 18:31:45  dechavez
 * initial release
 *
 */
