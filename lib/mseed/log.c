#pragma ident "$Id: log.c,v 1.2 2014/10/29 21:24:40 dechavez Exp $"
/*======================================================================
 *
 *  Library logging facility
 *
 *====================================================================*/
#include "mseed.h"

VOID mseedLog(MSEED_HANDLE *handle, int level, char *message, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    if (handle == NULL || message == NULL) return;

    if (handle->debug < level) return;

    ptr = msgbuf;
    va_start(marker, message);
    vsprintf(ptr, message, marker);
    va_end(marker);

    logioMsg(handle->lp, LOG_INFO, msgbuf);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * Revision 1.2  2014/10/29 21:24:40  dechavez
 * changed name of "format" argument to "message", for better clarity
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
