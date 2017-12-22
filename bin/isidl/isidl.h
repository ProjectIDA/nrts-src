#pragma ident "$Id: isidl.h,v 1.48 2016/08/26 20:19:37 dechavez Exp $"
#ifndef isidl_h_included
#define isidl_h_included

#define INCLUDE_APP_SUPPLIED_WRITER
#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"
#include "ida.h"
#include "logio.h"
#include "ttyio.h"
#include "paro.h"
#include "msgq.h"
#include "q330.h"
#include "qdp.h"
#include "qdplus.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2016 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define ISIDL_MOD_MAIN      ((INT32)  100)
#define ISIDL_MOD_INIT      ((INT32)  200)
#define ISIDL_MOD_LOG       ((INT32)  300)
#define ISIDL_MOD_SIGNALS   ((INT32)  400)
#define ISIDL_MOD_ISI       ((INT32)  500)
#define ISIDL_MOD_IDA10     ((INT32)  600)
#define ISIDL_MOD_MSEED     ((INT32)  700)
#define ISIDL_MOD_Q330      ((INT32)  800)
#define ISIDL_MOD_QDP       ((INT32)  900)
#define ISIDL_MOD_QDPLUS    ((INT32) 1000)
#define ISIDL_MOD_META      ((INT32) 1100)
#define ISIDL_MOD_LISS      ((INT32) 1200)
#define ISIDL_MOD_LOCAL     ((INT32) 1300)
#define ISIDL_MOD_INTERCEPT ((INT32) 1400)
#define ISIDL_MOD_NET       ((INT32) 1500)

/* Data source enums */

#define SOURCE_UNDEFINED  0
#define SOURCE_ISI        1
#define SOURCE_Q330       2
#define SOURCE_LISS       3

/* For handling locally acquired packets */

typedef struct {
    MUTEX mutex;
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
    ISI_DL_OPTIONS *options;
} LOCALPKT;

/* Quanterra Q330 support */

typedef struct {
    MUTEX mutex;
    Q330_CFG *cfg;
    QDP_PAR par;
    LOGIO *lp;
    QDP *qp;
    BOOL first;
    LOCALPKT local;
    struct {
        UINT64 dt_data;    /* time most recent DT_DATA was received */
        UINT64 c1_stat;    /* time most recent C1_STAT was received */
        UINT64 other;      /* time most recent "other" was received (neither DT_DATA nor C1_STAT) */
    } tstamp;
    UINT32 retry;     /* registration retry interval */
    UINT32 watchdog;  /* maximum msecs of no data before restarting connection */
} Q330;

typedef struct {
    QDP_LCQ lcq;
    int dummy;
} INTERCEPT_BUFFER;

/* Run time parameters */

typedef struct {
    int  source;        /* source identifier */
    char *input;        /* input descriptor string */
    int timeout;        /* I/O timeout, seconds */
    LOGIO *lp;          /* logging handle */
    struct {
        char *remote;   /* remote site string */
        char *local;    /* local site string */
    } site;
    char *netid;        /* netid string (for MiniSEED) */
    char *begstr;       /* optional beg= string */
    char *endstr;       /* optional end= string */
    int nsite;          /* number of disk loop handles to follow */
    ISI_DL **dl;        /* disk loop handle(s) */
    BOOL *first;        /* to track incoming packets */
    NRTS_TOLERANCE tol; /* time tag tolerances */
    char *dbgpath;      /* IACP debug dump path */
    char *cfgpath;      /* Q330 configuration file */
    LNKLST q330;        /* zero or more Q330's */
    int sndbuf;         /* ISI/IACP socket send buffer length */
    int rcvbuf;         /* ISI/IACP socket recv buffer length */
    UINT32 compress;    /* ISI/IACP telemetry compression */
    UINT16 flags;       /* option flags */
#define ISIDL_FLAG_INTERCEPT       0x0001
    INTERCEPT_BUFFER ib; /* used when ISIDL_FLAG_INTERCEPT bit set */
    struct {
#define SCL_UNSPECIFIED 0
#define SCL_FROM_NRTS   1
#define SCL_FROM_DB     2
#define SCL_FROM_FILE   3
        int source;      /* source type */
        char *spec;      /* specifier for loading ISI stream control list */
        LNKLST *list;    /* stream control list */
    } scl;
    int net;             /* TCP/IP port for accepting 3rd party packets */
    ISI_DL_OPTIONS options; /* ISI disk loop I/O options */
    struct {
        LNKLST *chan;    /* IDA10_NAME_MAP for channel name changes */
    } rename;
} ISIDL_PAR;

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
#define DEFAULT_QDEPTH         10
#define DEFAULT_SEQNO          1
#define DEFAULT_QCODE          'R'
#define DEFAULT_NETID          "II"
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

#define DEFAULT_BAUD   19200
#define DEFAULT_PARITY TTYIO_PARITY_NONE
#define DEFAULT_FLOW   TTYIO_FLOW_SOFT
#define DEFAULT_SBITS  2

#define DEFAULT_SNDBUF 65536
#define DEFAULT_RCVBUF 65536
#define DEFAULT_COMPRESS ISI_COMP_UNDEF

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/*  Function prototypes  */

/* bground.c */
BOOL BackGround(ISIDL_PAR *par);
BOOL BackGround(ISIDL_PAR *par);

/* exit.c */
void BlockShutdown(char *fid);
void UnblockShutdown(char *fid);
INT32 ExitStatus();
void SetExitStatus(INT32 status);
void Hold(INT32 status);
void Exit(INT32 status);
VOID InitExit(ISIDL_PAR *ptr);

/* help.c */
void help(char *myname);

/* ida10.c */
void CompleteIda10Header(ISI_RAW_PACKET *raw, ISI_DL *dl);
THREAD_FUNC Ida10BarometerThread(void *argptr);

/* init.c */
ISIDL_PAR *init(char *myname, int argc, char **argv);

/* intercept.c */
void InterceptProcessor(ISI_RAW_PACKET *raw, ISI_DL *dl, INTERCEPT_BUFFER *ib);
BOOL InitInterceptBuffer(INTERCEPT_BUFFER *ib);

/* isi.c */
void ToggleISIDebugState();
void StartIsiReader(ISIDL_PAR *par);

/* liss.c */
BOOL ParseLissArgs(char *string);
void StartLissReader(ISIDL_PAR *par);

/* local.c */
void ProcessLocalData(LOCALPKT *local);
BOOL InitLocalProcessor(ISIDL_PAR *par);

/* log.c */
LOGIO *GetLogHandle();
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* meta.c */
void ProcessMetaData(Q330 *q330, QDP_META *meta);
BOOL InitMetaProcessor(ISIDL_PAR *par);

/* mseed.c */
//void CloseMSEED(QCAL *qcal);
//void GenerateMSEED(UINT8 *ida10);
//BOOL OpenMSEED(QCAL *qcal);

/* q330.c */
void ToggleQ330DebugState(void);
void SaveQ330Packet(void *args, QDP_PKT *pkt);
void SaveQ330Meta(void *args, QDP_META *meta);
char *AddQ330(ISIDL_PAR *par, char *argstr, char *root);
void StartQ330Readers(ISIDL_PAR *par);

/* qdplus.c */
void InsertQdplusSerialno(ISI_RAW_PACKET *raw, UINT64 serialno);
void CompleteQdplusHeader(ISI_RAW_PACKET *raw, ISI_DL *dl);
void CopyQDPToQDPlus(ISI_RAW_PACKET *raw, QDP_PKT *src);

/* scl.c */
BOOL BuildStreamControlList(ISIDL_PAR *par);

/* signals.c */
VOID StartSignalHandler(VOID);
void SignalHandler(int sig);
VOID StartSignalHandler(VOID);

/* write.c */
BOOL WritePacketToDisk(ISI_DL *dl, ISI_RAW_PACKET *raw);

#endif /* isidl_h_included */

/* Revision History
 *
 * $Log: isidl.h,v $
 * Revision 1.48  2016/08/26 20:19:37  dechavez
 * replaced tstamp with a structure that tracks time since most recent
 * DT_DATA, C1_STAT and "other" packets were received
 *
 * Revision 1.47  2016/08/04 22:08:08  dechavez
 * added local.c prototypes which have been missing for a long time
 *
 * Revision 1.46  2016/02/12 21:56:27  dechavez
 * removed old code related to barometer, ars, and pre-IDA10 packet support
 *
 * Revision 1.45  2015/12/18 18:51:36  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.44  2015/12/09 18:34:04  dechavez
 * cleaned up prototypes
 *
 * Revision 1.43  2014/08/28 22:12:07  dechavez
 * removed INCLUDE_Q330 conditionals
 *
 * Revision 1.42  2014/08/28 22:07:43  dechavez
 * added WritePacketToDisk() prototype
 *
 * Revision 1.41  2012/12/13 21:52:08  dechavez
 * make ISIDL_PAR site a struct with local and remote fields, also added rename struct for holding rename channel map
 *
 * Revision 1.40  2011/11/07 17:49:20  dechavez
 * accomodated move of mseed to ISI_DL_OPTIONS
 *
 * Revision 1.39  2011/11/03 18:02:52  dechavez
 * Populate ISI_RAW_HEADER mseed field (not tested)
 *
 * Revision 1.38  2011/10/19 16:21:07  dechavez
 * changed static 91011bug to variable daybug to accomodate new behavior in these crazy clocks
 *
 * Revision 1.37  2011/10/14 17:23:52  dechavez
 * updated prototypes
 *
 * Revision 1.36  2011/10/13 18:09:10  dechavez
 * 9/10/11 GPS bug support
 *
 * Revision 1.35  2011/08/04 20:52:54  dechavez
 * added support for network push
 *
 * Revision 1.34  2011/04/25 23:18:22  dechavez
 * added compress parameter
 *
 * Revision 1.33  2011/04/07 22:49:45  dechavez
 * added UINT64 tstamp field to ISIDL_TIMESTAMP, added pndiff to BAROMETER.format.qdplus
 * and added last and diff to BAROMETER (all for support of QDP barometer input)
 *
 * Revision 1.32  2010/12/06 17:31:16  dechavez
 * added mutex,tstamp,retry,watchdog to Q330, to permit watchdog restarts
 *
 * Revision 1.31  2010/04/02 18:37:02  dechavez
 * removed DeregisterQ330s() prototype
 *
 * Revision 1.30  2010/04/01 22:02:56  dechavez
 * updated prototypes
 *
 * Revision 1.29  2010/03/31 22:44:57  dechavez
 * added DeregisterQ330s() prototype
 *
 * Revision 1.28  2009/05/14 16:40:54  dechavez
 * Changed missed barometer sample from -2 to 0xFFFFFFFF
 *
 * Revision 1.27  2008/03/05 23:24:18  dechavez
 * added support intercept processor and ARS leap year bug check
 *
 * Revision 1.26  2008/01/25 22:02:12  dechavez
 * added scl field to ISIDL_PAR
 *
 * Revision 1.25  2007/09/07 20:06:20  dechavez
 * added q330.h Q330_CFG
 *
 * Revision 1.24  2007/05/03 20:27:34  dechavez
 * added LISS support
 *
 * Revision 1.23  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.22  2007/02/08 22:54:07  dechavez
 * define LOCALPKT handle
 *
 * Revision 1.21  2006/12/12 23:27:28  dechavez
 * simplified metadata stuff
 *
 * Revision 1.20  2006/09/29 19:48:57  dechavez
 * add socket send/receive buffers to ISIDL_PAR, with 64kbyte default
 *
 * Revision 1.19  2006/07/10 21:10:06  dechavez
 * added ida5 prototypes
 *
 * Revision 1.18  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.17  2006/06/19 19:14:36  dechavez
 * conditional Q330 support, define ARS handle for rev aware ARS input
 *
 * Revision 1.16  2006/06/07 22:40:50  dechavez
 * removed nsamp from qdplus barometer format handle
 *
 * Revision 1.15  2006/06/02 21:01:59  dechavez
 * added Q330/QDPLUS support
 *
 * Revision 1.14  2006/04/20 23:02:26  dechavez
 * updated prototypes
 *
 * Revision 1.13  2006/04/03 21:38:23  dechavez
 * added ValidSensorSeen to barometer handle
 *
 * Revision 1.12  2006/03/30 22:05:07  dechavez
 * barometer and station message queue support
 *
 * Revision 1.11  2006/03/14 20:31:48  dechavez
 * pass fid to block/unlock exit mutex calls, for debugging use
 *
 * Revision 1.10  2006/03/13 23:07:37  dechavez
 * updated prototypes
 *
 * Revision 1.9  2005/09/30 22:30:31  dechavez
 * updated prototypes
 *
 * Revision 1.8  2005/09/13 00:37:40  dechavez
 * added dbgpath support
 *
 * Revision 1.7  2005/09/10 22:06:34  dechavez
 * replaced DEFAULT_LOG with DEFAULT_BACKGROUND_LOG and DEFAULT_FOREGROUND_LOG
 *
 * Revision 1.6  2005/08/26 20:08:16  dechavez
 * Changed default log spec to syslogd:user
 *
 * Revision 1.5  2005/07/26 18:37:15  dechavez
 * prefix log messages with site or source tag
 *
 * Revision 1.4  2005/07/26 00:49:05  dechavez
 * initial release
 *
 * Revision 1.3  2005/07/06 15:49:36  dechavez
 * checkpoint, apparently with working isi input
 *
 * Revision 1.2  2005/06/24 21:50:36  dechavez
 * checkpoint 0.0.2, with apparently working tty input
 *
 * Revision 1.1  2005/06/10 16:04:06  dechavez
 * 0.0.1 checkpoint
 *
 */
