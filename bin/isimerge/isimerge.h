#pragma ident "$Id: isimerge.h,v 1.3 2015/12/18 18:51:39 dechavez Exp $"
#ifndef isimerge_h_included
#define isimergeh_included

#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"
#include "ida.h"
#include "logio.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2010 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define ISIMERGE_MOD_MAIN    ((INT32) 100)
#define ISIMERGE_MOD_DATA    ((INT32) 200)
#define ISIMERGE_MOD_EXIT    ((INT32) 300)
#define ISIMERGE_MOD_LOG     ((INT32) 400)
#define ISIMERGE_MOD_SAVE    ((INT32) 500)
#define ISIMERGE_MOD_IO      ((INT32) 600)
#define ISIMERGE_MOD_META    ((INT32) 700)
#define ISIMERGE_MOD_SIGNALS ((INT32) 900)

/* default parameters */

#define DEFAULT_USER           "nrts"
#define DEFAULT_DAEMON         FALSE
#define DEFAULT_SOURCE         SOURCE_UNDEFINED
#define DEFAULT_HOME           "/usr/nrts"
#define DEFAULT_TIMEOUT        30
#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_DBGPATH        NULL
#ifndef HAVE_MYSQL
#define DEFAULT_DB             "/usr/nrts"
#else
#define DEFAULT_DB             "localhost:nrts:NULL:ida:0"
#endif
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif
#define DEFAULT_SNDBUF 65536
#define DEFAULT_RCVBUF 65536

#define MINIMUM_COUNT 2 /* minimum number of remote sites */

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/* For passing parameters to the data threads */

typedef struct {
    char name[ISI_SITELEN+1]; /* remote site name */
    char server[MAXPATHLEN+1];/* remote server name or ip */
    char mpath[MAXPATHLEN+1]; /* path name of meta data file tracking source seqnos */
    FILE *mfp;                /* mpath, opened for update */
    ISI_SEQNO beg;            /* start of data specifier */
    ISI_SEQNO end;            /* end   of data specifier */
    ISI_SEQNO last;           /* original sequence number of most recent packet */
    ISI_SEQNO next;           /* original sequence number to request next */
    ISI_PARAM isi_param;      /* connection parameters */
    SEMAPHORE semaphore;      /* for synchronization */
    UINT8 *buf;               /* for decompressing packets */
    ISI_RAW_PACKET raw;       /* incoming data */
    BOOL first;               /* TRUE until data are received */
} SITE_PAR;

/*  Function prototypes  */

/* bground.c */
BOOL BackGround(void);

/* data.c */
void InitDataThreadMutex(void);
BOOL QueueSiteThread(LNKLST *head, char *srcstr);

/* dl.c */
void LockSaveMutex(void);
void ReleaseSaveMutex(void);
void UpdateParentID(void);
BOOL UpdateSitePar(SITE_PAR *site);
void SavePacket(SITE_PAR *par, ISI_RAW_PACKET *raw);
BOOL InitSavePacket(ISI_GLOB *glob, char *site, LOGIO *lp);

/* exit.c */
int ExitStatus(void);
BOOL InitExitHandler(void);
void Abort(int value);

/* init.c */
void InitializeSitePar(SITE_PAR *site);

/* io.c */
ISI *ConnectToServer(SITE_PAR *site, char *ident);
ISI *CloseConnection(ISI *isi, char *ident, SITE_PAR *par);
int ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw, UINT8 *buf, UINT32 buflen);

/* log.c */
LOGIO *LogHandle(void);
void LogMsgLevel(int level);
void LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* main.c */
void LockGlobalMutex(void);
void ReleaseGlobalMutex(void);
void WakeMainThread(void);

/* meta.c */
BOOL SetMetaStartSeqno(SITE_PAR *site);
void SaveLastSeqno(SITE_PAR *site);

/* parse.c */
BOOL ParseSourceString(char *string, SITE_PAR *site);

/* seqno.c */
void UpdateSequenceNumbers(ISI_SEQNO *new, ISI_RAW_PACKET *raw);

/* signals.c */
void StartSignalHandler(void);

#endif /* isidl_h_included */

/* Revision History
 *
 * $Log: isimerge.h,v $
 * Revision 1.3  2015/12/18 18:51:39  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.2  2010/09/17 20:04:22  dechavez
 * reordered stuff in failed attempt to figure out why the threads hang when run as a daemon,
 * changed site@server specification to eliminate the isi= prefix
 *
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
