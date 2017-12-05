#pragma ident "$Id: log.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "siomet.h"

static LOGIO lp;
static int level = LOG_INFO;

void LogCommandLine(int argc, char **argv)
{
int i;
char message[MAXPATHLEN+1];

    sprintf(message, "working directory: ");
    getcwd(message+strlen(message), MAXPATHLEN-strlen(message));
    LogMsg(message);

    sprintf(message, "command line:     ");
    for (i = 0; i < argc; i++) sprintf(message+strlen(message), " %s", argv[i]);
    LogMsg(message);
}

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

void LogMsgLevel(int newlevel)
{
    level = newlevel;
    logioMsg(&lp, level, "log level %d\n", level);
}

LOGIO *InitLogging(char *myname, char *path, char *SITE)
{
static char *fid = "InitLogging";

    if (!logioInit(&lp, path, NULL, myname)) {
        fprintf(stderr, "%s: logioInit: %s\n", fid, strerror(errno));
        exit(1);
    }
    logioSetPrefix(&lp, SITE);
    logioMsg(&lp, level, "%s - %s", myname, VersionIdentString);
    logioMsg(&lp, level, "Build %s %s", __DATE__, __TIME__);

    return &lp;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
