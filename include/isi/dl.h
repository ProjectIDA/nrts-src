#pragma ident "$Id: dl.h,v 1.60 2016/07/20 21:50:22 dechavez Exp $"
/*======================================================================
 *
 *  ISI disk loop support
 *
 *====================================================================*/
#ifndef isi_dl_h_included
#define isi_dl_h_included

#ifndef WIN32
#include <curses.h>
#endif
#include "platform.h"
#include "ida.h"
#include "ida10.h"
#include "isi.h"
#include "nrts/types.h"
#include "dbio.h"
#include "qdp.h"
#include "msgq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISI_RDONLY 0
#define ISI_RDWR   1

#ifdef DARWIN
#define ISI_DL_DEFAULT_HOME_DIR     "/Users/nrts"
#else
#define ISI_DL_DEFAULT_HOME_DIR     "/usr/nrts"
#endif
#define ISI_DL_DIR_NAME             "isi"
#define ISI_DL_SYS_NAME             "sys"
#define ISI_DL_HDR_NAME             "hdr"
#define ISI_DL_RAW_NAME             "raw"
#define ISI_DL_LOCK_NAME            "lock"
#define ISI_DL_TEE_DIR              "tee"
#define ISI_DL_META_DIR             "meta"
#define ISI_DL_QDP_STATE_FILE       "qhlp"
#define ISI_DL_MERGE_PREFIX         "merge"
#ifdef DARWIN
#define ISI_DL_DEFAULT_DBID         "/Users/nrts"
#else
#define ISI_DL_DEFAULT_DBID         "/usr/nrts"
#endif
#define ISI_DL_DEFAULT_SLINK_CFGSTR "localhost:16000:512:500:II"
#define ISI_DL_DEFAULT_SLINK_RETRY 30000
#define ISI_DL_DEFAULT_SLINK_DEBUG 0

#define ISI_INACTIVE 0 /* disk loop has no parent */
#define ISI_ACTIVE   1 /* active packet is being filled */
#define ISI_IDLE     2 /* disk loop has no active packets */
#define ISI_UPDATE   3 /* disk loop indices being updated */

#define ISI_UNDEFINED_INDEX 0xFFFFFFFF
#define ISI_CORRUPT_INDEX   (ISI_UNDEFINED_INDEX - 1)
#define isiIsValidIndex(index) (index != ISI_UNDEFINED_INDEX && index != ISI_CORRUPT_INDEX)

/* isiWriteToDiskLoop() options */

#define ISI_OPTION_NONE                     0x0000
#define ISI_OPTION_GENERATE_SEQNO           0x0001
#define ISI_OPTION_REJECT_DUPLICATES        0x0002
#define ISI_OPTION_REJECT_COMPLETE_OVERLAPS 0x0004
#define ISI_OPTION_INSERT_32BIT_SEQNO       0x0008
#define ISI_OPTION_INSERT_32BIT_TSTAMP      0x0010

/* For feeding SeedLink */

typedef struct {
    MUTEX mutex;
    LNKLST *list;     /* linked list of MiniSEED packets ready to forward */
    SEMAPHORE sem;    /* to alert each time a packet is added to the list */
    UINT32 depth;     /* maximum number of elements in the list */
    UINT32 dropped;   /* number of packets dropped */
    BOOL empty;       /* TRUE when queue is drained */
} RECORDQ;

typedef struct {
    BOOL enabled;              /* TRUE when SeedLink support enabled */
    struct {
        BOOL acquired;         /* FALSE until after first packet is acquired by backend */
        BOOL sent;             /* FALSE until after first packet is sent to ring server */
    } first;
    char rsaddr[MAXPATHLEN+1]; /* server[:port] */
    RECORDQ queue;             /* message queue for buffering packets to server */
    THREAD tid;                /* data forwarding thread */
    UINT32 retry;              /* msec to wait between ringserver connection attempts */
    void *dlcp;                /* data link handle (will be cast to DLCP in lib/isidl/slink.c) */
    void *mseed;               /* will be cast to MSEED_HANDLE in lib/isidl/slink.c */
    SEMAPHORE sem;             /* for synchronization with server write thread */
    int debug;                 /* debug verbosity level */
    LOGIO *lp;
} ISI_DL_SEEDLINK;

typedef struct {
    UINT32 flags;        /* bitmask of ISI_OPTION_X values above */
    struct {
        int seqno;       /* for ISI_OPTION_INSERT_32BIT_SEQNO */
        int tstamp;      /* for ISI_OPTION_INSERT_32BIT_TSTAMP */
    } offset;
} ISI_DL_OPTIONS;

/* Debug levels */

typedef struct {
    int pkt;  /* packet decoder, checker */
    int ttag; /* time tags */
    int bwd;  /* segment tracker */
    int dl;   /* disk loop i/o */
    int lock; /* disk loop locks */
} ISI_DEBUG;

/* for accessing the original NRTS disk loops */

typedef struct {
    ISI_GENERIC_TS isi;
    struct {
        UINT32 type;  /* digitizer packet type */
        UINT32 len;   /* digitizer packet length */
        UINT8 *data;  /* digitizer packet */
        int order;    /* digitizer packet byte order */
        struct {
            UINT32 len;
            UINT8 *data;
        } hdr, dat;   /* header and data portions of digitizer packet */
    } orig;
    struct {
        union {
            IDA_DREC ida;    /* decoded IDA rev < 10 packet */
            IDA10_TS ida10;  /* decoded IDA10 packet */
        } buf;
        int len; /* number of bytes used by buf */
    } work;
    int status;
    char ident[ISI_STREAM_NAME_LEN+1];
    int chndx; /* index into the associated NRTS_CHN array */
    BOOL tear; /* TRUE if time tear was detected */
    struct {
        int hdr; /* offset in hdr file */
        int dat; /* offset in dat file */
    } foff;
    int indx; /* index into ISI disk loop for this packet */
    IDA_EXTRA extra; /* optional seqno and timestamp */
} NRTS_PKT;

typedef BOOL (*NRTS_UNPACK_FUNC)(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out);

typedef struct {
    UINT32 clock;  /* discrepancy between host time and data time */
    UINT32 tic;    /* time tears in ticks */
    UINT32 samp;   /* time tears in samples */
} NRTS_TOLERANCE;

typedef struct {
    MUTEX mutex;
    int flushint; /* flush interval (sec) */
    time_t tolf; /* time segment list was last synced to disk */
    LNKLST list[NRTS_MAXCHN];  /* per channel list of wfdiscs */
    int level; /* used internally for tracking recursion levels */
} NRTS_BWD;
#define NRTS_DEFAULT_BWD_FLUSHINT 30

typedef struct {
    NRTS_SYS *sys;           /* mapped system */
    NRTS_MAP map;            /* for holding mapped system stuff */
    NRTS_DLNAMES fname;      /* file names */
    LOGIO *lp;               /* for logging */
    DBIO *db;                /* inherited database pointer */
    NRTS_UNPACK_FUNC unpack; /* packet decoder */
    NRTS_BWD bwd;            /* binary wfdiscs (segment tracker) */
    void *save[NRTS_MAXCHN]; /* for sanity checks with previous packets */
    int perm;                /* ISI_RDONLY or ISI_RDWR */
#define NRTS_DL_FLAGS_STRICT_TIME    0x00000001 /* real time test */
#define NRTS_DL_FLAGS_STATION_SYSTEM 0x00000002
#define NRTS_DL_FLAGS_USE_BWD_DB     0x00000004
#define NRTS_DL_FLAGS_NO_BWD_FF      0x00000008
#define NRTS_DL_FLAGS_ALWAYS_TESTWD  0x00000010
#define NRTS_DL_FLAGS_LAX_TIME       0x00000020 /* ignore suspicious bit */
#define NRTS_DL_FLAGS_LEAP_YEAR_CHK  0x00000040 /* check for possible leap year bug */
#define NRTS_DL_FLAGS_ASYNC_WRITE    0x00000080 /* supprress O_SYNC on write */
#define NRTS_DL_FLAGS_DEFAULT        0
    int flags;               /* option flags */
    ISI_DEBUG *debug;        /* inherited debug levels */
    char *home;              /* inherited home directory */
    NRTS_TOLERANCE tol;      /* for trusting station time stamps */
    UINT8 *buf;              /* internal use */
    int buflen;              /* length of buf */
    NRTS_PKT *pkt;           /* internal use */
    IDA *ida;                /* handle for IDA packet decoders */
    struct {
        int hdr;
        int dat;
    } handle;                /* file handles for reading packets from the disk loop */
    LNKLST *ida10;           /* packet conversion workspace */
} NRTS_DL;

/* ISI disk loops */

typedef struct {
    DBIO *db;                /* database pointer */
    char *dbid;              /* database identifier */
    char root[MAXPATHLEN+1]; /* root directory */
    UINT32 trecs;            /* records per tee file */
    ISI_DEBUG debug;         /* debug levels */
    UINT16 flags;            /* default DL flags */
} ISI_GLOB;

typedef struct {
    UINT32 active;   /* index of active packet (n/a when state != ISI_BUSY) */
    UINT32 oldest;   /* index of oldest packet in the disk loop */
    UINT32 yngest;   /* index of youngest packet in the disk loop */
    UINT32 lend;     /* index of logical end of disk loop */
} ISI_INDEX;

typedef struct {
    char site[ISI_SITELEN+1];  /* site name */
    ISI_SEQNO seqno;           /* sequence number generator */
    UINT32 numpkt;             /* number of packets in disk loop */
    UINT32 nhide;              /* number of "hidden" packets in disk loop */
    UINT32 maxlen;             /* largest packet allowed */
    UINT32 hdrlen;             /* size of the ISI_RAW_HEADER used */
    ISI_INDEX index;           /* disk loop pointers */
    ISI_INDEX backup;          /* backup disk loop pointers */
    pid_t parent;              /* process id of "owner" (writer) */
    int state;                 /* ISI_EMPTY, ISI_BUSY, or ISI_IDLE */
    UINT64 count;              /* number of records written since creation */
   struct {
      INT64 start;             /* time DL writer started */
      INT64 write;             /* time most recent packet was written to DL */
   } tstamp;
} ISI_DL_SYS;

typedef struct {
    BOOL disabled;
    FILE *fp;
    char path[MAXPATHLEN+1];
} ISI_TEE;

typedef struct {
    char hdr[MAXPATHLEN+1];
    char raw[MAXPATHLEN+1];
    char meta[MAXPATHLEN+1];
    char qdpstate[MAXPATHLEN+1];
} ISI_DL_PATH;

typedef struct {
    ISI_GLOB *glob;          /* convenience stuff */
    char base[MAXPATHLEN+1]; /* top level of the isi directory */
    ISI_DL_SYS *sys;         /* disk loop control and state of health */
    UTIL_MMAP sysmap;        /* used to mmap sys, above */
    ISI_DL_PATH path;        /* full path names to the hdr and raw files */
    LOGIO *lp;               /* for logging */
    NRTS_DL *nrts;           /* NRTS 2.0 disk loop */
    ISI_TEE tee;             /* tee subsystem */
#define ISI_DL_FLAGS_HAVE_META      0x0001
#define ISI_DL_FLAGS_HAVE_QDP_STATE 0x0002
#define ISI_DL_FLAGS_IGNORE_LOCKS   0x1000
#define ISI_DL_FLAGS_TIME_DL_OPS    0x2000
#define ISI_DL_FLAGS_ASYNC_WRITE    0x4000
    UINT16 flags;            /* misc flags bitmap */
    struct {
        int hdr;
        int raw;
    } fd;                    /* file descriptors for disk loop writer */
#define MAXTP 32
    int ntp;
    /* struct timeb tp[MAXTP]; */
    struct {
        FILE *fp;
        int perm;
    } lock;                  /* advisory lock for diskloop I/O */
    ISI_DL_SEEDLINK slink;   /* for feeding a SeedLink server */
    ISI_DL_OPTIONS options;  /* options for disk loop writer */
} ISI_DL;

typedef struct {
    ISI_GLOB *glob; /* convenience stuff */
    LOGIO *lp;      /* for logging */
    LNKLST list;    /* linked list of disk loop descriptors */
    ISI_DL **dl;    /* array view of list */
    int ndl;        /* number of entries in dl */
} ISI_DL_MASTER;

/* User supplied disk loop write function for ISI push server */

typedef BOOL (*ISI_DL_SAVE_FUNC)(ISI_DL *dl, ISI_RAW_PACKET *pkt);

/* Handle for monitoring tools */

typedef struct {
    ISI_DL **dl;     /* array of pointers to all available ISI disk loop descriptors */
    int ndl;         /* number of entries in dl */
    ISI_DL *show[2]; /* disk loop(s) to display */
    int index[2];    /* index into dl of show[0] and show[1] */
} ISI_ISI_DL_SET;

typedef struct {
    NRTS_DL **dl;  /* array of pointers to all available NRTS disk loop descriptors */
    int ndl;       /* number of entries in dl */
    NRTS_DL *show; /* disk loop to display */
    int index;     /* index into dl of show */
    int beg;       /* index of channel to display next */
    int end;       /* index of last channel shown */
    int nchn;      /* number of channels available to show */
} ISI_NRTS_DL_SET;

typedef struct {
    ISI_GLOB glob;
    ISI_DL_MASTER *master;
    ISI_ISI_DL_SET  isi;  /* all available ISI  disk loops */
    ISI_NRTS_DL_SET nrts; /* all available NRTS disk loops */
} ISI_MON;

/* Prototyypes */

/* application supplied */
#ifdef INCLUDE_APP_SUPPLIED_WRITER
extern BOOL AppSuppliedWritePacketToDisk(ISI_DL *dl, ISI_RAW_PACKET *raw);
#endif /* INCLUDE_APP_SUPPLIED_WRITER */

/* cnf.c */
BOOL isidlBuildStreamCnfList(ISI_DL_MASTER *master, LNKLST *list);

/* datreq.c */
INT32 isidlExpandTwindRequest(ISI_DL_MASTER *master, ISI_TWIND_REQUEST *req, LNKLST *out);
INT32 isidlExpandSeqnoRequest(ISI_DL_MASTER *master, ISI_SEQNO_REQUEST *req, LNKLST *out);

/* dl.c */
ISI_DL *isidlCreateDiskLoop(ISI_GLOB *glob, char *site, UINT32 numpkt, UINT32 nhide, UINT32 maxlen, LOGIO *lp);
void isidlUpdateParentID(ISI_DL *dl, int pid);
void isidlSetMetaDir(ISI_DL *dl);
ISI_DL *isidlOpenDiskLoop(ISI_GLOB *glob, char *site, LOGIO *lp, int perm, UINT32 options);
void isidlCloseDiskLoop(ISI_DL *dl);
BOOL isidlWriteToDiskLoop(ISI_DL *dl, ISI_RAW_PACKET *raw);
BOOL isidlReadDiskLoop(ISI_DL *dl, ISI_RAW_PACKET *dest, UINT32 index);
BOOL isidlSnapshot(ISI_DL *dl, ISI_DL *snapshot, ISI_DL_SYS *sys);
ISI_SEQNO isidlPacketSeqno(ISI_DL *dl, UINT32 index);
ISI_SEQNO isidlCrntSeqno(ISI_DL *dl);

/* glob.c */
ISI_GLOB *isidlInitDefaultGlob(DBIO *db, ISI_GLOB *glob);
BOOL isidlSetGlobalParameters(char *dbspec, char *app, ISI_GLOB *glob);
void isidlLoadDebugFlags(FILE *fp, ISI_DEBUG *debug);

/* master.c */
ISI_DL_MASTER *isidlOpenDiskLoopSet(LNKLST *sitelist, ISI_GLOB *glob, LOGIO *lp, int perm, UINT32 options);

/* meta.c */
void isidlWriteQDPMetaData(ISI_DL *dl, UINT64 serialno, QDP_META *meta);

/* mon.c */
void isidlMonUpdateISI(WINDOW *win, ISI_MON *mon, int row);
void isidlMonStaticISI(WINDOW *win, int row);
void isidlSetMonISI(ISI_MON *mon, char *target);
void isidlIncrMonISI(ISI_MON *mon);
void isidlDecrMonISI(ISI_MON *mon);
void isidlMonUpdateNRTS(WINDOW *win, ISI_MON *mon, int start);
void isidlMonStaticNRTS(WINDOW *win, int row);
void isidlSetMonNRTS(ISI_MON *mon, char *target);
void isidlIncrMonNRTSchn(ISI_MON *mon);
void isidlIncrMonNRTS(ISI_MON *mon);
void isidlDecrMonNRTS(ISI_MON *mon);
BOOL isidlInitMON(char *myname, ISI_MON *mon, char *dbspec, LOGIO *lp);

/* pack.c */
int isidlPackDLSys(UINT8 *start, ISI_DL_SYS *src);
int isidlUnpackDLSys(UINT8 *start, ISI_DL_SYS *dest);

/* print.c */
void isidlLogDL(ISI_DL *dl, int level);
void isidlPrintDL(FILE *fp, ISI_DL *dl);

/* push.c */
BOOL isidlPacketServer(ISI_DL *dl, int port, LOGIO *lp, ISI_DL_SAVE_FUNC func);

/* report.c */
BOOL isidlBuildDlSysReport(ISI_DL_MASTER *master, LNKLST *list);

/* search.c */
UINT32 isidlSearchDiskLoopForSeqno(ISI_DL *dl, ISI_SEQNO *target, UINT32 ldef, UINT32 rdef);

/* slink.c */
void isidlLogSeedLinkOption(LOGIO *logio, int level, ISI_DL *dl);
BOOL isidlSetSeedLinkOption(ISI_DL *dl, char *argstr, char *ident, int debug);
void isidlFeedSeedLink(ISI_DL_SEEDLINK *slink, ISI_RAW_PACKET *raw);

/* soh.c */
BOOL isidlBuildStreamSohList(ISI_DL_MASTER *master, LNKLST *list);

/* string.c */
char *isidlIndexString(ISI_INDEX *index, char *buf);
char *isidlStateString(int state);

/* sys.c */
ISI_SEQNO isidlSysIncrSeqno(ISI_DL *dl, BOOL sync);
void isidlSysSetSeqno(ISI_DL *dl, ISI_SEQNO *seqno, BOOL sync);
void isidlSysSetIndex(ISI_DL *dl, ISI_INDEX *index, BOOL sync);
void isidlSysSetBackup(ISI_DL *dl, ISI_INDEX *backup, BOOL sync);
void isidlSysSetParent(ISI_DL *dl, int pid, BOOL sync);
void isidlSysSetState(ISI_DL *dl, int state, BOOL sync);
void isidlSysIncrCount(ISI_DL *dl, BOOL sync);
void isidlSysSetTstampStart(ISI_DL *dl, INT64 tstamp, BOOL sync);
void isidlSysSetTstampWrite(ISI_DL *dl, INT64 tstamp, BOOL sync);

/* tee.c */
void isidlCloseTee(ISI_DL *dl);
void isidlWriteToTeeFile(ISI_DL *dl, ISI_RAW_PACKET *raw);

/* utils.c */
NRTS_DL *isidlLocateNrtsDiskloop(ISI_DL_MASTER *master, ISI_STREAM_NAME *target, NRTS_STREAM *result);
BOOL isidlStreamExists(ISI_DL_MASTER *master, ISI_STREAM_NAME *target);
ISI_DL *isidlLocateDiskLoopBySite(ISI_DL_MASTER *master, char *target);
void isidlInitOptions(ISI_DL_OPTIONS *dest, UINT32 flags);

/* version.c */
char *isidlVersionString(void);
VERSION *isidlVersion(void);

/* wfdisc.c */
BOOL isidlGetWfdiscList(ISI_DL_MASTER *master, LNKLST *list, UINT32 maxdur);
#ifdef __cplusplus
}
#endif

#endif /* isi_dl_h_included */

/* Revision History
 *
 * $Log: dl.h,v $
 * Revision 1.60  2016/07/20 21:50:22  dechavez
 * changed ISI_DL_DEFAULT_HOME_DIR and ISI_DL_DEFAULT_DBID to /Users/nrts for MacOS builds
 *
 * Revision 1.59  2015/12/08 18:20:43  dechavez
 * completely updated prototypes
 *
 * Revision 1.58  2015/12/08 17:50:30  dechavez
 * fixed up prototypes some more
 *
 * Revision 1.57  2015/12/08 17:47:26  dechavez
 * fixed up prototypes
 *
 * Revision 1.56  2014/08/28 21:19:11  dechavez
 * removed mseed and bud from ISI_DL_OPTIONS, and made it a part of the ISI_DL handle
 *
 * Revision 1.55  2014/08/19 18:54:18  dechavez
 * added debug field to ISI_DL_SEEDLINK
 *
 * Revision 1.54  2014/08/08 15:41:50  dechavez
 * reworked ISI_DL_SEEDLINK to support new (IDA) libmseed, updated isidlFeedSeedLink() prototype
 *
 * Revision 1.53  2014/04/30 15:51:58  dechavez
 * removed MSEED_HANDLE, made ISI_DL_SEEDLINK::mseed a void pointer (cast to MSTemplateHandle *)
 *
 * Revision 1.52  2013/03/08 18:27:11  dechavez
 * moved ISI_DL_SEEDLINK from options to ISI_DL (to make for easy exit handling)
 *
 * Revision 1.51  2013/02/07 23:32:05  dechavez
 * added MUTEX, LOGIO and dropped count to ISI_DL_SEEDLINK
 *
 * Revision 1.50  2013/02/07 18:11:30  dechavez
 * changed ISI_DL_SEEDLINK to use (new) RECORDQ instead of MSGQ_BUF
 *
 * Revision 1.49  2013/01/23 23:33:24  dechavez
 * updated isidlLogSeedLinkOption() prototype
 *
 * Revision 1.48  2013/01/18 23:37:39  dechavez
 * changed IDA9 (and below) work space to IDA_DREC in NRTS_PKT
 *
 * Revision 1.47  2013/01/14 22:07:10  dechavez
 * introduced SeedLink support
 *
 * Revision 1.46  2012/08/02 17:41:08  dechavez
 * WIN32 ifdef'd out mon.c stuff (aap)
 *
 * Revision 1.45  2012/08/01 17:06:29  dechavez
 * defined NRTS_DL_FLAGS_ASYNC_WRITE option flag
 *
 * Revision 1.44  2012/07/30 16:55:59  dechavez
 * defined ISI_DL_FLAGS_ASYNC_WRITE option flag
 *
 * Revision 1.43  2012/06/27 15:27:46  dechavez
 * added isidlLocateDiskLoopBySite() prototype
 *
 * Revision 1.42  2011/11/07 17:24:52  dechavez
 * created ISI_DL_BUD and added to ISI_DL_OPTIONS
 *
 * Revision 1.41  2011/10/12 17:17:22  dechavez
 * introduced ISI_DL_OPTIONS
 *
 * Revision 1.40  2011/08/04 22:00:22  dechavez
 * added curses.h
 *
 * Revision 1.39  2011/07/27 23:34:57  dechavez
 * added ISI_MON and related prototypes
 *
 * Revision 1.38  2010/11/04 21:41:23  dechavez
 * add hdrlen to ISI_DL_SYS
 *
 * Revision 1.37  2010/09/10 22:40:26  dechavez
 * added ISI_DL_MERGE_PREFIX
 *
 * Revision 1.36  2010/08/27 18:46:24  dechavez
 * updated prototypes (in support of DL_SYS report)
 *
 * Revision 1.35  2009/03/17 17:16:29  dechavez
 * eliminated mutex and ISI_DL_LOCK from ISI_DL handle, use a simple embedded
 * structure for advisory locks
 *
 * Revision 1.34  2009/01/30 04:51:17  dechavez
 * updated prototypes
 *
 * Revision 1.33  2008/03/05 22:46:46  dechavez
 * define NRTS_DL_FLAGS_LEAP_YEAR_CHK NRTS_DL flag bit
 *
 * Revision 1.32  2008/01/07 21:45:01  dechavez
 * added IDA10 conversion buffer to NRTS_DL handle
 *
 * Revision 1.31  2007/09/14 19:23:26  dechavez
 * added ISI_OPTION_REJECT_COMPLETE_OVERLAPS option
 *
 * Revision 1.30  2007/06/28 19:40:00  dechavez
 * added NRTS_DL_FLAGS_LAX_TIME flag bit
 *
 * Revision 1.29  2007/06/01 19:00:03  dechavez
 * replaced explicit structure for NRTS_PKT extra field with IDA_EXTRA type
 *
 * Revision 1.28  2007/05/03 20:26:19  dechavez
 * changed name of state file
 *
 * Revision 1.27  2007/04/18 22:44:40  dechavez
 * added NRTS_DL_FLAGS_NO_BWD_FF and NRTS_DL_FLAGS_ALWAYS_TESTWD NRTS flags
 * added ISI_DL_FLAGS_HAVE_QDP_STATE ISI flag
 * added qdpstate to ISI_DL_PATH and defined ISI_DL_QDP_STATE_FILE
 *
 * Revision 1.26  2007/03/26 21:38:52  dechavez
 * defined NRTS_DL_FLAGS_x constants
 *
 * Revision 1.25  2007/02/08 23:09:23  dechavez
 * added ISI_DL_FLAGS_TIME_DL_OPS flag, file descriptors for writing disk loops
 *
 * Revision 1.24  2007/01/23 02:54:14  dechavez
 * added ignore locks flag
 *
 * Revision 1.23  2007/01/11 22:00:12  dechavez
 * renamed all functions to use isidl prefix
 *
 * Revision 1.22  2007/01/11 17:45:43  dechavez
 * STREAM to TWIND
 *
 * Revision 1.21  2006/12/12 22:34:35  dechavez
 * added QDP metadata support
 *
 * Revision 1.20  2006/11/10 05:44:52  dechavez
 * added ttag to ISP_DEBUG
 *
 * Revision 1.19  2006/06/19 18:00:54  dechavez
 * added indx field to NRTS_PKT and readonly handles to NRTS_DL for smart reads (aap)
 *
 * Revision 1.18  2006/06/12 21:43:17  dechavez
 * updated prototypes
 *
 * Revision 1.17  2006/06/02 20:24:27  dechavez
 * updated prototypes (sys.c)
 *
 * Revision 1.16  2006/04/20 22:55:09  dechavez
 * updated prototypes
 *
 * Revision 1.15  2006/04/07 17:05:12  dechavez
 * removed gz from TEE handle
 *
 * Revision 1.14  2006/03/13 23:42:32  dechavez
 * fixed ISI_GLOB trecs declaration
 *
 * Revision 1.13  2006/03/13 22:13:59  dechavez
 * redefined ISI_GLOB (as part of replacing ini= command line opt with db=)
 *
 * Revision 1.12  2006/02/14 17:05:05  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.11  2006/02/09 20:06:52  dechavez
 * added IDA handle, removed ziptee from ISI_GLOB, removed db.c prototypes
 *
 * Revision 1.10  2005/12/09 21:14:26  dechavez
 * replaced ISI_KEEP_DUPS with ISI_REJECT_DUPLICATES
 *
 * Revision 1.9  2005/12/09 21:10:30  dechavez
 * added isiWriteToDiskLoop() flags, changed isiWriteToDiskLoop() SeqnoRule arg to flags
 *
 * Revision 1.8  2005/10/10 23:55:48  dechavez
 * changed ISI_DL_GZIP_FLAG to ISI_DL_NOZIP_FLAG (since we are going to gzip by default)
 *
 * Revision 1.7  2005/10/06 21:55:50  dechavez
 * added samp field to NRTS_TOLERANCE
 *
 * Revision 1.6  2005/10/01 00:23:36  dechavez
 * added support for on-the-fly gzip to the tee directory
 *
 * Revision 1.5  2005/09/10 21:47:19  dechavez
 * redesign to eliminate mmap'd hdr and raw parts of ISI_DL, rechristen ISI_DL_HDR
 * to more appropriate ISI_DL_SYS.
 *
 * Revision 1.4  2005/08/26 18:09:30  dechavez
 * added tee support to ISI_DL
 *
 * Revision 1.3  2005/07/26 17:06:06  dechavez
 * added a mutex to the ISI_DL handle
 *
 * Revision 1.2  2005/07/26 00:25:41  dechavez
 * added version.c prototyes
 *
 * Revision 1.1  2005/07/25 22:46:30  dechavez
 * initial release
 *
 */
