#pragma ident "$Id: log.c,v 1.2 2015/12/07 19:01:55 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_LOG

static LOGIO lp;

LOGIO *GetLogHandle()
{
    return &lp;
}

VOID LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

LOGIO *InitLogging(char *myname, char *spec)
{
    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    LogMsg("qcal - %s", VersionIdentString);
    LogMsg("Build %s %s", VersionIdentString, __DATE__, __TIME__);

    return &lp;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * Revision 1.2  2015/12/07 19:01:55  dechavez
 * rework version banner to label build date/time as such
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
