#pragma ident "$Id: imseed.h,v 1.10 2017/11/01 21:35:25 dechavez Exp $"
#ifndef imseed_h_included
#define imseed_h_included

#include "ida.h"
#include "dbio.h"
#include "ida10.h"
#include "isi.h"
#include "mseed.h"
#include "dmc/libmseed.h"

extern char *VersionIdentString;

#define MOD_MAIN     1000
#define MOD_CONVERT  1100
#define MOD_PROCESS  1200
#define MOD_WRITE    1300
#define MOD_IDA10    1400
#define MOD_IDA      1500
#define MOD_SIGNALS  1600

#define DEFAULT_REV    10
#define DEFAULT_RECLEN 512
#define DEFAULT_NET    "II"
#define DEFAULT_QCODE  'R'
#define DEFAULT_LOG    "stderr"

/* Function prototypes */

/* exit.c */
void IncrementInputCount(void);
void IncrementBadCount(void);
UINT32 InputCount(void);
void GracefulExit(int status);

/* log.c */
void LogCommandLine(int argc, char **argv);
void LogMsg(char *format, ...);
void LogErr(char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, BOOL verbose);

/* process.c */
void ProcessIDA1012(MSEED_HANDLE *handle, UINT8 *src);
void ProcessRecord(MSEED_HANDLE *handle, MSEED_RECORD *record);

/* signals.c */
void StartSignalHandler(void);

/* write.c */
void CloseFiles(void);
void WriteRecord(void *unused, MSEED_PACKED *packed);
int InitWriter(char *use_root);

#endif /* imseed_h_included */

/* Revision History
 *
 * $Log: imseed.h,v $
 * Revision 1.10  2017/11/01 21:35:25  dechavez
 * added ProcessIDA1012() prototype
 *
 * Revision 1.9  2015/05/21 21:55:16  dechavez
 * added IncrementBadCount() prototype
 *
 * Revision 1.8  2014/08/19 18:18:52  dechavez
 * use MSEED_PACKED instead of MSEED_RECORD
 *
 * Revision 1.7  2014/08/11 20:45:06  dechavez
 * Rework to support libmseed 2.0.0
 *
 * Revision 1.6  2014/04/18 19:21:28  dechavez
 * Included rechan option description on help message, suppressed the (unused)
 * options for specifying clock thresholds (locked=percent suspect=percent)
 *
 * Revision 1.5  2014/04/14 19:12:07  dechavez
 * updated ConvertxToMSTrace() to include optional channel map for renaming
 *
 * Revision 1.4  2014/03/06 18:51:48  dechavez
 * rework to accomodate the move of template.c over to libmseed
 *
 * Revision 1.3  2014/03/04 22:07:47  dechavez
 * added clock locked/suspect fields to Global parameters
 *
 * Revision 1.2  2014/03/04 21:10:43  dechavez
 * Major rework to make use of the new MSFlags field in the modified libmseed
 * which allows propagating input conditions to the MiniSEED FSDH flags and
 * blockette 1001
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
