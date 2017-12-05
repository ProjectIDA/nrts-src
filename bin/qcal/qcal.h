#pragma ident "$Id: qcal.h,v 1.11 2016/09/01 17:06:33 dechavez Exp $"
#ifndef qcal_h_included
#define qcal_h_included

#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"
#include "ida.h"
#include "logio.h"
#include "msgq.h"
#include "q330.h"
#include "qdp.h"
#include "qdplus.h"
#include "e300.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2010 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define QCAL_MOD_MAIN       ((INT32)  100)
#define QCAL_MOD_INIT       ((INT32)  200)
#define QCAL_MOD_LOG        ((INT32)  300)
#define QCAL_MOD_SIGNALS    ((INT32)  400)
#define QCAL_MOD_DATA       ((INT32)  500)
#define QCAL_MOD_IDA        ((INT32)  600)
#define QCAL_MOD_MSEED      ((INT32)  700)
#define QCAL_MOD_QDP        ((INT32)  800)
#define QCAL_MOD_E300       ((INT32)  900)

#define QCAL_STATUS_RUNNING ((INT32) 9999)

/* output formats */

#define OUTPUT_IDA10 0x01
#define OUTPUT_QDP   0x02
#define OUTPUT_MSEED 0x04
#define DEFAULT_OUTPUT OUTPUT_MSEED

/* Run time parameters */

typedef struct {
    Q330_CFG *cfg;
    Q330_ADDR addr;
    QDP *qdp; /* configuration port connection */
    QDP_TYPE_C1_QCAL c1_qcal;
    char *digitizer;
    int port;
    int debug;
    LOGIO *lp;
    QDP_LCQ lcq;
    struct {
        char prefix[MAXPATHLEN+1];
        char qdp[MAXPATHLEN+1];
        char ida[MAXPATHLEN+1];
        char log[MAXPATHLEN+1];
        char mseed[MAXPATHLEN+1];
    } name;
    struct {
        char *tokens; /* name of token set in lcq database */
        char *sta;    /* override existing site name */
        char *net;    /* override existing network code */
        QDP_TYPE_DPORT saved; /* original data port configuration */
        QDP_TYPE_DPORT work;  /* modified data port configuraiton */
    } dport;
    QDP_TYPE_C1_FIX fix;
    int output;
} QCAL;

/* Default parameters */

#define DEFAULT_DEBUG   QDP_TERSE
#define DEFAULT_PORT    QDP_LOGICAL_PORT_4
#define DEFAULT_OPTIONS QDP_HLP_RULE_FLAG_LCASE

/*  Function prototypes  */

/* calib.c */
void PrintCalib(FILE *fp, QDP_TYPE_C1_QCAL *c1_qcal);
BOOL LaunchCal(QDP *ConfigPortConnection, QCAL *qcal, BOOL HaveE300Control);
void AbortCal(QDP *ConfigPortConnection);

/* data.c */
void ShutdownDataConnection(QCAL *qcal);
BOOL StartDataThread(QCAL *qcal);

/* e300.c */
BOOL E300Control(Q330_CALIB *calib);

/* exit.c */
void SetWatchdogExitDelay(UINT32 value);
void SetDeleteFlag(void);
void SetExitStatus(INT32 status);
INT32 ExitStatus(void);
void Exit(INT32 status);
VOID InitExit(QCAL *pqcal);

/* help.c */
void help(char *myname);

/* ida.c */
void CloseIDA(QCAL *qcal);
BOOL OpenIDA(QCAL *qcal, UINT32 options);

/* init.c */
void init(int argc, char **argv, QCAL *qcal);

/* log.c */
VOID LogMsg(char *format, ...);
LOGIO *InitLogging(char *myname, char *spec);

/* mseed.c */
void CloseMSEED(QCAL *qcal);
void GenerateMSEED(UINT8 *ida10);
BOOL OpenMSEED(QCAL *qcal);

/* names.c */
void BuildFileNames(QCAL *qcal, char *digitizer, char *caltag);

/* qdp.c */
QDP *RegisterWithConfigPort(QCAL *qcal);
void SetCalibrationInitialized(UINT32 seqno);
BOOL CalInProgress(QDP_PKT *pkt);
void CloseQDP(QCAL *qcal);
BOOL OpenQDP(QCAL *qcal);
void WriteQDP(ISI_RAW_PACKET *raw);

/* signals.c */
VOID StartSignalHandler(QCAL *qcal);

/* tokens.c */
BOOL ConfigureDataPort(QDP *ConfigPortConnection, QCAL *qcal);
BOOL RestoreDataPort(QDP *ConfigPortConnection, QCAL *qcal);

#endif /* qcal_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: qcal.h,v $
 * Revision 1.11  2016/09/01 17:06:33  dechavez
 * added QDP handle for config port connection to QCAL typedef, updated prototypes
 *
 * Revision 1.10  2015/12/07 19:00:01  dechavez
 * added miniSEED support, introduced "output" field for QCAL, updated prototypes
 *
 * Revision 1.9  2014/10/21 21:55:31  dechavez
 * added e300.c support
 *
 * Revision 1.8  2014/06/12 20:19:00  dechavez
 * added DEFAULT_OPTIONS (QDP_HLP_RULE_FLAG_LCASE) and updated openQDP() prototype
 *
 * Revision 1.7  2012/06/24 18:28:56  dechavez
 * Modified to use libqdp 3.4.0 where data port and tokens are bound together
 * and main channel output frequencies are forced to agree with token set
 *
 * Revision 1.6  2012/01/11 19:21:45  dechavez
 * updated prototypes
 *
 * Revision 1.5  2011/04/14 19:21:05  dechavez
 * added QCAL.token support
 *
 * Revision 1.4  2010/09/30 17:25:29  dechavez
 * updated prototypes
 *
 * Revision 1.3  2010/04/12 21:08:21  dechavez
 * removed verbose field, added QCAL_STATUS_RUNNING
 *
 * Revision 1.2  2010/04/01 20:26:15  dechavez
 * updated prototypes
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
