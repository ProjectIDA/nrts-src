#pragma ident "$Id: paro10.h,v 1.6 2015/12/18 18:51:43 dechavez Exp $"
#ifndef paro10_h_included
#define paro10_h_included

#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"
#include "list.h"
#include "ida.h"
#include "logio.h"
#include "ttyio.h"
#include "paro.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2012 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define PARO10_MOD_MAIN      ((INT32)  100)
#define PARO10_MOD_LOG       ((INT32)  200)
#define PARO10_MOD_SIGNALS   ((INT32)  300)
#define PARO10_MOD_BARO      ((INT32)  400)
#define PARO10_MOD_PACKET    ((INT32)  500)
#define PARO10_MOD_PUSH      ((INT32)  600)

#define DEFAULT_SINT               1000
#define DEFAULT_USER               "nrts"
#define DEFAULT_DAEMON             FALSE
#define DEFAULT_HOME               "/usr/nrts"
#define DEFAULT_TIMEOUT            30
#define DEFAULT_BACKGROUND_LOG     "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG     "-"
#define DEFAULT_DEBUG              FALSE
#define DEFAULT_DBGPATH            NULL
#define DEFAULT_NETID              "II"
#define DEFAULT_PUSH_TIMEOUT       30
#define DEFAULT_PACKET_QUEUE_DEPTH 50
#ifndef HAVE_MYSQL
#define DEFAULT_DB                 "/usr/nrts"
#else
#define DEFAULT_DB                 "localhost:nrts:NULL:ida:0"
#endif
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif

#define PARO10_MISSED_SAMPLE (INT32) 0x80000000
#define PARO10_NO_SENSOR ~PARO10_MISSED_SAMPLE

/* For reading data from a single barometer */

typedef struct {
    MUTEX mutex;
    LOGIO *lp;
    BOOL debug;
    char *port;
    int baud;
    char sname[IDA105_SNAME_LEN+1];
    char nname[IDA105_NNAME_LEN+1];
    char cname[IDA10_CNAMLEN+1];
    PARO *pp;
    INT32 sample;
    BOOL ValidSensorSeen;
    BOOL ValidSampleSeen;
    int nsamp;
    int maxsamp;
    ISI_RAW_PACKET pkt;
    INT16 srfact;
    INT16 srmult;
    UINT32 MsecSint;
    IDA10_GENTAG clock;
    BOOL first;
    UINT8 *pnsamp;
    UINT64 tols;
} BAROMETER;

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/*  Function prototypes  */

/* baro.c */
void ToggleBarometerDebugState(void);
BOOL BarometerDebugEnabled(void);
void LogBarometerSerialNumber(void);
INT32 GetSample(BAROMETER *bp);
BOOL AddBarometer(LNKLST *list, char *argstr, char *sname);
THREAD_FUNC ReadThread(void *argptr);

/* exit.c */
void BlockShutdown(char *fid);
void UnblockShutdown(char *fid);
void Exit(INT32 status);
VOID InitExit(void);

/* log.c */
LOGIO *GetLogHandle(VOID);
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* packet.c */
THREAD_FUNC PacketThread(void *argptr);

/* push.c */
void FlushPacket(BAROMETER *bp);
void StartPacketPusher(char *server, int port, LOGIO *lp, int depth, char *sname, char *nname);

/* signals.c */
VOID StartSignalHandler(VOID);

#endif /* paro10_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * $Log: paro10.h,v $
 * Revision 1.6  2015/12/18 18:51:43  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.5  2015/12/07 19:12:36  dechavez
 * added missing prototypes
 *
 * Revision 1.4  2014/08/28 23:14:29  dechavez
 * LogBarometerSerialNumber() prototype
 *
 * Revision 1.3  2012/10/31 17:22:51  dechavez
 * added pnsamp and tols to BAROMETER, renamed PushToRemoteDiskLoop() to FlushPacket()
 *
 * Revision 1.2  2012/10/25 22:15:38  dechavez
 * initial release
 *
 * Revision 1.1  2012/10/25 21:40:15  dechavez
 * development release
 *
 */
