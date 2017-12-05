#pragma ident "$Id: push.h,v 1.1 2011/08/04 22:07:22 dechavez Exp $"
#ifndef push_included
#define push_included

#include "isi.h"
#include "ida10.h"
#include "util.h"
#include "logio.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2011 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define PUSH_MOD_MAIN      ((INT32)  100)
#define PUSH_MOD_INIT      ((INT32)  200)
#define PUSH_MOD_LOG       ((INT32)  300)
#define PUSH_MOD_SIGNALS   ((INT32)  400)

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/*  Function prototypes  */

/* exit.c */
INT32 ExitStatus(void);
void SetExitStatus(INT32 status);
void Exit(INT32 status);
VOID InitExit(void);

/* init.c */
ISI_PUSH *init(char *myname, int argc, char **argv);

/* log.c */
LOGIO *GetLogHandle(void);
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec);

/* signals.c */
void StartSignalHandler(void);

#endif /* push_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: push.h,v $
 * Revision 1.1  2011/08/04 22:07:22  dechavez
 * initial release
 *
 */
