#pragma ident "$Id: msdmx.h,v 1.2 2015/12/05 00:32:35 dechavez Exp $"
#ifndef msdmx_h_defined
#define msdmx_h_defined

extern char *VersionIdentString;
extern int LogLevel;

#include "cssio.h"
#include "util.h"
#include "ida10.h"
#include "mseed.h"

#define MSDMX_MOD_MAIN      100
#define MSDMX_MOD_HANDLE    200
#define MSDMX_MOD_PROCESS   300
#define MSDMX_MOD_FILES     400
#define MSDMX_MOD_WRITE     500
#define MSDMX_MOD_WFDISC    600

#define MSDMX_DEFAULT_OUTDIR   "dmxdata"
#define MSDMX_LOG_FILE_NAME    "log"
#define MSDMX_DATA_DIR_NAME    "data"
#define MSDMX_WFDISC_FILE_NAME "css.wfdisc"

/*  Structure templates  */

typedef struct {
    MSEED_HDR hdr;           /* copy of current record header */
    WFDISC wfdisc;           /* CSS 3.0 wfdisc */
    char path[MAXPATHLEN+1]; /* path to .w file for this stream */
    FILE *fp;                /* fopen()'d path */
} MSDMX_HANDLE;

struct wflist {
    char   name[MAXPATHLEN+2]; /* .w file this wfdisc rec refers to */
    struct cssio_wfdisc28 wfdisc;    /* the wfdisc record                 */
    struct wflist *next;       /* pointer to next element           */
};

/*  Function prototypes  */

/* exit.c */
void IncrementCounter(void);
void GracefulExit(int status);

/* files.c */
FILE *OpenWfdiscFile(void);
char *LogPathName(void);
char *BaseDir(void);
void CreateOutputDirectory(char *outdir);

/* handle.c */
void FlushHandle(MSDMX_HANDLE *handle);
void CloseHandle(MSDMX_HANDLE *handle);
void CloseAllHandles(void);
void ReInitHandle(MSDMX_HANDLE *handle, MSEED_RECORD *record);
MSDMX_HANDLE *GetHandle(MSEED_RECORD *record);
MSDMX_HANDLE *CreateHandle(MSEED_RECORD *record);

/* log.c */
void LogErr(char *format, ...);
void LogMsg(char *format, ...);
void DebugMsg(char *format, ...);
void InitLogging(char *myname);

/* process.c */
void ProcessRecord(MSEED_RECORD *record);

/* wfdisc.c */
void InitWfdisc(WFDISC *wfdisc, MSEED_HDR *hdr);

/* write.c */
void WriteData(MSDMX_HANDLE *handle, MSEED_RECORD *record);

#endif /* msdmx_h_defined */

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
 * $Log: msdmx.h,v $
 * Revision 1.2  2015/12/05 00:32:35  dechavez
 * added some missing prototypes
 *
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
