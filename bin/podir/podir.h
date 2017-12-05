#pragma ident "$Id: podir.h,v 1.5 2015/09/30 20:25:46 dechavez Exp $"
#ifndef podir_h_included
#define podir_h_included

#include "db.h"
#define HAVE_STRLCPY /* Antelope includes its own prototype */
#include "dccdb.h"
#include "mseed.h"

extern char *VersionIdentString;
extern int LogLevel;

#define PODIR_MOD_MAIN    100
#define PODIR_MOD_SITE    200
#define PODIR_MOD_CHAN    300
#define PODIR_MOD_CASCADE 400
#define PODIR_MOD_STAGE   500
#define PODIR_MOD_B50     600
#define PODIR_MOD_B52     700
#define PODIR_MOD_B53     800
#define PODIR_MOD_B54     900
#define PODIR_MOD_B57    1000
#define PODIR_MOD_B58    1100

#define PODIR_DEFAULT_THRESHOLD   1
#define PODIR_DEFAULT_DB_PATH     "/ida/dcc/db/IDA"
#define PODIR_DEFAULT_NETWORK     "II"

#define PODIR_HEADER_DIR_NAME     "HDR000"
#define PODIR_HA_FILE_NAME        PODIR_HEADER_DIR_NAME"/H.A"
#define PODIR_REQ_FILE_NAME       "POD.req"
#define PODIR_LOG_CHN_NAME        "LOG"

#define PODIR_B52_CLOCK_TOLERANCE_FACTOR    ((REAL64) 5.e-5)
#define PODIR_B52_CLOCK_TOLERANCE(x) (PODIR_B52_CLOCK_TOLERANCE_FACTOR / (x))

#define PODIR_ACTION_EXCLUDE 1
#define PODIR_ACTION_INCLUDE 2

/* Function prototypes */

/* b50.c */
void WriteBlockette50(FILE *fp, char *fpath, DCCDB_SITE *site, int net_code, char *net_id);

/* b52.c */
void WriteBlockette52(FILE *fp, char *fpath, DCCDB *db, DCCDB_SITE *site, DCCDB_CHAN *chan, DCCDB_STAGE *stage, REAL64 srate);

/* b53.c */
void WriteBlockette53(FILE *fp, char *fpath, DCCDB *db, DCCDB_STAGE *stage, REAL64 freq, REAL64 a0, FILTER_PANDZ *filter);

/* b54.c */
void WriteBlockette54(FILE *fp, char *fpath, DCCDB *db, DCCDB_STAGE *stage, FILTER_COEFF *filter);

/* b57.c */
void WriteBlockette57(FILE *fp, char *fpath, DCCDB_STAGE *stage, REAL64 delay);

/* b58.c */
void WriteBlockette58(FILE *fp, char *fpath, int stageid, REAL64 gain, REAL64 freq);

/* chan.c */
void ProcessChan(DCCDB *db, DCCDB_SITE *site, DCCDB_CHAN *chan, char *root);

/* exit.c */
void GracefulExit(int status);

/* ha.c */
BOOL CreateHA(DCCDB *db, char *outdir);

/* log.c */
void LogRunParameters(int argc, char **argv, char *dbpath, char *workdir);
void LogErr(char *format, ...);
void LogMsg(int level, char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, int level);

/* req.c */
BOOL CreateReq(DCCDB *db, char *outdir, char *net_id);
void CloseReq(void);
void UpdateReq(DCCDB_CASCADE *cascade);

/* select.c */
BOOL ProcessThisSite(DCCDB_SITE *site);
char *StationSelectionString(char *buf, int buflen);
BOOL BuildStationSelectionList(char *input, int ActionFlag, BOOL CaseFlag);

/* site.c */
void ProcessSite(DCCDB *db, DCCDB_SITE *site, char *net_id, char *root);

/* stage.c */
void ProcessStage(DCCDB *db, FILE *fp, char *fpath, DCCDB_SITE *site, DCCDB_CHAN *chan, DCCDB_STAGE *stage, REAL64 freq);

/* util.c */
INT64 ConvertDccdbBegtTo1999Nsec(REAL64 tstamp);
INT64 ConvertDccdbEndtTo1999Nsec(REAL64 tstamp);

#endif /* podir_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: podir.h,v $
 * Revision 1.5  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
