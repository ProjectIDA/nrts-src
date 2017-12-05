#pragma ident "$Id: log.c,v 1.3 2014/06/12 21:37:57 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "tristar10.h"

#define MY_MOD_ID TRISTAR10_MOD_LOG

static LOGIO lp;
static BOOL DebugFlag = FALSE;
static MUTEX mutex;

BOOL DebugEnabled()
{
BOOL retval;

    MUTEX_LOCK(&mutex);
        retval = DebugFlag;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void SetDebugFlag(BOOL value)
{
    MUTEX_LOCK(&mutex);
        DebugFlag = value;
    MUTEX_UNLOCK(&mutex);

    LogMsg("debug flag %s", value ? "set" : "cleared");
}

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

void LogDebug(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    if (DebugEnabled()) {
        ptr = msgbuf;
        va_start(marker, format);
        vsprintf(ptr, format, marker);
        va_end(marker);

        logioMsg(&lp, LOG_INFO, msgbuf);
    }
}

LOGIO *GetLogHandle()
{
    return &lp;
}

LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug)
{
char *PREFIX;

    MUTEX_INIT(&mutex);
    DebugFlag = debug;

    if (!logioInit(&lp, spec, NULL, myname)) {
        perror("logioInit");
        exit(MY_MOD_ID + 1);
    }
    if ((PREFIX = strdup(prefix)) == NULL) {
        perror("strdup");
        exit(MY_MOD_ID + 2);
    }
    util_ucase(PREFIX);
    logioSetPrefix(&lp, PREFIX);
    free(PREFIX);

    LogMsg("Tristar to IDA10 disk loop writer - %s (%s %s)", VersionIdentString, __DATE__, __TIME__);

    return &lp;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * Revision 1.3  2014/06/12 21:37:57  dechavez
 * renamed flag to DebugFlag to resolve libmseed.h clash
 *
 * Revision 1.2  2012/05/02 18:30:48  dechavez
 * *** initial production release ***
 *
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
