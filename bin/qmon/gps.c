#pragma ident "$Id: gps.c,v 1.2 2016/01/04 21:22:37 dechavez Exp $"
/*======================================================================
 *
 *  GPS control
 *
 *====================================================================*/
#include "qmon.h"

static void HelpMesg(QMON *qmon)
{
static char *text =
"usage: gps { on | off | cold | id | sv | internal | external }\n"
"      on - turn GPS receiver on                              \n"
"     off - turn GPS receiver off                             \n"
"    cold - cold-start GPS receiver                           \n"
"      id - display GPS id strings                            \n"
"      sv - display satellite info                            \n"
"internal - configure GPS to use internal receiver            \n"
"external - configure GPS to use external receiver            \n";

    PopupMessage(qmon, text);
}

static int Verify(QMON *qmon, int argc, char **argv)
{
int cmd;

    switch (argc) {
      case 1:
        if (strcasecmp(argv[0], "gsv") == 0) {
            cmd = Q330_CMD_GPS_SV;
        } else {
            cmd = Q330_CMD_ERROR;
        }
        break;
      case 2:
        if (strcasecmp(argv[1], "on") == 0) {
            cmd = Q330_CMD_GPS_ON;
        } else if (strcasecmp(argv[1], "off") == 0) {
            cmd = Q330_CMD_GPS_OFF;
        } else if (strcasecmp(argv[1], "cold") == 0) {
            cmd = Q330_CMD_GPS_COLD;
        } else if (strcasecmp(argv[1], "id") == 0) {
            cmd = Q330_CMD_GPS_ID;
        } else if (strcasecmp(argv[1], "sv") == 0) {
            cmd = Q330_CMD_GPS_SV;
        } else if (strcasecmp(argv[1], "internal") == 0 || strcasecmp(argv[1], "int") == 0) {
            cmd = Q330_CMD_GPS_INT;
        } else if (strcasecmp(argv[1], "external") == 0 || strcasecmp(argv[1], "ext") == 0) {
            cmd = Q330_CMD_GPS_EXT;
        } else {
            cmd = Q330_CMD_ERROR;
        }
        break;
      default:
        cmd = Q330_CMD_ERROR;
    }

   return cmd;
}

static void gpsON(QMON *qmon)
{
    if (!qdpCtrl(qmon->qdp, QDP_CTRL_GPS_ON, TRUE)) PopupMessage(qmon, "gps ON command failed\n");
}

static void gpsOFF(QMON *qmon)
{
    if (!qdpCtrl(qmon->qdp, QDP_CTRL_GPS_OFF, TRUE)) PopupMessage(qmon, "gps OFF command failed\n");
}

static void gpsCOLD(QMON *qmon)
{
    if (!qdpCtrl(qmon->qdp, QDP_CTRL_GPS_COLDSTART, TRUE)) PopupMessage(qmon, "gps COLDSTART command failed\n");
}

void gpsCNF(QMON *qmon, QDP_TYPE_C2_GPS *cnf)
{
QDP_PKT pkt;

    qdpEncode_C2_GPS(&pkt, cnf);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "qdp_C1_SGPS failed\n");
    } else {
        qmon->active->flags |= UNSAVED_CHANGES;
    }
}

static void gpsID(QMON *qmon)
{
int i, numstr;
QDP_TYPE_C1_GID gid[QDP_NGID];

    if (!qdp_C1_RQGID(qmon->qdp, gid)) {
        PopupMessage(qmon, "qdp_C1_RQGID failed\n");
        return;
    }

    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt + strlen(qmon->poptxt), "GPS ID Strings\n");
    sprintf(qmon->poptxt + strlen(qmon->poptxt), "--------------\n");
    for (numstr = 0, i = 0; i < QDP_NGID; i++) {
        if (strlen(gid[i]) > 0) {
            sprintf(qmon->poptxt + strlen(qmon->poptxt), "%s\n", gid[i]);
            ++numstr;
        }
    }

    PopupMessage(qmon, qmon->poptxt);
}

static void gpsSV(QMON *qmon)
{
#define MAXENTRY (QMON_ROWS - 3)
int i, nentry;
QDP_TYPE_C1_STAT stat;

    qdpInit_C1_STAT(&stat);
    if (!qdp_C1_RQSTAT(qmon->qdp, QDP_SRB_GSV, &stat)) {
        PopupMessage(qmon, "qdp_C1_RQSTAT failed\n");
        return;
    }

    nentry = (stat.gsv.nentry > MAXENTRY) ? MAXENTRY : stat.gsv.nentry;

    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt + strlen(qmon->poptxt), "GPS Satellites\n");
    sprintf(qmon->poptxt + strlen(qmon->poptxt), "--------------\n");
    sprintf(qmon->poptxt + strlen(qmon->poptxt), "    Number Elevation Azimuth SNR\n");
    for (i = 0; i < nentry; i++) sprintf(qmon->poptxt + strlen(qmon->poptxt), "%2d: %6hu %9hu %7hu %3hu\n",
        i+1,
        stat.gsv.satl[i].num,
        stat.gsv.satl[i].elevation,
        stat.gsv.satl[i].azimuth,
        stat.gsv.satl[i].snr
    );

    PopupMessage(qmon, qmon->poptxt);
}

void ExecuteGps(QMON *qmon, int argc, char **argv)
{
int cmd;
static QDP_TYPE_C2_GPS internal = Q330_DEFAULT_TYPE_C2_GPS_INTERNAL;
static QDP_TYPE_C2_GPS external = Q330_DEFAULT_TYPE_C2_GPS_EXTERNAL;

    if ((cmd = Verify(qmon, argc, argv)) == Q330_CMD_ERROR) {
        HelpMesg(qmon);
        return;
    }

    switch (cmd) {
      case Q330_CMD_GPS_ON:   gpsON(qmon);             break;
      case Q330_CMD_GPS_OFF:  gpsOFF(qmon);            break;
      case Q330_CMD_GPS_COLD: gpsCOLD(qmon);           break;
      case Q330_CMD_GPS_ID:   gpsID(qmon);             break;
      case Q330_CMD_GPS_SV:   gpsSV(qmon);             break;
      case Q330_CMD_GPS_INT:  gpsCNF(qmon, &internal); break;
      case Q330_CMD_GPS_EXT:  gpsCNF(qmon, &external); break;
      default: PopupMessage(qmon, QMON_MSG_NOT_IMPLEMENTED);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * Revision 1.2  2016/01/04 21:22:37  dechavez
 * removed spurious 'cnf' prefixes from the internal and external help messages
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
