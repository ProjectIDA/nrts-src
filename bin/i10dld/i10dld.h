#pragma ident "$Id: i10dld.h,v 1.3 2018/01/11 19:05:19 dechavez Exp $"
#ifndef i10dld_h_included
#define i10dld_h_included
#include "util.h"
#include "logio.h"
#include "ida10.h"
#include "isi.h"
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2018 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define MOD_MAIN    ((INT32)  100)
#define MOD_DL      ((INT32)  200)
#define MOD_SIGNALS ((INT32)  300)

#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DB             "/usr/nrts"
#define DEFAULT_USER           "nrts"
#define DEFAULT_DAEMON         FALSE
#define DEFAULT_VERBOSE        FALSE
#define DEFAULT_SEEDLINK       TRUE

#define UNDEFINED_PORT       -1

#define DEFAULT_OPTIONS       0
#define OPTION_LAXNRTS        0x01

/* Function prototypes */

/* debug.c */
void LogIDA10PacketHeader(UINT8 *raw, UINT64 count);

/* dl.c */
void ToggleDLWriteVerbosity(void);
ISI_DL *OpenDiskLoop(char *dbspec, char *myname, char *site, LOGIO *lp, int flags, char *seedlink, BOOL verbose);
void CloseDiskLoop(void);
BOOL USER_SUPPLIED_WRITE_FUNCTION(ISI_DL *dl, ISI_RAW_PACKET *raw);

/* exit.c */
void GrabGlobalMutex(char *fid);
void ReleaseGlobalMutex(char *fid);
void SetExitStatus(int status);
void CheckExitStatus();
void Exit(INT32 status);
void InitExit(void);

/* log.c */
void LogMsg(char *format, ...);
void LogCommandLine(int argc, char **argv);
LOGIO *InitLogging(char *myname, char *spec, char *SITE);

/* signals.c */
void StartSignalHandler(void);
void StartSignalHandler(void);

/* server.c */

#endif /* i10dld_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2018 Regents of the University of California            |
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
 * $Log: i10dld.h,v $
 * Revision 1.3  2018/01/11 19:05:19  dechavez
 * updated prototypes and defines
 *
 * Revision 1.2  2018/01/10 23:52:41  dechavez
 * first production release
 *
 * Revision 1.1  2018/01/10 21:20:18  dechavez
 * created
 *
 */
