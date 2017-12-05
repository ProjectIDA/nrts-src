#pragma ident "$Id: calib.c,v 1.3 2015/12/07 19:46:37 dechavez Exp $"
/*======================================================================
 *
 *  Calibration control
 *
 *====================================================================*/
#include "qmon.h"
#include "e300.h"

static char *shortcut = NULL;
Q330_CALIB calib;

static void HelpMesg(QMON *qmon)
{
static char *details = 
" wf = waveform (0=sine 1=red 2=white 3=step 4=random)\n"
"amp = amplitude bit shift (0=-6db 1=-12db 2=-18db etc)\n"
"dur = duration in seconds\n"
"set = settling time in seconds\n"
"trl = trailer time in seconds\n"
"chn = calibration channel bitmap (eg. 7 = chans 1, 2, 3)\n"
"mon = monitor channel bitmap (eg. 8 = chan 4)\n"
"div = frequency divisor\n"
"      (for sine, freq = 1/divisor, all others, freq = 125/divisor, in Hz)";

    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "usage: cal start { wf amp dur set trl chn mon div | shortcut }\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "       cal sc   (to list shortcuts)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "       cal stop (to abort active or pending calibration)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "cal start parameters\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", details);

    PopupMessage(qmon, qmon->poptxt);
}

static int Verify(QMON *qmon, int argc, char **argv)
{
int cmd;

    switch (argc) {
      case 2:
        if (strcasecmp(argv[1], "stop") == 0) {
            cmd = Q330_CMD_CAL_STOP;
        } else if (strcasecmp(argv[1], "abort") == 0) {
            cmd = Q330_CMD_CAL_STOP;
        } else if (strcasecmp(argv[1], "sc") == 0) {
            cmd = Q330_CMD_CAL_LSCUT;
        } else if (strcasecmp(argv[1], "shortcut") == 0) {
            cmd = Q330_CMD_CAL_LSCUT;
        } else if (strcasecmp(argv[1], "shortcuts") == 0) {
            cmd = Q330_CMD_CAL_LSCUT;
        } else {
            cmd = Q330_CMD_ERROR;
        }
        break;

      case 3:
      case 10:
        if (strcasecmp(argv[1], "start") == 0) {
            cmd = Q330_CMD_CAL_START;
        } else {
            cmd = Q330_CMD_ERROR;
        }
        if (argc == 3) {
            shortcut = argv[2];
        } else {
            shortcut = NULL;
            calib.qcal.waveform  = (UINT16) atoi(argv[2]);
            calib.qcal.amplitude = (UINT16) atoi(argv[3]);
            calib.qcal.duration  = (UINT16) atoi(argv[4]);
            calib.qcal.settle    = (UINT16) atoi(argv[5]);
            calib.qcal.trailer   = (UINT16) atoi(argv[6]);
            calib.qcal.chans     = (UINT16) atoi(argv[7]);
            calib.qcal.monitor   = (UINT16) atoi(argv[8]);
            calib.qcal.divisor   = (UINT16) atoi(argv[9]);
            calib.input.e300.present = FALSE;
        }
        cmd = Q330_CMD_CAL_START;
        break;

      default:
        cmd = Q330_CMD_ERROR;
    }

if (cmd == Q330_CMD_ERROR) debug("bad cal syntax (argc=%d)\n", argc);
    return cmd;

}

static void calStart(QMON *qmon)
{
QDP_PKT pkt;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_STAT stat;
QDP_TYPE_C1_GLOB glob;
static struct { UINT16 a; UINT16 b; } mask = {0x07, 0x38};

/* If using a shortcut, load cal parameters */

    if (shortcut != NULL && !q330GetCalib(shortcut, qmon->active->addr, qmon->cfg, &calib)) {
        sprintf(qmon->poptxt, "shortcut '%s' not defined for sensor A (%s)\n", shortcut, qmon->active->addr->input.a.name);
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "use `cal sc' for a list of valid shortcuts");
        PopupMessage(qmon, qmon->poptxt);
        return;
    }

/* Make sure we aren't already running a calibration */

    qdpInit_C1_STAT(&stat);

    if (!qdp_C1_RQSTAT(qmon->qdp, QDP_SRB_GBL, &stat)) {
        PopupMessage(qmon, "ERROR: Unable to query Q330 for global status");
        return;
    }
    if ((stat.bitmap & QDP_SRB_GBL) == 0) {
        PopupMessage(qmon, "ERROR: qdp_C1_RQSTAT(QDP_SRB_GBL) yields no QDP_SRB_GBL!");
        return;
    }

    if (stat.gbl.cal_stat != 0) {
        PopupMessage(qmon, "digitizer already has an active or pending calibration");
        return;
    }

/* Determine start time, taking into account settling time */

    calib.qcal.starttime = stat.gbl.tstamp + calib.qcal.settle; /* immediate start */

/* Set sensor control lines (if defined) */

    if (!qdp_C1_RQGLOB(qmon->qdp, &glob)) {
        PopupMessage(qmon, "ERROR: unable to retrieve global programming!\n");
        return;
    }
    if (!qdp_C1_RQSC(qmon->qdp, sc)) {
        PopupMessage(qmon, "ERROR: unable to retrieve sensor control mapping!\n");
        return;
    }
    calib.qcal.sensor = glob.sensor_map;

    if (calib.qcal.chans & mask.a) qdpUpdateSensorControlBitmap(&calib.qcal.sensor, sc,  QDP_SC_SENSOR_A_CALIB);
    if (calib.qcal.chans & mask.b) qdpUpdateSensorControlBitmap(&calib.qcal.sensor, sc,  QDP_SC_SENSOR_B_CALIB);

/* If E330 present, enable external calibration */

    if (calib.input.e300.present && !e300Extcal(calib.input.e300.name)) {
        sprintf(qmon->poptxt, "ERROR: unable to configure E330 '%s' for external calibration", calib.input.e300.name);
        PopupMessage(qmon, qmon->poptxt);
        return;
    }

/* Build and issue calibration start command */

    qdpEncode_C1_QCAL(&pkt, &calib.qcal);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "ERROR: calibration start command failed");
        return;
    }
    q330CalibParamString(&calib.qcal, qmon->poptxt);
    PopupMessage(qmon, qmon->poptxt);
}

static void calAbort(QMON *qmon)
{
QDP_PKT pkt;

    qdpInitPkt(&pkt, QDP_C1_STOP, 0, 0);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) PopupMessage(qmon, "ERROR: calibration stop command failed");
}

static void calListShortcuts(QMON *qmon)
{
Q330_CALIB *shortcut;
LNKLST_NODE *crnt;
char *sensorA;

    sensorA = qmon->active->addr->input.a.name;

    sprintf(qmon->poptxt,"      calibration shortcuts      \n");
    strcat(qmon->poptxt, "      ---------------------      \n");
    strcat(qmon->poptxt, "  name wf  amp    dur set trl div\n");
    crnt = listFirstNode(qmon->cfg->calib);
    while (crnt != NULL) {
        shortcut = (Q330_CALIB *) crnt->payload;
        if (strcasecmp(shortcut->input.name, qmon->active->addr->input.a.name) == 0) {
            sprintf(qmon->poptxt+strlen(qmon->poptxt), "%6s", shortcut->name);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %2d", shortcut->qcal.waveform);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %4d", shortcut->qcal.amplitude);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %6d", shortcut->qcal.duration);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %3d", shortcut->qcal.settle);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %3d", shortcut->qcal.trailer);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %3d", shortcut->qcal.divisor);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), "\n");
        }
        crnt = listNextNode(crnt);
    }

    PopupMessage(qmon, qmon->poptxt);
}

void ExecuteCalib(QMON *qmon, int argc, char **argv)
{
int cmd;
    
    if ((cmd = Verify(qmon, argc, argv)) == Q330_CMD_ERROR) {
        HelpMesg(qmon);
        return;
    }

    switch (cmd) {
        case Q330_CMD_CAL_START: calStart(qmon); break;
        case Q330_CMD_CAL_STOP:  calAbort(qmon); break;
        case Q330_CMD_CAL_LSCUT: calListShortcuts(qmon); break;
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
 * $Log: calib.c,v $
 * Revision 1.3  2015/12/07 19:46:37  dechavez
 * added parenthesis around status bitmap test to calm OS X
 *
 * Revision 1.2  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
