#pragma ident "$Id: debug.c,v 1.2 2010/10/20 18:35:31 dechavez Exp $"
/*======================================================================
 *
 *  Debug logging
 *
 *====================================================================*/
#include "qmon.h"

static LOGIO lp;
static char *LogFile = NULL;

void debug(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    if (LogFile == NULL) return;

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);

}

void InitDebug(char *myname, char *path)
{
FILE *fp;

    if (path == NULL) return;

    LogFile = path;

    if ((fp = fopen(LogFile, "w")) == NULL) {
        perror("LogFile");
        exit(1);
    }
    fclose(fp);

    if (!logioInit(&lp, LogFile, NULL, myname)) {
        perror("logioInit");
        exit(1);
    }

}

LOGIO *dbglp(void)
{
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
 * $Log: debug.c,v $
 * Revision 1.2  2010/10/20 18:35:31  dechavez
 * log to user specified file, convert to no ops if not specified
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
