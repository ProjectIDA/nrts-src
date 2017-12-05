#pragma ident "$Id: sbds.h,v 1.3 2013/05/11 23:07:38 dechavez Exp $"
#ifndef sbd_h_included
#define sbd_h_included

#include "sbd.h"
#include "util.h"
#include "logio.h"
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2013 - Regents of the University of California.";

#define STDIN_PORT 0xffff /* for testing */

/* various default parameters */

#define DEFAULT_DAEMON         FALSE
#define DEFAULT_USER           "nrts"
#define DEFAULT_PORT           SBD_DEFAULT_PORT
#define DEFAULT_MAXCLIENT      128
#define DEFAULT_BUFLEN         32768
#define DEFAULT_RECVBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_SENDBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_BACKGROUND_LOG "syslogd:local3"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_TIMEOUT        SBD_MINTIMEO
#define DEFAULT_POLL_INTERVAL  1000
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif
#define DEFAULT_SEEDLINK       TRUE

#define SBDS_INI_NAME       "sbds.ini"
#define SBDS_ENV_INI        "SBDS_INI"

#ifdef WIN32
#define SBDS_WIN32_INI_NAME "SBDS.INI"
#endif /* WIN32 */

/* Module Id's for generating meaningful exit codes */

#define SBDS_MOD_MAIN     ((INT32) 100)
#define SBDS_MOD_CLIENT   ((INT32) 200)
#define SBDS_MOD_INIT     ((INT32) 300)
#define SBDS_MOD_LOG      ((INT32) 400)
#define SBDS_MOD_LOADINI  ((INT32) 500)
#define SBDS_MOD_DISKLOOP ((INT32) 600)
#define SBDS_MOD_SIGNALS  ((INT32) 700)

/* Status flags for servicing data requests */

#define SBDS_DONE              0
#define SBDS_WAITING_FOR_DATA  1
#define SBDS_DATA_READY        2
#define SBDS_CORRUPT_DISK_LOOP 3
#define SBDS_ABORT_REQUEST     4

/* status of a connection */

#define SBDS_STATUS_IDLE         0 /* unused CLIENT */
#define SBDS_STATUS_RECV         1 /* server is receiving stuff from the client */
#define SBDS_STATUS_SEND         2 /* server is sending stuff to the client */
#define SBDS_STATUS_FINISHED     3 /* we are done */
#define SBDS_STATUS_SERVER_ABORT 4 /* server abort */
#define SBDS_STATUS_CLIENT_ABORT 5 /* client aborted */

/* For managing client connections */

typedef struct {
    MUTEX  mutex;              /* for protection */
    UINT32 index;              /* index number for debugging use */
    SBD *sbd;                  /* client handle */
    char *ident;               /* client id string */
    int  status;               /* SBDS_STATUS_FINISHED when all done with this client */
    SBD_MESSAGE send;          /* space for message to be sent */
    SBD_MESSAGE recv;          /* space for message to be received */
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
    SBD_ATTR  attr;        /* connection attributes */
    ISI_GLOB glob;         /* database stuff for ISI/NRTS */
    struct {
        UINT32 send;
        UINT32 recv;
    } buflen;               /* maximum send/receive buffer lengths */
    char *site;             /* SBD disk loop site name */
    BOOL seedlink;          /* SeedLink enable flag */
} PARAM;

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/* Function prototypes */

/* addoss.c */
void ProcessADDOSS(SBD_MESSAGE *message);
void InitADOSSprocessor(PARAM *par);

/* client.c */
void CloseClientConnection(CLIENT *client);
CLIENT *NextAvailableClient(SBD *sbd);
void InitClientList(PARAM *par);

/* dlmgr.c */
void CloseAllDiskLoops(void);
ISI_DL *GetIDA10dl(char *site, char *net, BOOL seedlink);
void InitDLmgr(PARAM *par);

/* exit.c */
void BlockShutdown(char *fid);
void UnlockShutdown(char *fid);
void BlockOnShutdown(void);
BOOL ShutdownInProgress(void);
void GracefulExit(INT32 status);
void InitGracefulExit(void);

/* init.c */
SBD *Init(char *myname, PARAM *par);

/* log.c */
LOGIO *LogPointer(void);
void LogMsgLevel(int level);
void LogMsg(int level, char *format, ...);
void HexDump(int level, UINT8 *data, int len);
BOOL InitLogging(char *myname, PARAM *par);

/* main.c */
void DisableNewConnections(void);

/* par.c */
PARAM *LoadPar(char *prog, int argc, char **argv);
void LogPar(PARAM *par);

/* service.c */
void LogRecvError(SBD *sbd);
void BreakNewConnection(SBD *sbd);
void ServiceConnection(SBD *sbd);

/* signals.c */
void StartSignalHandler(void);

/* write.c */
BOOL SaveMessage(SBD_MESSAGE *message);
ISI_DL *InitSBDdl(PARAM *par);

#endif /* sbd_h_included */

/* Revision History
 *
 * $Log: sbds.h,v $
 * Revision 1.3  2013/05/11 23:07:38  dechavez
 * various changes to support the addition of ADDOSS/IDA10 support
 *
 * Revision 1.2  2013/03/13 21:34:43  dechavez
 * updated prototypes
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
