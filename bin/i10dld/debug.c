#pragma ident "$Id: debug.c,v 1.1 2018/01/10 21:20:18 dechavez Exp $"
/*======================================================================
 *
 * log IDA10 packet headers
 *
 *====================================================================*/
#include "i10dld.h"

#define MY_MOD_ID MOD_DEBUG

void LogIDA10PacketHeader(UINT8 *raw, UINT64 count)
{
int i;
IDA10_TS ts;
char msgbuf[1024], *message;

    ida10UnpackTS(raw, &ts);
    message = ida10TStoString(&ts, msgbuf);
    // replace % (as in 100%) to avoid confusing the LogMsg() variable arg processor
    for (i = 0; i < strlen(message); i++) if (message[i] == '%') message[i] = 'p';
    LogMsg("%llu: %s", count, message);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2018 Regents of the University of California            |
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
 * Revision 1.1  2018/01/10 21:20:18  dechavez
 * created
 *
 */
