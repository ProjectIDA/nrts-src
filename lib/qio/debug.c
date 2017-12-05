#pragma ident "$Id: debug.c,v 1.2 2016/06/16 23:02:30 dechavez Exp $"
/*======================================================================
 *
 *  Library logging facility
 *
 *====================================================================*/
#include "qio.h"

VOID qioDebug(QIO *handle, int level, char *message, ...)
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

VOID qioDebugContents(QIO *handle, char *buf, int buflen)
{
    if (handle == NULL || buf == NULL || buflen < 1) return;
    if (handle->debug < QIO_DEBUG_VERBOSE) return;

    logioHexDump(handle->lp, LOG_INFO, (UINT8 *) buf, buflen);
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
 * $Log: debug.c,v $
 * Revision 1.2  2016/06/16 23:02:30  dechavez
 * cast to calm Darwin compiles
 *
 * Revision 1.1  2016/06/15 21:19:41  dechavez
 * initial release
 *
 */
