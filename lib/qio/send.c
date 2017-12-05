#pragma ident "$Id: send.c,v 1.6 2016/08/01 22:01:08 dechavez Exp $"
/*======================================================================
 * 
 * Send a QDP packet
 *
 *====================================================================*/
#include "qio.h"

static int InsertChar(UINT8 *start, UINT8 value)
{
UINT8 *ptr;

    ptr = start;
    switch (value) {
      case QIO_FRM: *ptr++ = QIO_ESC; *ptr++ = QIO_ESC_FRM; break;
      case QIO_ESC: *ptr++ = QIO_ESC; *ptr++ = QIO_ESC_ESC; break;
      default: *ptr++ = value;
    }

    return (int) (ptr - start);
}

static int BuildXmitFrame(QIO *qio, UINT32 destinationIP, int destinationPort, UINT8 *payload, int len)
{
int i;
UINT8 *ptr;
UINT8  iphdr[QIO_IP_HDRLEN];
UINT8 udphdr[QIO_UDP_HDRLEN];

    qioPackIpHeader(iphdr, qio, destinationIP, len);
    qioPackUdpHeader(udphdr, qio, destinationIP, destinationPort, payload, len);

    ptr = qio->xmit.buf;
    *ptr++ = QIO_FRM;
    for (i = 0; i < QIO_IP_HDRLEN; i++) ptr += InsertChar(ptr, iphdr[i]);
    for (i = 0; i < QIO_UDP_HDRLEN; i++) ptr += InsertChar(ptr, udphdr[i]);
    for (i = 0; i < len; i++) ptr += InsertChar(ptr, payload[i]);
    *ptr++ = QIO_FRM;

    return (int) (ptr - qio->xmit.buf);
}

int qioSend(QIO *qio, UINT32 destinationIP, int destinationPort, char *buf, int buflen)
{
int nbyte, sent;
char strbuf1[MAXPATHLEN], strbuf2[MAXPATHLEN];
static char *fid = "qioSend";

#ifdef ENABLE_QIO_DEBUG_MESSAGES
    qioDebug(qio, QIO_DEBUG_TERSE, "%s: %d bytes from %s:%d to %s:%d\n",
        fid,
        buflen,
        utilDotDecimalString(qio->my.ip, strbuf1),
        qio->my.port,
        utilDotDecimalString(destinationIP, strbuf2),
        destinationPort
    );
    qioDebugContents(qio, buf, buflen);
#endif /* ENABLE_QIO_DEBUG_MESSAGES */

/* For UDP I/O we just send the next packet as is. */
/* For the others, we have to deal with SLIP framing and the IP and UDP headers. */

    if (qio->type == QIO_UDP) {
        sent = udpioSend(&qio->method.udp, destinationIP, destinationPort, buf, buflen);
    } else {
        nbyte = BuildXmitFrame(qio, destinationIP, destinationPort, (UINT8 *) buf, buflen);
        sent = ttyioWrite(qio->method.tty, qio->xmit.buf, nbyte);
        if (sent == nbyte) sent = buflen;
    }

    qioUpdateStats(&qio->recv.stats, sent, utilTimeStamp());
    return sent;
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
 * $Log: send.c,v $
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
 * Revision 1.2  2011/01/31 21:18:31  dechavez
 * removed debugging statements
 *
 * Revision 1.1  2011/01/25 18:36:29  dechavez
 * checkpoint.  QIO_UDP OK, still debugging QIO_DIG and QIO_TTY
 *
 */
