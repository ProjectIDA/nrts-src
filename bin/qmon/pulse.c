#pragma ident "$Id: pulse.c,v 1.4 2014/01/30 19:54:19 dechavez Exp $"
/*======================================================================
 *
 *  Pulse control lines
 *
 *====================================================================*/
#include "qmon.h"

static void HelpMesg(QMON *qmon, int argc, char **argv)
{
int i;

    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "usage: pulse sensor line duration\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "  sensor - a or b\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "    line - center, calib, lock, unlock, deploy, remove\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "duration - duration in seconds\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "example: pulse a center 2.00\n");
    PopupMessage(qmon, qmon->poptxt);
}

static UINT32 SelectLine(char *sensor, char *what)
{
UINT32 line;

    if (strcasecmp(sensor, "a") == 0) {
        if (strcasecmp(what, "center") == 0) {
            line = QDP_SC_SENSOR_A_CENTER;
        } else if (strcasecmp(what, "calib") == 0) {
            line = QDP_SC_SENSOR_A_CALIB;
        } else if (strcasecmp(what, "lock") == 0) {
            line = QDP_SC_SENSOR_A_LOCK;
        } else if (strcasecmp(what, "unlock") == 0) {
            line = QDP_SC_SENSOR_A_UNLOCK;
        } else if (strcasecmp(what, "deploy") == 0) {
            line = QDP_SC_SENSOR_A_DEPREM;
        } else if (strcasecmp(what, "remove") == 0) {
            line = QDP_SC_SENSOR_A_DEPREM;
        } else if (strcasecmp(what, "deprem") == 0) {
            line = QDP_SC_SENSOR_A_DEPREM;
        } else {
            line = QDP_SC_UNDEFINED;
        }
    } else if (strcasecmp(sensor, "b") == 0) {
        if (strcasecmp(what, "center") == 0) {
            line = QDP_SC_SENSOR_B_CENTER;
        } else if (strcasecmp(what, "calib") == 0) {
            line = QDP_SC_SENSOR_B_CALIB;
        } else if (strcasecmp(what, "lock") == 0) {
            line = QDP_SC_SENSOR_B_LOCK;
        } else if (strcasecmp(what, "unlock") == 0) {
            line = QDP_SC_SENSOR_B_UNLOCK;
        } else if (strcasecmp(what, "deploy") == 0) {
            line = QDP_SC_SENSOR_B_DEPREM;
        } else if (strcasecmp(what, "remove") == 0) {
            line = QDP_SC_SENSOR_B_DEPREM;
        } else if (strcasecmp(what, "deprem") == 0) {
            line = QDP_SC_SENSOR_B_DEPREM;
        } else {
            line = QDP_SC_UNDEFINED;
        }
    } else {
        line = QDP_SC_UNDEFINED;
    }

    return line;
}

void ExecutePulse(QMON *qmon, int argc, char **argv)
{
UINT32 line;
QDP_PKT pkt;
UINT16 bitmap, duration;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_GLOB glob;

    if (argc != 4) {
        HelpMesg(qmon, argc, argv);
        return;
    }

    if ((line = SelectLine(argv[1], argv[2])) == QDP_SC_UNDEFINED) {
        HelpMesg(qmon, argc, argv);
        return;
    }

    duration = (UINT16) (atof(argv[3]) * 100);

    if (!qdp_C1_RQSC(qmon->qdp, sc)) {
        PopupMessage(qmon, "ERROR: unable to retrieve sensor control mapping!\n");
        return;
    }
    if (!qdp_C1_RQGLOB(qmon->qdp, &glob)) {
        PopupMessage(qmon, "ERROR: unable to retrieve global programming!\n");
        return;
    }
    bitmap = glob.sensor_map;

    if (!qdpUpdateSensorControlBitmap(&bitmap, sc, line)) {
        sprintf(qmon->poptxt, "ERROR: %s line is not defined.", qdpSensorControlString(line));
        PopupMessage(qmon, qmon->poptxt);
        return;
    }

    qdpEncode_C1_PULSE(&pkt, bitmap, duration);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "ERROR: pulse sensor control lines failed");
        return;
    }

    sprintf(qmon->poptxt, "%s line pulsed for %.2f seconds", qdpSensorControlString(line), duration / 100.0);
    PopupMessage(qmon, qmon->poptxt);
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
 * $Log: pulse.c,v $
 * Revision 1.4  2014/01/30 19:54:19  dechavez
 * added deploy and remove to help line
 *
 * Revision 1.3  2014/01/23 23:59:38  dechavez
 * added support for STS-5A sensor control lines
 *
 * Revision 1.2  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
