#pragma ident "$Id: siomet.h,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*
 * SIO met sensor support
 */

#ifndef siomet_h_defined
#define siomet_h_defined

#include "platform.h"
#include "util.h"
#include "ttyio.h"
#include "logio.h"
#include "ida10.h"
#include "mseed.h"
#include "sanio.h" /* for SAN_EPOCH_TO_1970_EPOCH */

#ifdef __cplusplus
extern "C" {
#endif

extern char *VersionIdentString;

/* Constants */

#define SIOMET_BUFLEN                     80
#define SIOMET_FIXED_TO                   5000
#define SIOMET_MIN_SINT                   1000
#define SIOMET_DEFAULT_SINT               SIOMET_MIN_SINT
#define SIOMET_DEFAULT_USER               "nrts"
#define SIOMET_DEFAULT_DAEMON             FALSE
#define SIOMET_DEFAULT_HOME               "/usr/nrts"
#define SIOMET_DEFAULT_TIMEOUT            30
#define SIOMET_DEFAULT_BACKGROUND_LOG     "syslogd:local0"
#define SIOMET_DEFAULT_FOREGROUND_LOG     "-"
#define SIOMET_DEFAULT_DEBUG              FALSE
#define SIOMET_DEFAULT_DBGPATH            NULL
#define SIOMET_DEFAULT_NETID              "II"
#define SIOMET_DEFAULT_PUSH_TIMEOUT       30
#define SIOMET_DEFAULT_PACKET_QUEUE_DEPTH 50

/* Float to int conversion factors */

#define SIOMET_TA_FACTOR 100
#define SIOMET_UA_FACTOR 10
#define SIOMET_PA_FACTOR 10000

/* Module Id's for generating meaningful exit codes */

#define SIOMET_MOD_MAIN    1000
#define SIOMET_MOD_READ    2000
#define SIOMET_MOD_SIGNALS 3000
#define SIOMET_MOD_PACKET  4000

/* Function return codes */

#define SIOMET_OK          0
#define SIOMET_TIMED_OUT  -1
#define SIOMET_CRC_ERROR  -2
#define SIOMET_BAD_STRING -3

/* useful data structures */

#define SIOMET_CNAME_LEN IDA10_CNAMLEN

typedef struct {
    char device[MAXPATHLEN+1];
    int speed;
    int sint;
    INT64 nsint;
    char ta[SIOMET_CNAME_LEN+1];
    char ua[SIOMET_CNAME_LEN+1];
    char pa[SIOMET_CNAME_LEN+1];
    LOGIO *lp;
} SIOMET_INFO;

typedef struct {
    char string[SIOMET_BUFLEN+1];
    REAL64 Ta;
    REAL64 Ua;
    REAL64 Pa;
    INT32 iTa;
    INT32 iUa;
    INT32 iPa;
} SIOMET_SCAN;

#define SIOMET_DATALEN IDA10_DEFDATALEN
#define MAXSAMP (SIOMET_DATALEN / sizeof(INT32))
#define MAXBYTE (MAXSAMP * sizeof(INT32))

typedef struct {
    char sname[MSEED_SNAMLEN+1];
    char nname[MSEED_NNAMLEN+1];
    char cname[IDA10_CNAMLEN+1];
    UINT64 tofs;                  /* time of first sample in buf */
    UINT64 tons;                  /* expected time of next sample */
    UINT64 nsint;                 /* sample interval */
    int nsamp;                    /* number of samples collected so far */
    INT32 data[SIOMET_DATALEN];   /* the packet */
} PROTO_PACKET;

/* Function prototypes */

/* crc.c */
BOOL ChecksumOK(UINT8 *pmsg);

/* exit.c */
void GracefulExit(INT32 status);
void InitExit(void);

/* ida10.c */
void BuildIda10(PROTO_PACKET *pkt, UINT8 *dest);

/* info.c */
SIOMET_INFO *ParseInfoString(char *string);

/* log.c */
void LogCommandLine(int argc, char **argv);
void LogMsg(char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, char *SITE);

/* packet.c */
void FlushPacket(PROTO_PACKET *pkt);
void StartPacketPusher(char *server, int port, LOGIO *lp, int depth, char *sname, char *nname);

/* process.c */
void ToggleProcessThreadVerbosity(void);
void InitPacketBuilder(char *sname, char *nname, SIOMET_INFO *info);
void ProcessScan(SIOMET_SCAN *scan, SIOMET_INFO *info, UINT64 tstamp);

/* read.c */
BOOL GetScan(SIOMET_SCAN *dest);
void ToggleReadThreadVerbosity(void);
void ReadScan(TTYIO *tty, SIOMET_SCAN *scan);
void StartDeviceReader(SIOMET_INFO *info, LOGIO *lp);

/* timer.c */
INT64 CurrentSecond(void);
INT64 WaitForSampleTime(SIOMET_INFO *info, INT64 tols);

#ifdef __cplusplus
}
#endif

#endif /* siomet_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: siomet.h,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
