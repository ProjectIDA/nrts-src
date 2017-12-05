#pragma ident "$Id: qio.h,v 1.6 2016/06/15 21:18:28 dechavez Exp $"
/*======================================================================
 *
 * Defines, templates, and prototypes Quanterra physical I/O library
 *
 *====================================================================*/
#ifndef qio_include_defined
#define qio_include_defined

#ifdef __cplusplus
extern "C" {
#endif

/* platform specific stuff */

#include "platform.h"
#include "util.h"
#include "ttyio.h"
#include "udpio.h"
#include "logio.h"

/* Constants used in building the IP and UDP headers */

#define QIO_IP_VERSION  4
#define QIO_IP_HDRLEN  20
#define QIO_IP_DS       0
#define QIO_IP_TTL      0xff
#define QIO_IP_UDP      0x11
#define QIO_UDP_HDRLEN  8
#define QIO_IP_FLAGS    0x40
#define QIO_IP_FRAGOFF  0

#define QIO_DEFAULT_MY_IP 0x7F000001 /* 127.0.0.1 */

/* I/O statistics */

typedef struct {
    MUTEX mutex;        /* for protection */
    struct {
        UINT64 bytes;   /* total number of bytes */
        UINT64 pkts;    /* total number of packets */
    } total;
    struct {
        int max;        /* largest packet */
        int min;        /* smallest packet */
    } len;
    UINT64 tstamp;      /* time of last I/O */
    struct {
        UINT64 count;   /* total number of errors */
        UINT64 tstamp;  /* time of last error */
    } error;
} QIO_STATS;

/* Source and destination addresses */

typedef struct {
    UINT32 ip;            /* IP address */
    UINT16 port;          /* port number */
} QIO_ADDRESS;

/* xmit and receive buffers */

#define QIO_BUFLEN 1500

typedef struct {
    UINT16 ident;          /* for unique idents in the IP header */
    QIO_STATS stats;       /* xmit statistics */
    UINT8 buf[QIO_BUFLEN]; /* xmit buffer */
} QIO_XMTBUF;

typedef struct {
    UTIL_TIMER timer;      /* for detecting input timeouts */
    UINT64 timeout;        /* timeout interval */
    int nread;             /* number of bytes in receive buffer */
    int index;             /* index to next unexamined byte in the buffer */
    QIO_STATS stats;       /* recv statistics */
    UINT8 buf[QIO_BUFLEN]; /* recv buffer */
} QIO_RCVBUF;

/* connection handle */

#define QIO_ERR -1
#define QIO_UDP  1 /* network */
#define QIO_TTY  2 /* serial port */

typedef struct {
    MUTEX mutex;
    int type;  /* QIO_UDP, QIO_TTY */
    union {
        UDPIO  udp; /* QIO_UDP */
        TTYIO *tty; /* QDP_TTY */
    } method;
    QIO_XMTBUF xmit;
    QIO_RCVBUF recv;
    LOGIO *lp;
    QIO_ADDRESS my;   /* my IP address and port number, set once */
    QIO_ADDRESS peer; /* peer's IP address and port number */
    char ident[MAXPATHLEN+1];
#define QIO_DEBUG_OFF       0 /* quiet library */
#define QIO_DEBUG_TERSE     1 /* log addresses and byte counts */
#define QIO_DEBUG_VERBOSE   2 /* log addesses, byte counts and contents */
    int debug;
} QIO;

/* Default attributes for a serial connection */

#define QIO_READ_TO    25 /* read interval (msec) */
#define QIO_WRITE_TO 1000 /* write timeout interval (msec) */

#define QIO_DEFAULT_TTY_ATTR { \
    TRUE,                      \
    QIO_READ_TO,               \
    19200,                     \
    19200,                     \
    TTYIO_PARITY_NONE,         \
    TTYIO_FLOW_HARD,           \
    1,                         \
    0                          \
}

/* SLIP framing and escape characters */

#define QIO_FRM     0xC0
#define QIO_ESC     0xDB
#define QIO_ESC_FRM 0xDC
#define QIO_ESC_ESC 0xDD

/* close.c */
void qioClose(QIO *qio);

/* debug.c */
VOID qioDebug(QIO *handle, int level, char *message, ...);
VOID qioDebugContents(QIO *handle, char *buf, int buflen);

/* init.c */
int qioConnectionType(char *iostr, char *ident, TTYIO_ATTR *attr);
BOOL qioInit(QIO *qio, UINT32 myIP, int myPort, char *iostr, int to, LOGIO *lp, int debug);

/* ip.c */
int qioPackIpHeader(UINT8 *start, QIO *qio, UINT32 destinationIP, int len);
BOOL qioVerifyIpHeader(UINT8 *start);
UINT32 qioIpHeaderSrcIP(UINT8 *start);
UINT32 qioIpHeaderDstIP(UINT8 *start);

/* recv.c */
int qioRecv(QIO *qio, char *buf, int buflen);

/* send.c */
int qioSend(QIO *qio, UINT32 destinationIP, int destinationPort, char *buf, int buflen);

/* stats.c */
void qioInitStats(QIO_STATS *stats);
void qioUpdateStats(QIO_STATS *stats, int nbytes, UINT64 tstamp);

/* udp.c */
int qioPackUdpHeader(UINT8 *start, QIO *qio, UINT32 destinationIP, int destinationPort, UINT8 *payload, int len);
BOOL qioVerifyUdpHeader(UINT8 *start, UINT8 *iphdr, UINT8 *payload, int len);
UINT16 qioUdpHeaderSrcPort(UINT8 *start);
UINT16 qioUdpHeaderDstPort(UINT8 *start);

/* version.c */
VERSION *qioVersion(VOID);
char *qioVersionString(VOID);

#ifdef __cplusplus
}
#endif

#endif

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
 * $Log: qio.h,v $
 * Revision 1.6  2016/06/15 21:18:28  dechavez
 * debug.c prototypes and added 'debug' field to QIO handle
 *
 * Revision 1.5  2014/08/11 18:00:50  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT
 *
 * Revision 1.4  2011/01/31 21:18:05  dechavez
 * changed read timeout to 25 ms
 *
 * Revision 1.3  2011/01/31 21:10:31  dechavez
 * initial release (only tty and UDP I/O tested)
 *
 */
