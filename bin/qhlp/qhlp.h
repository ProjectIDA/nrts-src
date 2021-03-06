#pragma ident "$Id: qhlp.h,v 1.21 2015/12/18 18:51:46 dechavez Exp $"
#ifndef qhlp_h_included
#define qhlp_h_included
#include "qdplus.h"
#include "ida10.h"
#include "q330.h"
#include "detect.h"
#define INCLUDE_APP_SUPPLIED_WRITER
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2007 - Regents of the University of California.";
extern LOGIO_WATCHDOG *Watch;

/* Module Id's for generating meaningful exit codes */

#define QHLP_MOD_MAIN    ((INT32)  100)
#define QHLP_MOD_IDA10   ((INT32)  200)
#define QHLP_MOD_INIT    ((INT32)  300)
#define QHLP_MOD_LOG     ((INT32)  400)
#define QHLP_MOD_SIGNALS ((INT32)  500)
#define QHLP_MOD_READ    ((INT32)  600)
#define QHLP_MOD_PROCESS ((INT32)  700)
#define QHLP_MOD_TRIGGER ((INT32)  800)
#define QHLP_MOD_CALTRIG ((INT32)  900)
#define QHLP_MOD_WRITE   ((INT32) 1000)

/* HLP format codes */

#define FORMAT_IDA10 0

/* Trigger flags */

#define QHLP_TRIG_DEFAULT    0x00
#define QHLP_TRIG_CONTINUOUS 0x01
#define QHLP_TRIG_NOCALTRIG  0x02

/* Default parameters */

#define DEFAULT_DAEMON         FALSE
#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_FORMAT         FORMAT_IDA10
#define DEFAULT_DB             "/usr/nrts"
#define DEFAULT_USER           "nrts"
#define DEFAULT_SYNC_INTERVAL  1
#define DEFAULT_DEBUG_DIR     "/usr/nrts/etc/debug"
#define DEFAULT_NETID         "II"
#define DEFAULT_SEQNO         1
#define DEFAULT_QCODE         'R'

typedef struct {
    char *site;
    char netid[IDA105_NNAME_LEN+1];
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
} DISK_LOOP_DATA;

typedef struct {
    QDPLUS *qdplus;
    UINT32 nxtndx;
    DISK_LOOP_DATA input;
    DISK_LOOP_DATA output;
    UINT64 SyncInterval;
    char dbginfo[MAXPATHLEN+1];
} QHLP_PAR;

typedef void (*QHLP_DL_FUNC)(DISK_LOOP_DATA *output, QDP_HLP *hlp);

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/* Function prototypes */

/* background.c */
BOOL BackGround(ISI_DL *dl);

/* debug.c */
void UpdateDebugState(QHLP_PAR *par);

/* exit.c */
void BlockShutdown(char *fid);
void UnblockShutdown(char *fid);
void SetShutdownFlag(void);
void QuitOnShutdown(INT32 status);
void Exit(INT32 status);
void InitExit(QHLP_PAR *ptr);

/* ida10.c */
void SaveIDA10(DISK_LOOP_DATA *output, QDP_HLP *hlp);
BOOL InitIDA10(QDP_HLP_RULES *rules, DISK_LOOP_DATA *output, UINT32 flags);

/* init.c */
QHLP_PAR *init(char *myname, int argc, char **argv);

/* log.c */
void DbgMsg(char *file, char *format, ...);
void LogMsg(char *format, ...);
void LogDebug(char *format, ...);
LOGIO *GetLogHandle(void);
void LogMsgLevel(int level);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* process.c */
BOOL InitPacketProcessor(Q330_CFG *cfg, ISI_DL *dl, char *metadir, QHLP_DL_FUNC saveFunc, UINT32 trigflags);
void ProcessHLP(void *arg, QDP_HLP *hlp);

/* read.c */
void SaveState(QDPLUS *handle);
void ReadNextPacket(QHLP_PAR *qhlp, QDPLUS_PKT *pkt);
void RecoverState(QHLP_PAR *qhlp);

/* signals.c */
void StartSignalHandler(QHLP_PAR *arg);
void StartSignalHandler(QHLP_PAR *arg);

/* trigger.c */
void LogTriggerData(void);
BOOL InitTrigger(Q330_CFG *cfg, ISI_DL *dl, char *metadir, QHLP_DL_FUNC saveFunc, UINT32 trigflags);
void ProcessTrigChan(DISK_LOOP_DATA *output, QDP_HLP *hlp);

/* write.c */
BOOL WritePacketToDisk(ISI_DL *dl, ISI_RAW_PACKET *raw);

#endif /* qhlp_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: qhlp.h,v $
 * Revision 1.21  2015/12/18 18:51:46  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.20  2015/12/09 18:41:58  dechavez
 * cleaned up prototypes
 *
 * Revision 1.19  2014/08/28 21:37:06  dechavez
 * removed options from DISK_LOOP_DATA (it is now part of ISI_DL), added
 * new write.c WritePacketToDisk() prototype
 *
 * Revision 1.18  2011/11/07 17:40:07  dechavez
 * removed mseed from DISK_LOOP_DATA (it's in ISI_DL_OPTIONS now)
 *
 * Revision 1.17  2011/11/03 17:53:23  dechavez
 * Generate equivalent MiniSEED equivalent, when possible
 *
 * Revision 1.16  2011/10/12 17:38:57  dechavez
 * changed DISK_LOOP_DATA options from int to ISI_DL_OPTIONS, added prototypes
 *
 * Revision 1.15  2011/04/07 22:56:06  dechavez
 * added netid to DISK_LOOP_DATA, default "II"
 *
 * Revision 1.14  2010/03/12 00:48:13  dechavez
 * trigger.c and caltrig.c prototypes and module codes
 *
 * Revision 1.13  2009/07/09 18:37:08  dechavez
 * updated prototypes
 *
 * Revision 1.12  2009/07/08 16:03:20  dechavez
 * updated prototypes
 *
 * Revision 1.11  2009/07/02 23:08:46  dechavez
 * added prototypes and trigger flag values
 *
 * Revision 1.10  2009/05/14 18:12:44  dechavez
 * added debug verbosity toggling via /usr/nrts/etc/debug
 *
 * Revision 1.9  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.8  2009/01/05 17:40:26  dechavez
 * added WatchDir global variable
 *
 * Revision 1.7  2007/09/25 22:29:14  dechavez
 * removed default trigger channels
 *
 * Revision 1.6  2007/09/22 00:28:41  dechavez
 * Removed explicit detector stuff (now using libdetect instead), updated
 * prototypes for new process.c which replaced trigger.c
 *
 * Revision 1.5  2007/09/17 23:27:51  dechavez
 * checkpoint - reworking threshold trigger (now with specific list of triggered channels)
 *
 * Revision 1.4  2007/09/14 19:51:45  dechavez
 * added trigger stuff
 *
 * Revision 1.3  2007/09/07 20:26:13  dechavez
 * added DEFAULT_USER
 *
 * Revision 1.2  2007/05/15 20:43:02  dechavez
 * use QuitOnShutdown() instead of ShutdownInProgress() test
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
