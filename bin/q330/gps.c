#pragma ident "$Id: gps.c,v 1.14 2016/01/21 19:37:49 dechavez Exp $"
/*======================================================================
 *
 *  Q330 GPS control
 *
 *====================================================================*/
#include "q330cc.h"

void GpsHelp(char *name, int argc, char **argv)
{
    printf("\n");
    printf("usage: %s { status | on | off | cold | id | cnf | internal | external }\n", name);
    printf("\n");
    printf("  status - GPS status (equivalent to the 'status gps' command)\n");
    printf("      on - turn GPS receiver on\n");
    printf("     off - turn GPS receiver off\n");
    printf("    cold - cold-start GPS receiver\n");
    printf("      id - print GPS id strings\n");
    printf("     cnf - print GSP configuration\n");
    printf("internal - configure GPS to use internal receiver\n");
    printf("external - configure GPS to use external receiver\n");
    printf("\n");
}

BOOL VerifyGps(Q330 *q330)
{
char *arg;
static QDP_TYPE_C2_GPS DefaultInternal = Q330_DEFAULT_TYPE_C2_GPS_INTERNAL;
static QDP_TYPE_C2_GPS DefaultExternal = Q330_DEFAULT_TYPE_C2_GPS_EXTERNAL;

    if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);

    arg = (char *) q330->cmd.arglst->array[0];
    if (strcasecmp(arg, "status") == 0) {
        q330->cmd.p32 = QDP_SRB_GPS;
        q330->cmd.code = Q330_CMD_STATUS;
    } else if (strcasecmp(arg, "on") == 0) {
        q330->cmd.code = Q330_CMD_GPS_ON;
    } else if (strcasecmp(arg, "off") == 0) {
        q330->cmd.code = Q330_CMD_GPS_OFF;
    } else if (strcasecmp(arg, "cold") == 0) {
        q330->cmd.code = Q330_CMD_GPS_COLD;
    } else if (strcasecmp(arg, "id") == 0) {
        q330->cmd.code = Q330_CMD_GPS_ID;
    } else if (strcasecmp(arg, "cnf") == 0 || strcasecmp(arg, "cfg") == 0) {
        q330->cmd.code = Q330_CMD_GPS_CNF;
        q330->cmd.update = FALSE;
    } else if (strcasecmp(arg, "internal") == 0) {
        q330->cmd.gps = DefaultInternal;
        q330->cmd.update = TRUE;
    } else if (strcasecmp(arg, "external") == 0) {
        q330->cmd.gps = DefaultExternal;
        q330->cmd.update = TRUE;
    } else if (strcasecmp(arg, "help") == 0) {
        GpsHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    } else {
        return UnexpectedArg(q330, arg);
    }

    return TRUE;
}

void gpsON(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_ON, TRUE)) {
        printf("gps ON command failed\n");
    } else {
        printf("gps ON command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
    }
}

void gpsOFF(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_OFF, TRUE)) {
        printf("gps OFF command failed\n");
    } else {
        printf("gps OFF command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
    }
}

void gpsColdStart(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_COLDSTART, TRUE)) {
        printf("gps cold start command failed\n");
    } else {
        printf("gps cold start command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
    }
}

void gpsCnf(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C2_GPS gps;

    if (q330->cmd.update) {
        qdpEncode_C2_GPS(&pkt, &q330->cmd.gps);
        if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
            printf("qdp_C1_SGPS failed\n");
            return;
        }
    }

    if (!qdp_C2_RQGPS(q330->qdp, &gps)) {
        printf("qdp_C2_RQGPS failed\n");
    } else {
        qdpPrint_C2_GPS(stdout, &gps);
    }
}

void gpsId(Q330 *q330)
{
QDP_TYPE_C1_GID gid[QDP_NGID];

    if (!qdp_C1_RQGID(q330->qdp, gid)) {
        printf("qdp_C1_RQGID failed\n");
    } else {
        qdpPrint_C1_GID(stdout, gid);
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
 * $Log: gps.c,v $
 * Revision 1.14  2016/01/21 19:37:49  dechavez
 * removed "cnf" prefix to internal and external commands, made "cfg" an alias for "cnf"
 *
 * Revision 1.13  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.12  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.11  2010/12/13 23:03:32  dechavez
 * use defaults from include/q330.h
 *
 * Revision 1.10  2010/08/18 18:09:55  dechavez
 * Added status to "help gps".
 *
 * Revision 1.9  2009/10/20 22:11:56  dechavez
 * use QDP_SRB_x macros instead of deprecated QDP_STATUS_y
 *
 * Revision 1.8  2009/07/25 17:39:44  dechavez
 * removed tabs
 *
 * Revision 1.7  2009/07/23 21:44:24  dechavez
 * allow for gps status
 *
 * Revision 1.6  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 * Revision 1.5  2009/02/23 22:08:34  dechavez
 * permit setting GPS parameters via internal/external option to gps cnf
 *
 * Revision 1.4  2009/02/04 17:48:33  dechavez
 * added cnf and id subcommands
 *
 * Revision 1.3  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
