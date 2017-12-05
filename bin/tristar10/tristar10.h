#pragma ident "$Id: tristar10.h,v 1.7 2015/12/18 18:51:47 dechavez Exp $"
#ifndef tristar10_h_included
#define tristar10_h_included
#include "platform.h"
#include "tristar.h"
#include "ida10.h"
#include "util.h"
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2012 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define TRISTAR10_MOD_MAIN    ((INT32)  100)
#define TRISTAR10_MOD_BGROUND ((INT32)  200)
#define TRISTAR10_MOD_MOXA    ((INT32)  300)
#define TRISTAR10_MOD_LOG     ((INT32)  400)
#define TRISTAR10_MOD_SIGNALS ((INT32)  500)
#define TRISTAR10_MOD_PACKET  ((INT32)  600)

/* Default parameters */

#define DEFAULT_USER               "nrts"
#define DEFAULT_DAEMON             FALSE
#define DEFAULT_BACKGROUND_LOG     "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG     "-"
#define DEFAULT_DEBUG              FALSE
#define DEFAULT_MOXA_IP            "192.168.1.20"
#define DEFAULT_MOXA_PORT           MODBUS_TCP_DEFAULT_PORT
#define DEFAULT_NETID              "II"
#define DEFAULT_PACKET_QUEUE_DEPTH 50
#define DEFAULT_SAMPLE_INTERVAL    10
#define MIN_SAMPLE_INTERVAL         5
#define CONVERSION_GAIN            100

/* stream (chnloc) names */

#define AERRS "aerrs"
#define AES01 "aes01"
#define AEA01 "aea01"
#define AEF01 "aef01"
#define AEV01 "aev01"
#define AEC01 "aec01"
#define AE101 "ae101"
#define AE201 "ae201"
#define AES02 "aes02"
#define AEA02 "aea02"
#define AEF02 "aef02"
#define AEV02 "aev02"
#define AEC02 "aec02"

/* modbus and disk loop servers */

typedef struct {
    char server[MAXPATHLEN+1];
    int port;
    struct timeval bto; /* byte timeout */
    struct timeval rto; /* response timeout */
} SERVER;

/* one reading */

typedef struct {
    char chnloc[IDA10_CNAMLEN+1]; /* channel name */
    INT32 value;   /* datum, scaled to fit in an INT32 if necessary */
    UINT32 missed; /* missed reading counter, cleared each time value is updated */
    BOOL fresh;    /* TRUE when value has been updated and not read */
    BOOL live;     /* FALSE until we get at least one valid reading */
} READING;

#define MAX_MISSED 1 /* OK to miss one reading */

typedef struct {
    READING aerrs;
    READING aes01;
    READING aea01;
    READING aef01;
    READING aev01;
    READING aec01;
    READING ae101;
    READING ae201;
    READING aes02;
    READING aea02;
    READING aef02;
    READING aev02;
    READING aec02;
} SCAN;

/* For building packets */

#define NNAME_LEN 2
#define SNAME_LEN 4
#define CNAME_LEN 3
#define LNAME_LEN 2
#define IDENT_LEN (NNAME_LEN+1 + SNAME_LEN+1 + CNAME_LEN + LNAME_LEN)

typedef struct {
    char ident[IDENT_LEN+1];      /* stream identifier */
    UINT8 gain;                   /* conversion gain for real values */
    UINT64 tons;                  /* expected time of next sample */
    int nsamp;                    /* number of samples collected so far */
    struct {
        int tofs;                 /* index to timestamp field buf */
        int nsamp;                /* index to nsamp field of buf */
        int data;                 /* index to next datum field of buf */
    } offset;
    UINT8 buf[IDA10_FIXEDRECLEN]; /* the packet */
} PROTO_PACKET;

/* Function prototypes */

/* bground.c */
void BackGround(BOOL daemon);

/* exit.c */
void SetShutdownFlag(void);
void SetFlushFlag(void);
BOOL FlushFlagSet(void);
void QuitOnShutdown(INT32 status);
void Exit(INT32 status);
void InitExit(void);

/* log.c */
BOOL DebugEnabled(void);
void SetDebugFlag(BOOL value);
void LogMsg(char *format, ...);
void LogDebug(char *format, ...);
LOGIO *GetLogHandle(void);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* moxa.c */
void LogScan(SCAN *scan);
SCAN *LockScan(void);
void UnlockScan(void);
void StartMoxaThread(SERVER *server);

/* packet.c */
void FlushPacket(PROTO_PACKET *pkt);
void FlushAllPackets(void);
void StuffScan(IDA10_GENTAG *ttag, SCAN *scan, UINT64 nsint);
void StartPacketPusher(SERVER *dl, LOGIO *lp, int depth, char *sname, char *nname, UINT64 nsint);

/* signals.c */
void StartSignalHandler(void);

#endif /* qhlp_h_included */

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
 * $Log: tristar10.h,v $
 * Revision 1.7  2015/12/18 18:51:47  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.6  2015/12/09 18:44:33  dechavez
 * cleaned up prototypes
 *
 * Revision 1.5  2012/07/25 18:11:42  dechavez
 * added DEFAULT_USER
 *
 * Revision 1.4  2012/05/02 18:30:49  dechavez
 * *** initial production release ***
 *
 * Revision 1.3  2012/04/26 17:35:40  dechavez
 * split sample into slave1 and slave2, and added aex channel
 *
 * Revision 1.2  2012/04/25 21:27:30  dechavez
 * changed default moxa IP to 192.168.1.20
 *
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
