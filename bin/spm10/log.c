#pragma ident "$Id: log.c,v 1.3 2016/04/28 23:23:08 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "spm10.h"

#define MY_MOD_ID SPM10_MOD_LOG

static LOGIO lp;

VOID LogMsgLevel(int level)
{
    logioSetThreshold(&lp, level);
}

VOID LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug)
{
    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    logioSetPrefix(&lp, prefix);

    if (debug) logioSetThreshold(&lp, LOG_DEBUG);
    LogMsg(LOG_INFO, "Newmar SPM-200 Disk Loop Writer - %s (%s %s)", VersionIdentString, __DATE__, __TIME__);

    return &lp;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * Revision 1.3  2016/04/28 23:23:08  dechavez
 * fixed model number in the startup message
 *
 * Revision 1.2  2016/04/28 23:18:09  dechavez
 * Added startup log message
 *
 * Revision 1.1  2016/04/28 23:01:49  dechavez
 * initial release
 *
 */
