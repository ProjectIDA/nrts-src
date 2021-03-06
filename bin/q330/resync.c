#pragma ident "$Id: resync.c,v 1.6 2011/01/25 18:26:15 dechavez Exp $"
/*======================================================================
 *
 *  Resync Q330
 *
 *====================================================================*/
#include "q330cc.h"

void ResyncHelp(char *name, int argc, char **argv)
{
    printf("usage: %s\n", name);
    printf("Re-sync Q330.\n");
}

BOOL VerifyResync(Q330 *q330)
{
    q330->cmd.code = Q330_CMD_RESYNC;
    if (q330->cmd.arglst->count != 0) return BadArgLstCount(q330);
    return TRUE;
}

void resync(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_RESYNC, TRUE)) {
        printf("resync command failed\n");
    } else {
        printf("resync command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: resync.c,v $
 * Revision 1.6  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.5  2010/12/13 22:58:53  dechavez
 * fixed error message typo
 *
 * Revision 1.4  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.3  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
