#pragma ident "$Id: combo.c,v 1.1 2016/06/23 20:31:38 dechavez Exp $"
/*======================================================================
 *
 *  Request and print COMBO packet
 *
 *====================================================================*/
#include "q330cc.h"

void ComboHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ 1 2 3 4 ]\n", name);
}

BOOL VerifyCombo(Q330 *q330)
{
char *arg;

    q330->cmd.code = Q330_CMD_COMBO;
    if (q330->cmd.arglst->count == 1) {
        arg = (char *) q330->cmd.arglst->array[0];
        if ((q330->cmd.p32 = qdpDataPortNumber(atoi(arg))) != QDP_UNDEFINED_PORT) return TRUE;
    }

    ComboHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void combo(Q330 *q330)
{
int i;
QDP_TYPE_C1_COMBO combo;

    if (!qdp_C1_RQCOMBO(q330->qdp, &combo, (UINT16) q330->cmd.p32)) {
        printf("qdp_C1_RQFLGS failed\n");
        return;
    } else {
        qdpPrint_C1_COMBO(stdout, &combo);
    }
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
 * $Log: combo.c,v $
 * Revision 1.1  2016/06/23 20:31:38  dechavez
 * created
 *
 */
