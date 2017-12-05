#pragma ident "$Id: recv.c,v 1.6 2016/08/01 22:01:08 dechavez Exp $"
/*======================================================================
 * 
 * Receive a QDP packet
 *
 *====================================================================*/
#include "qio.h"

static BOOL GetNextRecvdChar(QIO *qio, UINT8 *value)
{
    if (qio->recv.index == qio->recv.nread) {
        qio->recv.index = qio->recv.nread = 0;
        while (qio->recv.nread == 0) {
            if (utilElapsedTime(&qio->recv.timer) > qio->recv.timeout) {
                errno = ETIMEDOUT;
                return FALSE;
            }
            qio->recv.nread = ttyioRead(qio->method.tty, qio->recv.buf, QIO_BUFLEN);

            if (qio->recv.nread < 0) return FALSE;
        }
    }

    *value = qio->recv.buf[qio->recv.index++];
    return TRUE;
}

static int RecvSlipFrame(QIO *qio, UINT8 *output, int buflen)
{
#define QIO_UDP_LEN_OFFSET 4
BOOL done, EscapeInProgress;
UINT16 udplen;
int len, nbyte, tossed;
UINT8 *udphdr, *payload, value;
UINT8 frame[QIO_BUFLEN];

/* Clear the receive buffer the first time, and after errors */

    if (qio->recv.nread < 0) qio->recv.nread = qio->recv.index = 0;

/* Start the timeout clock */

    utilResetTimer(&qio->recv.timer);

/* Read a valid frame or timeout trying */

    while (1) {

    /* Toss everything before the first framing character */

        tossed = -1;
        do {
            if (!GetNextRecvdChar(qio, &value)) return (errno == ETIMEDOUT) ? 0 : -1;
            ++tossed;
        } while (value != QIO_FRM);

    /* Keep everything until the next framing character */

        nbyte = 0;
        done  = EscapeInProgress = FALSE;
        while (!done) {
            if (nbyte == QIO_BUFLEN) {
                errno = EOVERFLOW;
                return -1;
            }
            if (!GetNextRecvdChar(qio, &value)) return errno == ETIMEDOUT ? 0 : -1;
            if (EscapeInProgress) {
                if (value == QIO_ESC_FRM) {
                    frame[nbyte++] = QIO_FRM;
                } else if (value == QIO_ESC_ESC) {
                    frame[nbyte++] = QIO_ESC;
                } else {
                    errno = EPROTO;
                    return -1;
                }
                EscapeInProgress = FALSE;
            } else {
                switch (value) {
                  case QIO_FRM:
                    done = TRUE;
                    break;
                  case QIO_ESC:
                    EscapeInProgress = TRUE;
                    break;
                  default:
                    frame[nbyte++] = value;
                }
            }
        }

    /* Verify checksums */

        if (!qioVerifyIpHeader(frame)) continue;

        udphdr = frame + QIO_IP_HDRLEN;

        payload = udphdr + QIO_UDP_HDRLEN;
        utilUnpackUINT16(&udphdr[QIO_UDP_LEN_OFFSET], &udplen); /* UDP header + payload length */
        len = udplen - QIO_UDP_HDRLEN; /* payload length */

        if (!qioVerifyUdpHeader(udphdr, frame, payload, len)) continue;

    /* Save peer address as taken from the IP and UDP headers */

        qio->peer.ip = qioIpHeaderSrcIP(frame);
        qio->peer.port = qioUdpHeaderSrcPort(udphdr);

    /* Fill the output buffer */

        memcpy(output, payload, len);

    /* return payload length */

        return len;
    }
}

int qioRecv(QIO *qio, char *buf, int buflen)
{
int nbyte;
char strbuf[MAXPATHLEN];
static char *fid = "qioRecv";

/* For UDP I/O we just read the next packet off the wire. */
/* For the others, we have to deal with SLIP framing and the IP and UDP headers. */

    if (qio->type == QIO_UDP) {
        nbyte = udpioRecv(&qio->method.udp, buf, buflen);
        qio->peer.ip = ntohl(qio->method.udp.peer.addr.sin_addr.s_addr);
        qio->peer.port = ntohs(qio->method.udp.peer.addr.sin_port);
    } else {
        nbyte = RecvSlipFrame(qio, (UINT8 *) buf, buflen);
    }
#ifdef ENABLE_QIO_DEBUG_MESSAGES
    qioDebug(qio, QIO_DEBUG_TERSE, "%s: %d bytes from %s:%d\n",
        fid,
        nbyte,
        utilDotDecimalString(qio->peer.ip, strbuf),
        qio->peer.port
    );
    qioDebugContents(qio, buf, nbyte);
#endif /* ENABLE_QIO_DEBUG_MESSAGES */
    if (nbyte > 0) errno = 0;

    qioUpdateStats(&qio->recv.stats, nbyte, utilTimeStamp());
    return nbyte;
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
 * $Log: recv.c,v $
 * Revision 1.6  2016/08/01 22:01:08  dechavez
 * make low-level debug calls visible only if ENABLE_QIO_DEBUG_MESSAGES defined
 *
 * Revision 1.5  2016/06/15 21:22:13  dechavez
 * handle based debug message support added
 *
 * Revision 1.4  2015/12/04 23:02:38  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.3  2014/08/11 18:01:20  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.2  2011/01/31 18:17:25  dechavez
 * removed debug statements
 *
 * Revision 1.1  2011/01/25 18:31:02  dechavez
 * checkpoint.  QIO_UDP OK, still debugging QIO_DIG and QIO_TTY
 *
 */
