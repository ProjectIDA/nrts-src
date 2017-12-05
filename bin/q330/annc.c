#pragma ident "$Id: annc.c,v 1.1 2009/09/28 22:17:32 dechavez Exp $"
/*======================================================================
 *
 *  Tertiary command test harness
 *
 *====================================================================*/
#include "q330cc.h"

void TercHelp(char *name, int argc, char **argv)
{
    printf("usage: %s command [ remainder ]\n", name);
}

BOOL VerifyTerc(Q330 *q330)
{
char *arg;
static QDP_TYPE_C2_TERC DefaultTerc = {QDP_CX_XXXX, 0};

    q330->cmd.code = Q330_CMD_TERC;

    if (q330->cmd.arglst->count != 1 && q330->cmd.arglst->count != 2) return BadArgLstCount(q330);

    arg = (char *) q330->cmd.arglst->array[0];
    if ((q330->cmd.terc.command = (UINT16) qdpCmdCode(arg)) == QDP_CX_XXXX) {
        printf("Unknown command code '%s'\n", arg);
        return FALSE;
    }

    if (q330->cmd.arglst->count == 1) {
        arg = (char *) q330->cmd.arglst->array[0];
        q330->cmd.terc.remainder = (UINT32) atoi(arg);
    }

    return TRUE;
}

void terc(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C2_TERR terr;

    if (!qdp_C2_TERC(q330->qdp, &q330->cmd.terc, &terr)) {
        printf("qdp_C2_TERC failed\n");
        return;
    }

    printf("C2_TERR: command=0x%04x, response=0x%04x, remainder=0x%08x\n", terr.command, terr.response, terr.remainder);
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
 * $Log: annc.c,v $
 * Revision 1.1  2009/09/28 22:17:32  dechavez
 * added
 *
 */
