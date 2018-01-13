#pragma ident "$Id: isistatd.h,v 1.4 2015/12/18 18:51:41 dechavez Exp $"
#ifndef isistatd_h_included
#define isistatd_h_included

#include <curl/curl.h>
#include "jansson.h"
#include "isi.h"
#include "util.h"

extern char *VersionIdentString;

/* Module Id's for generating meaningful exit codes */

#define ISISTATD_MOD_MAIN    ((INT32)  100)
#define ISISTATD_MOD_LOG     ((INT32)  200)
#define ISISTATD_MOD_SIGNALS ((INT32)  300)
#define ISISTATD_MOD_ISI     ((INT32)  400)
#define ISISTATD_MOD_PROCESS ((INT32)  500)

/* Default parameters */

#define DEFAULT_DAEMON         FALSE
#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_SERVER         "idahub.ucsd.edu"
#define DEFAULT_APIURL         "https://ida-staging.ucsd.edu/isi-status-api/api/isi/v1/sohs"
#define DEFAULT_PORT           ISI_DEFAULT_PORT
#define DEFAULT_USER           "nrts"
#define DEFAULT_INTERVAL       1

/* Handy structure to hold all the run time parameters */

typedef struct {
    char server[MAXPATHLEN+1];
    char *apiurl;
    int port;
    int interval;
    ISI_PARAM isipar;
} ISISTATD_PAR;

/* Function prototypes */

/* exit.c */
void GracefulExit(INT32 status);
void InitExitHandler(void);

/* isi.c */
LNKLST *RequestSOH(void);
void InitISI(ISISTATD_PAR *par);

/* log.c */
void LogMsg(int level, char *format, ...);
void LogMsgLevel(int newlevel);
void IncreaseLogVerbosity(void);
void LogRunParameters(int level, int argc, char **argv, ISISTATD_PAR *par);
LOGIO *InitLogging(char *myname, char *path, int level);

/* signals.c */
void StartSignalHandler(void);

/* soh.c */
void ProcessSohCnfLists(LNKLST *soh_list, LNKLST *cnf_list, char *apiurl);

/* soh2json.c */
void ProcessIsiInfo(char *server, char *apiurl, ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf);


#endif /* isistatd_h_included */

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
 * $Log: isistatd.h,v $
 * Revision 1.4  2015/12/18 18:51:41  dechavez
 * changed sylogd facility from LOG_USER to LOG_LOCAL0
 *
 * Revision 1.3  2015/10/14 19:42:43  dauerbach
 * chanbge default api URL protocol to https and set curl option to use TLS v1.2
 *
 * Revision 1.2  2015/09/15 19:36:42  dauerbach
 * Changed to use isiSoh and isiCnf functions that return Structs with arrays
 *
 * Revision 1.1  2015/09/02 21:13:12  dechavez
 * checkpoint commit
 *
 */
