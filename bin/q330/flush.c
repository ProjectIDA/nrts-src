#pragma ident "$Id: flush.c,v 1.5 2014/08/11 19:20:26 dechavez Exp $"
/*======================================================================
 *
 *  Flush a data port
 *
 *====================================================================*/
#include "q330cc.h"

void FlushHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ 1 2 3 4 ]\n", name);
}

BOOL VerifyFlush(Q330 *q330)
{
char *arg;

    q330->cmd.code = Q330_CMD_FLUSH;
    if (q330->cmd.arglst->count == 1) {
        arg = (char *) q330->cmd.arglst->array[0];
        if ((q330->cmd.p32 = qdpDataPortNumber(atoi(arg))) != QDP_UNDEFINED_PORT) return TRUE;
    }

    FlushHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void flush(Q330 *q330)
{
    if (!qdpFlushDataPort(q330->qdp, q330->cmd.p32, TRUE)) {
        printf("qdpFlushDataPort failed\n");
        return;
    } 
    printf("flush %s command sent to Q330 %s (S/N %016llX)\n", qdpPortString(q330->cmd.p32), q330->addr.connect.ident, q330->addr.serialno);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: flush.c,v $
 * Revision 1.5  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.4  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.3  2010/12/23 21:55:07  dechavez
 * cleaned up presentation
 *
 * Revision 1.2  2010/12/23 00:06:16  dechavez
 * use qdpDataPortNumber() to assign data port parameter
 *
 * Revision 1.1  2010/11/15 23:07:08  dechavez
 * initial release
 *
 */
