#pragma ident "$Id: log.c,v 1.2 2015/09/02 21:40:31 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "isistatd.h"

#define MY_MOD_ID ISISTATD_MOD_LOG 

static LOGIO lp;
static int threshold = 0;

void LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    if (level > threshold) return;

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

void LogMsgLevel(int newlevel)
{
    threshold = newlevel;
    LogMsg(threshold, "log level %d\n", threshold);
}

void IncreaseLogVerbosity(void)
{
    LogMsgLevel(threshold + 1);
}

void LogRunParameters(int level, int argc, char **argv, ISISTATD_PAR *par)
{
int i;
char message[MAXPATHLEN+1];

    LogMsg(threshold, "%s - %s", argv[0], VersionIdentString);
    LogMsg(threshold, "Build %s %s", __DATE__, __TIME__);

    sprintf(message, "command line:    ");
    for (i = 0; i < argc; i++) sprintf(message+strlen(message), " %s", argv[i]);
    LogMsg(level, message);
    LogMsg(level, "ISI data server: %s:%d\n", par->server, par->port);
    LogMsg(level, "API url:         %s\n", par->apiurl);
    LogMsg(level, "Query interval:  %d second%c\n", par->interval, par->interval > 1 ? 's' : ' ');
}

LOGIO *InitLogging(char *myname, char *path, int level)
{
static char *fid = "InitLogging";

    threshold = level;

    if (!logioInit(&lp, path, NULL, myname)) {
        fprintf(stderr, "%s: logioInit: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }

    return &lp;
}

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
 * $Log: log.c,v $
 * Revision 1.2  2015/09/02 21:40:31  dechavez
 * checkpoint commit
 *
 * Revision 1.1  2015/09/02 21:13:12  dechavez
 * checkpoint commit
 *
 */
