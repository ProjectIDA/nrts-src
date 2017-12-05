#pragma ident "$Id: calib.c,v 1.13 2016/09/01 17:28:51 dechavez Exp $"
/*======================================================================
 *
 *  Calibration control
 *
 *====================================================================*/
#include "qcal.h"

typedef struct {
    char *text;
    char *description;
    UINT16 code;
} TEXT_MAP;

static TEXT_MAP WaveformMap[] = {
    {"sine",  "sine wave",     QDP_QCAL_SINE},
    {"red",   "red noise",     QDP_QCAL_RED},
    {"white", "white noise",   QDP_QCAL_WHITE},
    {"step",  "step",          QDP_QCAL_STEP},
    {"rb",    "random binary", QDP_QCAL_RB},
    {NULL, NULL, -1}
};

static char *WaveformString(UINT16 value)
{
int i;
static char *undefined = "UNDEFINED";

    for (i = 0; WaveformMap[i].text != NULL; i++) {
        if (value == WaveformMap[i].code) return WaveformMap[i].description;
    }
    return undefined;
}

static char *ChannelString(UINT16 bitmap)
{
int i;
static char none[] = " none";
static char string[64];

    if (bitmap == 0) return none;

    string[0] = 0;
    for (i = 0; i < QDP_NCHAN; i++) {
        if (bitmap & (1 << i)) sprintf(string+strlen(string), " %d", i+1);
    }
    return string;
}

static char *StartTimeString(UINT32 tstamp)
{
static char *now = "immediately";
static char string[64];

    return (tstamp == 0) ? now : utilLttostr(tstamp + QDP_EPOCH_TO_1970_EPOCH, 0, string);
}

static float FrequencyValue(QDP_TYPE_C1_QCAL *c1_qcal)
{
float base, result;

    base = c1_qcal->waveform == QDP_QCAL_SINE ? 1.0 : 125.0;
    result = base / c1_qcal->divisor;

    return result;
}

void PrintCalib(FILE *fp, QDP_TYPE_C1_QCAL *c1_qcal)
{
float freq;

    freq = FrequencyValue(c1_qcal);

    fprintf(fp, "\n");
    fprintf(fp, "        start time = %s\n", StartTimeString(c1_qcal->starttime));
    fprintf(fp, "          waveform = %s\n", WaveformString(c1_qcal->waveform));
    fprintf(fp, "         amplitude = -%ddb\n", 6 * (c1_qcal->amplitude + 1));
    fprintf(fp, "          duration = %hu seconds\n", c1_qcal->duration);
    fprintf(fp, "     settling time = %hu seconds\n", c1_qcal->settle);
    fprintf(fp, "      trailer time = %hu seconds\n", c1_qcal->trailer);
    fprintf(fp, "   calibrate chans = %s\n", ChannelString(c1_qcal->chans));
    fprintf(fp, "     monitor chans = %s\n", ChannelString(c1_qcal->monitor));
    fprintf(fp, "         frequency = %.4f Hz (%.4f sec period)\n", freq, 1.0 / freq);
    fprintf(fp, "    control bitmap = "); utilPrintBinUINT16(fp, c1_qcal->sensor); fprintf(fp, "\n");
    fprintf(fp, "\n");
    fflush(fp);
}

void LogCalib(char *path, QDP_TYPE_C1_QCAL *c1_qcal)
{
FILE *fp;

    if ((fp = fopen(path, "a+")) == NULL) return;
    PrintCalib(fp, c1_qcal);
    fclose(fp);
}

BOOL LaunchCal(QDP *ConfigPortConnection, QCAL *qcal, BOOL HaveE300Control)
{
QDP_PKT pkt;
int warning, errcode, suberr;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_STAT stat;
QDP_TYPE_C1_GLOB glob;
UINT32 min_start_time;
struct { UINT16 a; UINT16 b; } mask = {0x07, 0x38};
static char *fid = "LaunchCal";

    qdpInit_C1_STAT(&stat);

/* Grab global status for calibration state and current time */

    if (!qdp_C1_RQSTAT(ConfigPortConnection, QDP_SRB_GBL, &stat)) {
        LogMsg("Unable to query Q330 for current time.");
        return FALSE;
    }
    if ((stat.bitmap & QDP_SRB_GBL) == 0) {
        LogMsg("*** ERROR *** qdp_C1_RQSTAT(QDP_SRB_GBL) yields no QDP_SRB_GBL!");
        return FALSE;
    }

/* Make sure we aren't already running a calibration */

    if (stat.gbl.cal_stat != 0) {
        LogMsg("%s already has an active or pending calibration\n", qcal->addr.connect.ident);
        return FALSE;
    }

/* Determine start time, taking into account settling time */

    min_start_time = stat.gbl.tstamp + qcal->c1_qcal.settle;
    if (qcal->c1_qcal.starttime == 0) {
        qcal->c1_qcal.starttime = min_start_time;
    } else if (qcal->c1_qcal.starttime < min_start_time) {
        qcal->c1_qcal.starttime = min_start_time;
        LogMsg("*** requested start time has been adjusted to accomodate settling time ***");
    }

/* Set sensor control lines */

    if (!qdp_C1_RQGLOB(ConfigPortConnection, &glob)) {
        LogMsg("ERROR: unable to retrieve global programming!");
        return FALSE;
    }
    if (!qdp_C1_RQSC(ConfigPortConnection, sc)) {
        LogMsg("ERROR: unable to retrieve sensor control mapping!");
        return FALSE;
    }
    qcal->c1_qcal.sensor = glob.sensor_map;

    if (!HaveE300Control) {
        warning = 0;
        if ((qcal->c1_qcal.chans & mask.a) && !qdpUpdateSensorControlBitmap(&qcal->c1_qcal.sensor, sc,  QDP_SC_SENSOR_A_CALIB)) {
            LogMsg("*** sensor A calibration enable line is not defined ***");
            ++warning;
        }
        if ((qcal->c1_qcal.chans & mask.b) && !qdpUpdateSensorControlBitmap(&qcal->c1_qcal.sensor, sc,  QDP_SC_SENSOR_B_CALIB)) {
            if (!warning)
            LogMsg("*** sensor B calibration enable line is not defined ***");
            ++warning;
        }
        if (warning) {
            LogMsg("The above warning is of no concern if the sensor does not require cal enable (eg, STS-1),");
            LogMsg("otherwise you may want to verify the sensor control mapping ('sc' command)");
        }
    }

/* Flush the data port */

    if (!qdpFlushDataPort(ConfigPortConnection, qcal->port, TRUE)) {
        LogMsg("ERROR: flush %s command failed", qdpPortString(qcal->port));
        return FALSE;
    }

/* Build and issue calibration start command */

    qdpEncode_C1_QCAL(&pkt, &qcal->c1_qcal);
    if (!qdpPostCmd(ConfigPortConnection, &pkt, TRUE)) {
        LogMsg("ERROR: calibration start command failed");
        return FALSE;
    } else {
        LogMsg("calibration start command sent to Q330 %s (S/N %016llX)\n", qcal->addr.connect.ident, qcal->addr.serialno);
        LogCalib(qcal->name.log, &qcal->c1_qcal);
        SetCalibrationInitialized(stat.gbl.cur_sequence);
    }

    return TRUE;
}

void AbortCal(QDP *ConfigPortConnection)
{
QDP_PKT pkt;
static char *fid = "AbortCal";

    qdpInitPkt(&pkt, QDP_C1_STOP, 0, 0);
    if (!qdpPostCmd(ConfigPortConnection, &pkt, TRUE)) {
        LogMsg("ERROR: calibration stop command failed");
    } else {
        LogMsg("calibration aborted");
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
 * Revision 1.13  2016/09/01 17:28:51  dechavez
 * don't call q330Register() directly anymore, instead use previously opened
 * ConfigPortConnection handle passed via arguments
 *
 * Revision 1.12  2016/08/04 22:31:01  dechavez
 * libqdp 3.13.1 and libq330 3.7.1 changes (basically added regerr to q330Register calls)
 *
 * Revision 1.11  2016/06/23 20:27:59  dechavez
 * deregister without waiting for ACK (thinking that this could be the source
 * of some of the occasional hanging exits)
 *
 * Revision 1.10  2015/12/07 19:09:38  dechavez
 * added parenthesis around status bitmap test to calm OS X
 *
 * Revision 1.9  2011/04/14 19:22:09  dechavez
 * added QCAL.token support
 *
 * Revision 1.8  2011/02/03 17:57:59  dechavez
 * use qdpShutdown() instead of qdpClose()
 *
 * Revision 1.7  2011/01/25 18:47:20  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.6  2010/11/15 23:05:46  dechavez
 * Flush data port before sending calibration start command
 *
 * Revision 1.5  2010/09/30 17:26:39  dechavez
 * don't print warning about sensor control lines if this calibration includes E300
 *
 * Revision 1.4  2010/08/25 18:19:22  dechavez
 * Fixed missing return code for successful LaunchCal().  Worked before only
 * because most boxes would leave something other than zero behind... found
 * this when trying it on the WRAB SunFire.
 *
 * Revision 1.3  2010/04/12 21:11:05  dechavez
 * removed console verbosity
 *
 * Revision 1.2  2010/04/01 20:32:00  dechavez
 * clean deregistration when finished, added AbortCal()
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
