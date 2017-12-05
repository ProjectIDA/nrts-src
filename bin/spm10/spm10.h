#pragma ident "$Id: spm10.h,v 1.2 2016/11/16 21:58:27 dechavez Exp $"
#ifndef spm10_h_included
#define spm10_h_included

#include "util.h"
#include "ida10.h"
#include "isi.h"
#include "iacp.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2016 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define SPM10_MOD_MAIN      ((INT32)  100)
#define SPM10_MOD_LOG       ((INT32)  200)
#define SPM10_MOD_SIGNALS   ((INT32)  300)
#define SPM10_MOD_PKT       ((INT32)  400)
#define SPM10_MOD_PROCESS   ((INT32)  500)

/* options */

#define SPM10_OPTION_LCASE  0x00000001

/* default parameters */

#define DEFAULT_POLL_SCRIPT    "PollSPM"
#define DEFAULT_PEER           "spm"
#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "stderr"
#define DEFAULT_USER           "nrts"
#define DEFAULT_NET            "II"
#define DEFAULT_OPTIONS        SPM10_OPTION_LCASE
#define DEFAULT_LOGRAW         FALSE
#define DEFAULT_DEPTH          20

/* structure for building data packets */

#define NNAME_LEN 2
#define SNAME_LEN 4
#define CNAME_LEN 3
#define LNAME_LEN 2
#define IDENT_LEN (NNAME_LEN+1 + SNAME_LEN+1 + CNAME_LEN + LNAME_LEN)
       
typedef struct {
    char ident[IDENT_LEN+1];      /* stream identifier */
    UINT64 tons;                  /* expected time of next sample */
    UINT64 sint;                  /* sample interval */
    int nsamp;                    /* number of samples collected so far */
    struct {
        int tofs;                 /* index to timestamp field buf */
        int nsamp;                /* index to nsamp field of buf */
        int data;                 /* index to next datum field of buf */
    } offset;
    UINT8 buf[IDA10_FIXEDRECLEN]; /* the packet */
} PROTO_PACKET;
       
/* generic time stamp */

typedef struct {
    int year;
    int month;
    int day;
    int jday;
    int hour;
    int minute;
    float second;
    UINT8 status;
    IDA10_GENTAG gentag;
} TIMESTAMP;
       
/* one raw datum, from the engine */

typedef struct {
    char ident[IDENT_LEN+1];            /* stream identifier */
    char nname[NNAME_LEN+1];            /* network code */
    char sname[SNAME_LEN+1];            /* station code */
    char cname[CNAME_LEN+1];            /* channel code */
    char lname[LNAME_LEN+1];            /* location code */
    char chnloc[CNAME_LEN+LNAME_LEN+1]; /* channel + location combined */
    TIMESTAMP tstamp;                   /* time stamp data */
    INT32 value;                        /* data sample */
    UINT64 sint;                        /* sample interval, nanosec */
    UINT8 gain;                         /* float to int conversion gain */
} LINE_SAMPLE;

/*  Function prototypes  */

/* exit.c */
BOOL ShutdownInProgress(void);
void SetPipeFILE(FILE *fp);
void GracefulExit(INT32 status);
VOID InitExit(void);

/* log.c */
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);
       
/* output.c */
BOOL SetDlOutput(char *string, LOGIO *lp);
void FlushPacket(PROTO_PACKET *pkt);

/* pkt.c */
void ResetPacket(PROTO_PACKET *pkt);
void AppendSample(LINE_SAMPLE *sample);
void InitPacketBuilder(void);
void FlushAllPackets(void);

/* process.c */
void ToggleRawLogOption(void);
void InitStringProcessor(BOOL initialLogRawOption);
void ProcessString(char *input, char *sta, char *net, UINT32 options);

/* signals.c */
void SignalHandler(int sig);
VOID StartSignalHandler(VOID);

/* util.c */
void BuildIdentString(char *dest, char *nname, char *sname, char *cname);
void BuildGentag(IDA10_GENTAG *dest, TIMESTAMP *tstamp);

#endif /* spm10_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: spm10.h,v $
 * Revision 1.2  2016/11/16 21:58:27  dechavez
 * added missing prototypes
 *
 * Revision 1.1  2016/04/28 23:01:50  dechavez
 * initial release
 *
 */
