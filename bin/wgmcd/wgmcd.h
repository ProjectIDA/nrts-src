#pragma ident "$Id: wgmcd.h,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
#ifndef wgmcd_h_included
#define wgmcd_h_included

#include "wg.h"
#include "isi/dl.h"
#include "iacp.h"
#include "util.h"
#include "logio.h"
#include "list.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2012 - Regents of the University of California.";

/* various default parameters */

#define DEFAULT_DAEMON         FALSE
#define DEFAULT_USER           "nrts"
#define DEFAULT_PORT           WG_DEFAULT_PORT
#define DEFAULT_MAXCLIENT      128
#define DEFAULT_BUFLEN         32768
#define DEFAULT_RECVBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_SENDBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_BACKGROUND_LOG "syslogd:local3"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_TIMEOUT        IACP_MINTIMEO
#define DEFAULT_POLL_INTERVAL  1000
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif
#define DEFAULT_ECHO_PORT      0 /* disabled by default */

#define WGMCD_INI_NAME       "wgmcd.ini"
#define WGMCD_ENV_INI        "WGMCD_INI"

#ifdef WIN32
#define WGMCD_WIN32_INI_NAME "WGMCD.INI"
#endif /* WIN32 */

/* Module Id's for generating meaningful exit codes */

#define WGMCD_MOD_MAIN     ((INT32) 100)
#define WGMCD_MOD_CLIENT   ((INT32) 200)
#define WGMCD_MOD_INIT     ((INT32) 300)
#define WGMCD_MOD_LOG      ((INT32) 400)
#define WGMCD_MOD_LOADINI  ((INT32) 500)
#define WGMCD_MOD_REPORT   ((INT32) 600)
#define WGMCD_MOD_SIGNALS  ((INT32) 700)
#define WGMCD_MOD_ECHO     ((INT32) 800)

/* Status flags for servicing data requests */

#define WGMCD_DONE              0
#define WGMCD_WAITING_FOR_DATA  1
#define WGMCD_DATA_READY        2
#define WGMCD_CORRUPT_DISK_LOOP 3
#define WGMCD_ABORT_REQUEST     4

/* status of a connection */

#define WGMCD_STATUS_IDLE         0 /* unused CLIENT */
#define WGMCD_STATUS_RECV         1 /* server is receiving stuff from the client */
#define WGMCD_STATUS_SEND         2 /* server is sending stuff to the client */
#define WGMCD_STATUS_FINISHED     3 /* we are done */
#define WGMCD_STATUS_SERVER_ABORT 4 /* server abort */
#define WGMCD_STATUS_CLIENT_ABORT 5 /* client aborted */

/* For managing client connections */

typedef struct {
    MUTEX  mutex;              /* for protection */
    UINT32 index;              /* index number for debugging use */
    IACP *iacp;                /* client handle */
    char *ident;               /* client id string */
    int  status;               /* TRUE when all done with this client */
    UINT32 result;             /* what to tell the client when finished */
    BOOL brkrcvd;              /* TRUE when client has sent us a break */
    BOOL dataready;            /* TRUE when disk loop has data available to send */
    struct {
        UINT32 poll;           /* data ready poll interval, msec */
        UINT32 hbeat;          /* heartbeat interval, sec */
    } interval;
    struct {
        IACP_FRAME frame;
        UINT8 *buf;
        UINT32 buflen;
    } send, recv, temp;        /* output, input and compressed data */
    ISI_GLOB *glob;            /* inherited from PARAM */
    ISI_DL *dl;                /* for storing incoming packets */
} CLIENT;

/* Run time parameters */

typedef struct param {
    BOOL daemon;           /* if TRUE, run in the background */
    UINT16 port;           /* TCP/IP connection port (cmnd and data) */
    UINT32 maxclient;      /* max number of simultaneous clients */
    int priority;          /* process priority */
    char *log;             /* log specifier */
    char *user;            /* run as user */
    LOGIO lp;              /* log handle */
    BOOL debug;            /* debug flag */
    IACP_ATTR  attr;       /* connection attributes */
    ISI_DL_MASTER *master; /* all the ISI and NRTS disk loops */
    ISI_GLOB glob;         /* database stuff for ISI/NRTS */
    struct {
        UINT32 send;
        UINT32 recv;
    } buflen;               /* maximum send/receive buffer lengths */
    UINT16 echo;            /* if non-zero, port for the IACP echo service */
} PARAM;

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/* Function prototypes */

/* client.c */
VOID CloseClientConnection(CLIENT *client);
CLIENT *NextAvailableClient(IACP *iacp);
VOID InitClientList(PARAM *par);

/* echo.c */
VOID StartEchoServer(PARAM *par);

/* exit.c */
void BlockShutdown(char *fid);
void UnlockShutdown(char *fid);
VOID BlockOnShutdown(VOID);
BOOL ShutdownInProgress(VOID);
VOID GracefulExit(INT32 status);
VOID InitGracefulExit(VOID);

/* init.c */
IACP *Init(char *myname, PARAM *par);

/* log.c */
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
BOOL InitLogging(char *myname, PARAM *par);

/* main.c */
VOID DisableNewConnections(VOID);

/* par.c */
PARAM *LoadPar(char *prog, int argc, char **argv);
VOID LogPar(PARAM *par);

/* recv.c */
VOID Process_WG_IACP_WGID(CLIENT *client, char *site);
VOID Process_WG_IACP_OBSPKT(CLIENT *client, WG_WGOBS1 *wgobs1);

/* send.c */
VOID SendOutboundCommands(CLIENT *client);

/* service.c */
VOID LogRecvError(IACP *iacp);
VOID BreakNewConnection(IACP *iacp, UINT32 cause);
VOID ServiceConnection(IACP *iacp);

/* service.c */
VOID LogRecvError(IACP *iacp);
VOID BreakNewConnection(IACP *iacp, UINT32 cause);
VOID ServiceConnection(IACP *iacp);

/* signals.c */
VOID StartSignalHandler(VOID);

/* util.c */
BOOL GetNextSeqno(ISI_DL *dl, WG_OBSPKT_SEQNO *seqno);

#endif /* wgmcd_h_included */

/* Revision History
 *
 * $Log: wgmcd.h,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
