#pragma ident "$Id: id.c,v 1.5 2016/02/03 17:40:57 dechavez Exp $"
/*======================================================================
 *
 *  Report digitizer indentifiers
 *
 *====================================================================*/
#include "q330cc.h"

void IdHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ -v ]\n", name);
}

BOOL VerifyId(Q330 *q330)
{
char *arg;

    q330->cmd.code = Q330_CMD_ID;
    switch (q330->cmd.arglst->count) {
      case 0:
        q330->cmd.p32 = 0;
        return TRUE;
      case 1:
        arg = (char *) q330->cmd.arglst->array[0];
        if (strcasecmp(arg, "-v") == 0) {
            q330->cmd.p32 = 1;
        } else {
            IdHelp(q330->cmd.name, 0, NULL);
        }
        return TRUE;
    }

    IdHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void id(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C1_FIX fix;

    if (!qdp_C1_RQFIX(q330->qdp, &fix)) {
        printf("qdp_C1_RQFIX failed\n");
    } else {
        if (q330->cmd.p32) printf("   Q330 Tag#  Serial Number    Sys  Slv\n");
        printf(" %6s", q330->addr.connect.ident);
        printf(" %4u", fix.proper_tag);
        printf(" %016llX", fix.sys_num);
        printf(" %d.%03d", fix.sys_ver.major, fix.sys_ver.minor);
        printf(" %d.%02d", fix.sp_ver.major, fix.sp_ver.minor);
        printf("\n");
    }
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
 * $Log: id.c,v $
 * Revision 1.5  2016/02/03 17:40:57  dechavez
 * changes related to the introduction of QDP_SOFTVER structures for version numbers
 *
 * Revision 1.4  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.3  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.2  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 * Revision 1.1  2009/07/10 18:34:25  dechavez
 * created
 *
 */
