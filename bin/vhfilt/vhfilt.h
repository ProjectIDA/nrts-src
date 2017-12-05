#pragma ident "$Id: vhfilt.h,v 1.2 2015/10/30 22:28:08 dechavez Exp $"
#ifndef vhfilt_h_included
#define vhfilt_h_included

#include "mseed.h"
#include "filter.h"
#include "util.h"

#ifdef DEBUG_TEST
#define LogErr printf
#endif

extern char *VersionIdentString;
extern int LogLevel;

#define VHFILT_MOD_MAIN       100
#define VHFILT_MOD_READ       200
#define VHFILT_MOD_REJECT     300
#define VHFILT_MOD_PROCESS    400
#define VHFILT_MOD_OUTPUT     500

#define VHFILT_NPASS          2

typedef struct {
    char *path;     /* if not NULL, path to FILTER file */
    FILTER filter;  /* FILTER data for this pass */
    int decimation; /* decimation factor */
} PASS;

typedef struct {
    INT64 delay;             /* total filter delay in nanoseconds */
    PASS pass[VHFILT_NPASS]; /* cascade of filters to use */
    int minsamp;             /* minimum number of input samples */
} VHFILT;

typedef struct {
    MSEED_HDR hdr; /* header describing dat */
    INT32 *dat;    /* continuous samples */
    int skip;      /* number of input samples to skip to get "nice" start time */
    INT64 nice;    /* the "nice" start time */
} TIME_SERIES;

/* Function prototypes */

/* exit.c */
void GracefulExit(int status);

/* loadf.c */
void LoadFilters(VHFILT *vhfilt);

/* log.c */
LOGIO *LogHandle(void);
void LogErr(char *format, ...);
void LogMsg(char *format, ...);
LOGIO *InitLogging(char *myname, BOOL verbose_flag);

/* output.c */
void SetRecordLength(int reclen);
void OutputData(FILE *fp, TIME_SERIES *ts);

/* process.c */
void Process(TIME_SERIES *ts, VHFILT *vhfilt, FILE *fp);

/* read.c */
LNKLST *ReadInput(void);

/* reject.c */
void RejectUnsupportedFormat(MSEED_RECORD *record);

#endif /* vhfilt_h_included */

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
 * $Log: vhfilt.h,v $
 * Revision 1.2  2015/10/30 22:28:08  dechavez
 * initial production release
 *
 * Revision 1.1  2015/10/30 19:45:41  dechavez
 * checkpoint commit
 *
 */
