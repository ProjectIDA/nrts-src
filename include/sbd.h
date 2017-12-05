#pragma ident "$Id: sbd.h,v 1.5 2015/12/04 23:19:43 dechavez Exp $"
#ifndef sbd_include_defined
#define sbd_include_defined

/* platform specific stuff */

#include "platform.h"
#include "logio.h"
#include "list.h"
#include "util.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* limits */

#define SBD_PROTOCOL_1_PREAMBLE_LEN 3

#define SBD_MAX_PAYLOAD  1960
#define SBD_MAX_OVERHEAD 32 /* actually, 31, the MO Header */
#define SBD_MAX_MSGLEN   (SBD_MAX_PAYLOAD + SBD_MAX_OVERHEAD + 64) /* includes 64 bytes of slop */
#define SBD_MINTIMEO     (60 * MSEC_PER_SEC)

#define SBD_IMEI_LEN     15

/* error levels associated with I/O failures */

#define SBD_ERR_NONE      (                     (int) 0)
#define SBD_ERR_TRANSIENT (SBD_ERR_NONE      + (int) 1)
#define SBD_ERR_NONFATAL  (SBD_ERR_TRANSIENT + (int) 1)
#define SBD_ERR_FATAL     (SBD_ERR_NONFATAL  + (int) 1)

/* connection attributes */

typedef struct {
    UINT32 at_timeo;   /* i/o timeout interval */
    UINT32 at_sndbuf;  /* TCP/IP send buffer size */
    UINT32 at_rcvbuf;  /* TCP/IP receive buffer size  */
    UINT32 at_maxerr;  /* largest tolerable I/O error level */
    BOOL   at_retry;   /* if TRUE, retry failed connects */
    UINT32 at_wait;    /* retry interval, msec */
    char  *at_dbgpath; /* path name for debug output */
    UINT32 at_proto;   /* handshake protocol */
} SBD_ATTR;

/* Handle for socket I/O */

#define SBD_DOT_DECIMAL_LEN 15
#define SBD_MAX_PEER_NAME_LEN 63

typedef struct {
    int sd;                         /* socket descriptor */
    time_t connect;                 /* time connection was established */
    int port;                       /* port number */
    struct {
        char addr[INET_ADDRSTRLEN]; /* remote peer IP address */
        char name[MAXPATHLEN];      /* remote peer name */
        char ident[MAXPATHLEN];     /* name:port */
    } peer;
    struct {
        int send;                   /* send I/O error */
        int recv;                   /* recv I/O error */
    } error;
    SBD_ATTR attr;                  /* connection attributes */
    LOGIO *lp;                      /* logging facility handle */
    int debug;                      /* debug level */
    BOOL disabled;                  /* used by server, TRUE when shutting down */
    FILE *dbgfp;                    /* for debugging traffic */
} SBD;

/* SBD Information Elements */

/* Information Element Identifiers */

#define SBD_IEI_MO_HEADER   0x01
#define SBD_IEI_MO_PAYLOAD  0x02
#define SBD_IEI_MO_LOCATION 0x03
#define SBD_IEI_MO_CONFIRM  0x05
#define SBD_IEI_MT_HEADER   0x41
#define SBD_IEI_MT_PAYLOAD  0x42
#define SBD_IEI_MT_CONFIRM  0x44
#define SBD_IEI_MT_PRIORITY 0x46

/* MT disposition flags */

#define SBD_MT_FLAG_NONE           0x00
#define SBD_MT_FLAG_FLUSHQ         0x01
#define SBD_MT_FLAG_SENDRINGALERT  0x02
#define SBD_MT_FLAG_UPDATELOCATION 0x08
#define SBD_MT_FLAG_HIGHPRIORITY   0x10
#define SBD_MT_FLAG_ASSIGNMTMSN    0x20

/* MT message status */

#define SBD_MT_CONFIRM_NO_PAYLOAD             0
#define SBD_MT_CONFIRM_INVALID_IMEI          -1
#define SBD_MT_CONFIRM_UNKNOWN_IMEI          -2
#define SBD_MT_CONFIRM_TOOBIG                -3
#define SBD_MT_CONFIRM_MISSING_PAYLOAD       -4
#define SBD_MT_CONFIRM_MT_QUEUE_FULL         -5
#define SBD_MT_CONFIRM_MT_NO_RESOURCES       -6
#define SBD_MT_CONFIRM_MT_PROTOCOL_ERROR     -7
#define SBD_MT_CONFIRM_RING_ALERTS_DISABLED  -8
#define SBD_MT_CONFIRM_IMEI_UNATTACHED       -9
#define SBD_MT_CONFIRM_SOURCE_IP_REJECTED   -10
#define SBD_MT_CONFIRM_INVALID_MTMSN        -11

/* masks for decoding the MO_LOCATION fields */

#define SBD_MO_LOCATION_MASK_FORMAT 0x0c /* 0 0 0 0 1 1 0 0 */
#define SBD_MO_LOCATION_MASK_NSI    0x02 /* 0 0 0 0 0 0 1 0 */
#define SBD_MO_LOCATION_MASK_EWI    0x01 /* 0 0 0 0 0 0 0 1 */

/* MO and MT payloads have the same structure */

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    UINT16 len;                  /* number of bytes in next field */
    UINT8 data[SBD_MAX_MSGLEN];  /* payload bytes */
} SBD_PAYLOAD;

/* MO Information Elements */

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    UINT32 cdr;                  /* call data record reference number (aka auto ID) */
    char   imei[SBD_IMEI_LEN+1]; /* Iridium equipment identifier */
    UINT8  status;               /* session status byte */
    UINT16 momsn;                /* mobile oriented message sequence number */
    UINT16 mtmsn;                /* mobile terminated message sequence number */
    UINT32 tstamp;               /* session time stamp */
} SBD_MO_HEADER;

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    REAL32 lat;                  /* latitude */
    REAL32 lon;                  /* longitude */
    UINT32 cep;                  /* Circular Error Probablity, km */
} SBD_MO_LOCATION;

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    BOOL status;                 /* TRUE (success) or FALSE (failure) */
} SBD_MO_CONFIRM;

typedef struct {
    SBD_MO_HEADER   header;
    SBD_PAYLOAD     payload;
    SBD_MO_LOCATION location;
    SBD_MO_CONFIRM  confirm;
} SBD_MO;

/* MT Information Elements */

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    UINT32 cdr;                  /* call data record reference number (aka auto ID) */
    UINT8 myid[5];               /* client supplied message ID */
    char  imei[SBD_IMEI_LEN+1];  /* target Iridium equipment identifier */
    UINT16 flags;                /* SBD_MT_FLAG_x bitmask */
} SBD_MT_HEADER;

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    UINT8 myid[5];               /* client supplied message ID */
    char  imei[SBD_IMEI_LEN+1];  /* target Iridium equipment identifier */
    UINT32 cdr;                  /* call data record reference number (aka auto ID) */
    UINT16 status;               /* SBD_MT_STATUS_x value */
} SBD_MT_CONFIRM;

typedef struct {
    BOOL valid;                  /* TRUE when contents are defined */
    UINT16 level;                /* priority level */
} SBD_MT_PRIORITY;

typedef struct {
    SBD_MT_HEADER   header;
    SBD_PAYLOAD     payload;
    SBD_MT_CONFIRM  confirm;
    SBD_MT_PRIORITY priority;
} SBD_MT;

/* SBD DirectIP message (protocol revision 1) */

typedef struct {
    UINT8  protocol;             /* protocol revision number (always 1) */
    UINT16 len;                  /* overall message length (includes 3-byte preamble) */
    UINT8  body[SBD_MAX_MSGLEN]; /* message body (first 3 bytes are the preamble) */
/* Following are filled in by parsing the body */
/* Or the body is filled in by packing the following */
    SBD_MO mo;
    SBD_MT mt;
} SBD_MESSAGE;

/* return codes from message parser */

#define SBD_OK       0 /* parsed OK */
#define SBD_ERROR   -1 /* some other error */
#define SBD_EPROTO  -2 /* unsupported protocol (not 1) */
#define SBD_TOOBIG  -3 /* illegal length field somewhere */
#define SBD_GARBAGE -4 /* message contains gibberish */

/* various defaults */

#define SBD_DEFAULT_SERVER "mcuc.wg.ucsd.edu"
#define SBD_DEFAULT_PORT 10800

#ifndef SBD_DEF_AT_TIMEO
#define SBD_DEF_AT_TIMEO SBD_MINTIMEO 
#endif

#ifndef SBD_DEF_AT_SNDBUF
#define SBD_DEF_AT_SNDBUF 0 /* use OS default */
#endif

#ifndef SBD_DEF_AT_RCVBUF
#define SBD_DEF_AT_RCVBUF 0 /* use OS default */
#endif

#ifndef SBD_DEF_AT_MAXERR
#define SBD_DEF_AT_MAXERR SBD_ERR_NONFATAL
#endif

#ifndef SBD_DEF_AT_RETRY
#define SBD_DEF_AT_RETRY TRUE
#endif

#ifndef SBD_DEF_AT_WAIT
#define SBD_DEF_AT_WAIT 10000
#endif

#ifndef SBD_DEF_AT_DBPATH
#define SBD_DEF_AT_DBPATH NULL
#endif

#define SBD_PROTO_SBD 0
#define SBD_PROTO_NRTS 1

#ifndef SBD_DEF_AT_PROTO
#define SBD_DEF_AT_PROTO SBD_PROTO_SBD
#endif

#ifdef INCLUDE_SBD_DEFAULT_ATTR
static SBD_ATTR SBD_DEFAULT_ATTR = {
    SBD_DEF_AT_TIMEO,
    SBD_DEF_AT_SNDBUF,
    SBD_DEF_AT_RCVBUF,
    SBD_DEF_AT_MAXERR,
    SBD_DEF_AT_RETRY,
    SBD_DEF_AT_WAIT,
    SBD_DEF_AT_DBPATH,
    SBD_DEF_AT_PROTO
};
#endif /* INCLUDE_SBD_DEFAULT_ATTR */

/* function prototypes */

/* accept.c */
SBD *sbdAccept(SBD *server);

/* close.c */
SBD *sbdClose(SBD *sbd);

/* connect.c */
BOOL sbdConnect(SBD *sbd);
BOOL sbdReconnect(SBD *sbd);

/* free.c */
SBD *sbdFree(SBD *sbd);

/* get.c */
time_t sbdGetStartTime(SBD *sbd);
int sbdGetDebug(SBD *sbd);
UINT32 sbdGetTimeoutInterval(SBD *sbd);
SBD_ATTR *sbdGetAttr(SBD *sbd, SBD_ATTR *attr);
UINT32 sbdGetRetryInterval(SBD *sbd);
UINT32 sbdGetMaxError(SBD *sbd);
BOOL sbdGetRetry(SBD *sbd);
BOOL sbdGetDisabled(SBD *sbd);
LOGIO *sbdGetLogio(SBD *sbd);
int sbdGetSendError(SBD *sbd);
int sbdGetRecvError(SBD *sbd);

/* open.c */
SBD *sbdOpen(char *server, int port, SBD_ATTR *attr, LOGIO *lp, int debug);

/* pack.c */
int sbdPackMO_HEADER(UINT8 *start, SBD_MO_HEADER *src);
int sbdPackMO_LOCATION(UINT8 *start, SBD_MO_LOCATION *src);
int sbdPackMO_CONFIRM(UINT8 *start, SBD_MO_CONFIRM *src);
int sbdPackMT_HEADER(UINT8 *start, SBD_MT_HEADER *src);
int sbdPackMT_CONFIRM(UINT8 *start, SBD_MT_CONFIRM *src);
int sbdPackMT_PRIORITY(UINT8 *start, SBD_MT_PRIORITY *src);
int sbdPackPayload(UINT8 *start, SBD_PAYLOAD *src, int which);
#define sbdPackMO_PAYLOAD(start, src) sbdPackPayload(start, src, SBD_IEI_MO_PAYLOAD)
#define sbdPackMT_PAYLOAD(start, src) sbdPackPayload(start, src, SBD_IEI_MT_PAYLOAD)
int sbdPackMessage(SBD_MESSAGE *message);

/* parse.c */
int sbdParseMessageBody(SBD_MESSAGE *message);

/* print.c */
void sbdPrintMessageIE(FILE *fp, SBD_MESSAGE *message);

/* send.c */
BOOL sbdSendMessage(SBD *sbd, SBD_MESSAGE *message);

/* server.c */
SBD *sbdServer(int port, SBD_ATTR *user_attr, LOGIO *lp, int debug);

/* set.c */
VOID sbdSetDebug(SBD *sbd, int value);
VOID sbdSetLogio(SBD *sbd, LOGIO *logio);
VOID sbdSetRetryFlag(SBD *sbd, BOOL value);
VOID sbdSetDisabled(SBD *sbd, BOOL value);

/* read.c */
BOOL sbdReadMessage(gzFile *gz, SBD_MESSAGE *message);

/* recv.c */
BOOL sbdRecvMessage(SBD *sbd, SBD_MESSAGE *message);

/* unpack.c */
int sbdUnpackMO_HEADER(UINT8 *start, SBD_MO_HEADER *dest);
int sbdUnpackMO_LOCATION(UINT8 *start, SBD_MO_LOCATION *dest);
int sbdUnpackMO_CONFIRM(UINT8 *start, SBD_MO_CONFIRM *dest);
int sbdUnpackMT_HEADER(UINT8 *start, SBD_MT_HEADER *dest);
int sbdUnpackMT_CONFIRM(UINT8 *start, SBD_MT_CONFIRM *dest);
int sbdUnpackMT_PRIORITY(UINT8 *start, SBD_MT_PRIORITY *dest);
int sbdUnpackPayload(UINT8 *start, SBD_PAYLOAD *dest);

/* util.c */
VOID sbdInitHandle(SBD *sbd, char *peer, int port, SBD_ATTR *user_attr, LOGIO *lp, int debug);
int sbdErrorLevel(int ErrorCode);
char *sbdPeerIdent(SBD *sbd);
char *sbdMaxerrString(UINT32 value);
VOID sbdPrintAttr(FILE *fp, SBD_ATTR *attr);
VOID sbdSetSendError(SBD *sbd, int value);
VOID sbdSetRecvError(SBD *sbd, int value);
void sbdClearMessageIEIs(SBD_MESSAGE *message);
VOID sbdClearMessage(SBD_MESSAGE *message);

/* version.c */
char *sbdVersionString(VOID);
VERSION *sbdVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: sbd.h,v $
 * Revision 1.5  2015/12/04 23:19:43  dechavez
 * defined SBD_DEFAULT_SERVER
 *
 * Revision 1.4  2013/03/15 21:33:15  dechavez
 * added SBD_MO_LOCATION_MASK_x defines, SBD_IMEI_LEN, tidied up comments
 *
 * Revision 1.3  2013/03/13 21:24:25  dechavez
 * introduced SBD_MO_LOCATION, SBD_MO_CONFIRM, SBD_MT_PRIORITY, SBD_MO and SBD_MT
 *
 * Revision 1.2  2013/03/11 22:55:05  dechavez
 * added SBD_MT_CONFIRM support, fixed SBD_MINTIMEO
 *
 * Revision 1.1  2013/03/07 21:00:20  dechavez
 * initial release
 *
 */
