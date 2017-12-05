#pragma ident "$Id: status.c,v 1.9 2017/09/28 20:41:08 dauerbach Exp $"
/*======================================================================
 *
 *  Request Status
 *
 *====================================================================*/
#include "q330cc.h"

void StatusHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ full | { global ps mp gps gsv pll arp dp1 dp2 dp3 dp4 enet ss sp1 sp2 sp3 ep } ]\n", name);
    printf("\n");
    printf("  full - complete report (scrolls off the screen)\n");
    printf("global - global status\n");
    printf("    ps - power supply status\n");
    printf("    mp - boom positions, temperatures and voltages\n");
    printf("   gps - GPS status\n");
    printf("   gsv - GPS satellites\n");
    printf("   pll - PLL status\n");
    printf("   arp - ARP table\n");
    printf("   dp1 - data port 1 status\n");
    printf("   dp2 - data port 2 status\n");
    printf("   dp3 - data port 3 status\n");
    printf("   dp4 - data port 4 status\n");
    printf("  enet - ethernet status\n");
    printf("   sp1 - serial port 1 status\n");
    printf("   sp2 - serial port 2 status\n");
    printf("   sp3 - serial port 3 status\n");
    printf("    ss - serial sensor status\n");
    printf("    ep - environmental processor status\n");
    printf("\n");
}

BOOL VerifyStatus(Q330 *q330)
{
int i;
char *arg;
UINT32 bitmap = QDP_SRB_DEFAULT | QDP_SRB_DP1;

    q330->cmd.code = Q330_CMD_STATUS;
    if (q330->cmd.arglst->count == 0) {
        q330->cmd.p32 = bitmap;
        return TRUE;
    }

    q330->cmd.p32 = 0;
    for (i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];

        if (strcasecmp(arg, "full") == 0) {
            q330->cmd.p32 |= bitmap;
        } else if (strcasecmp(arg, "help") == 0) {
            StatusHelp(q330->cmd.name, 0, NULL);
            return FALSE;
        } else if (strcasecmp(arg, "?") == 0) {
            StatusHelp(q330->cmd.name, 0, NULL);
            return FALSE;
        } else if (strcasecmp(arg, "global") == 0) {
            q330->cmd.p32 |= QDP_SRB_GBL;
        } else if (strcasecmp(arg, "ps") == 0) {
            q330->cmd.p32 |= QDP_SRB_PWR;
        } else if (strcasecmp(arg, "mp") == 0) {
            q330->cmd.p32 |= QDP_SRB_BOOM;
        } else if (strcasecmp(arg, "gps") == 0) {
            q330->cmd.p32 |= QDP_SRB_GPS;
        } else if (strcasecmp(arg, "gsv") == 0) {
            q330->cmd.p32 |= QDP_SRB_GSV;
        } else if (strcasecmp(arg, "pll") == 0) {
            q330->cmd.p32 |= QDP_SRB_PLL;
        } else if (strcasecmp(arg, "arp") == 0) {
            q330->cmd.p32 |= QDP_SRB_ARP;
        } else if (strcasecmp(arg, "dp1") == 0) {
            q330->cmd.p32 |= QDP_SRB_DP1;
        } else if (strcasecmp(arg, "dp2") == 0) {
            q330->cmd.p32 |= QDP_SRB_DP2;
        } else if (strcasecmp(arg, "dp3") == 0) {
            q330->cmd.p32 |= QDP_SRB_DP3;
        } else if (strcasecmp(arg, "dp4") == 0) {
            q330->cmd.p32 |= QDP_SRB_DP4;
        } else if (strcasecmp(arg, "sp1") == 0) {
            q330->cmd.p32 |= QDP_SRB_SP1;
        } else if (strcasecmp(arg, "sp2") == 0) {
            q330->cmd.p32 |= QDP_SRB_SP2;
        } else if (strcasecmp(arg, "sp3") == 0) {
            q330->cmd.p32 |= QDP_SRB_SP3;
        } else if (strcasecmp(arg, "enet") == 0) {
            q330->cmd.p32 |= QDP_SRB_ETH;
        } else if (strcasecmp(arg, "ss") == 0) {
            q330->cmd.p32 |= QDP_SRB_SS;
        } else if (strcasecmp(arg, "ep") == 0) {
            q330->cmd.p32 |= QDP_SRB_EP;
        } else {
            printf("ERROR: unrecognized %s argument '%s'\n", q330->cmd.name, arg);
            StatusHelp(q330->cmd.name, 0, NULL);
            return FALSE;
        }
    }

    return q330->cmd.p32 ? TRUE : FALSE;
}

void status(Q330 *q330)
{
int i;
QDP_TYPE_C1_STAT stat;

/* !!!!!!!!!!!!!!! */
/* need to check if EPis only status requested and whether EP is supported by q330 in q330->qdp->flags */

    if (q330->cmd.p32 == (q330->cmd.p32 & QDP_SRB_EP)) {
        if (!(q330->qdp->flags & QDP_FLAGS_EP_SUPPORTED)) {
            printf("QEP Not Supported. Digitizer firmware update required. (minimum ver 1.146)\n");
            return;
        }
    }

    qdpInit_C1_STAT(&stat);

    if (!qdp_C1_RQSTAT(q330->qdp, q330->cmd.p32, &stat)) {
        printf("qdp_C1_RQSTAT failed\n");
        return;
    } else {
        qdpPrint_C1_STAT(stdout, &stat);
    }

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
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
 * $Log: status.c,v $
 * Revision 1.9  2017/09/28 20:41:08  dauerbach
 * added version checks for EP commands
 *
 * Revision 1.8  2016/01/19 23:14:20  dechavez
 * added ep (environmental processor) support
 *
 * Revision 1.7  2010/03/31 20:46:35  dechavez
 * iniitialize status structure before use
 *
 * Revision 1.6  2009/10/20 22:13:47  dechavez
 * added serial sensor status support, switched to QDP_SRB_x macros
 *
 * Revision 1.5  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.4  2009/02/04 17:47:20  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.3  2008/10/03 19:41:41  dechavez
 * Allow specifying an arbitrary number of status selectors
 *
 * Revision 1.2  2008/10/03 17:13:57  dechavez
 * Added gsv command, removed satellites from gps selection
 *
 * Revision 1.1  2008/10/02 23:04:26  dechavez
 * created
 *
 */
