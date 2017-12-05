#pragma ident "$Id: calib.c,v 1.22 2014/08/11 19:20:26 dechavez Exp $"
/*======================================================================
 *
 *  Calibration control
 *
 *====================================================================*/
#include "q330cc.h"
#include "e300.h"

static struct { UINT16 a; UINT16 b; } mask = {0x07, 0x38};

static void CalibStartHelp(Q330_CFG *cfg, char *name)
{
static char *details =
"amp = amplitude bit shift (0=-6db 1=-12db etc)\n"
"dur = duration in seconds\n"
"set = settling time in seconds\n"
"trl = trailer time in seconds\n"
"chn = calibration channel bitmap (eg. 7 = chans 1, 2, 3)\n"
"mon = monitor channel bitmap (eg. 8 = chan 4)\n"
"div = frequency divisor (for sine, freq = 1/divisor, all others, freq = 125/divisor, in Hz)\n";

    printf("usage: %s start { now | yyyy:ddd-hh:mm:ss } { wf amp dur set trl chn mon div | shortcut }\n", name);
    printf("\n");
    printf(" wf = waveform (%d=sine %d=red noise %d=white noise %d=step %d=random binary)\n", QDP_QCAL_SINE, QDP_QCAL_RED, QDP_QCAL_WHITE, QDP_QCAL_STEP, QDP_QCAL_RB);
    printf("%s\n", details);
}

void CalibHelp(Q330_CFG *cfg, char *name, int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s { start { now | yyyy:ddd-hh:mm:ss } | stop | status } { cal parameters }\n", name);
    } else if (strcasecmp(argv[3], "start") == 0) {
        CalibStartHelp(cfg, name);
    } else if (strcasecmp(argv[3], "stop") == 0) {
        printf("usage: %s stop\n", name);
        printf("Aborts an active or pending calibration.\n");
    } else if (strcasecmp(argv[3], "status") == 0) {
        printf("usage: %s status\n", name);
        printf("Reports current calibration status.\n");
    }
}

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

static float FrequencyValue(QDP_TYPE_C1_QCAL *calib)
{
float base, result;

    base = calib->waveform == QDP_QCAL_SINE ? 1.0 : 125.0;
    result = base / calib->divisor;

    return result;
}

static void PrintCalib(QDP_TYPE_C1_QCAL *calib)
{
float freq;

    freq = FrequencyValue(calib);

    printf("        start time = %s\n", StartTimeString(calib->starttime));
    printf("          waveform = %s\n", WaveformString(calib->waveform));
    printf("         amplitude = -%ddb\n", 6 * (calib->amplitude + 1));
    printf("          duration = %hu seconds\n", calib->duration);
    printf("     settling time = %hu seconds\n", calib->settle);
    printf("      trailer time = %hu seconds\n", calib->trailer);
    printf("   calibrate chans = %s\n", ChannelString(calib->chans));
    printf("     monitor chans = %s\n", ChannelString(calib->monitor));
    printf("         frequency = %.4f Hz (%.4f sec period)\n", freq, 1.0 / freq);
    printf("    control bitmap = "); utilPrintBinUINT16(stdout, calib->sensor); printf("\n");
}

static void SetSensorControlBitmap(QDP_TYPE_C1_QCAL *calib)
{
    calib->sensor = 0;
    if (calib->chans & mask.a) calib->sensor |= QDP_SC_SENSOR_A_CALIB;
    if (calib->chans & mask.b) calib->sensor |= QDP_SC_SENSOR_B_CALIB;
}

static BOOL StartAtSpecifiedTime(Q330 *q330, char *when)
{
double starttime;

    if (strcasecmp(when, "now") == 0 || strcasecmp(when, "0") == 0) {
        q330->cmd.calib.qcal.starttime = 0;
    } else {
        if ((starttime = utilAttodt(when)) < 0.0) {
            printf("Start time `%s' contains one or more illegal fields.\n", when);
            return FALSE;
        }
        q330->cmd.calib.qcal.starttime = starttime - QDP_EPOCH_TO_1970_EPOCH;
    }

    return TRUE;
}

static BOOL StartCustom(Q330 *q330, char *when)
{
    q330->cmd.calib.qcal.waveform = (UINT16) atoi(q330->cmd.arglst->array[2]);
    q330->cmd.calib.qcal.amplitude = (UINT16) atoi(q330->cmd.arglst->array[3]);
    q330->cmd.calib.qcal.duration = (UINT16) atoi(q330->cmd.arglst->array[4]);
    q330->cmd.calib.qcal.settle = (UINT16) atoi(q330->cmd.arglst->array[5]);
    q330->cmd.calib.qcal.trailer = (UINT16) atoi(q330->cmd.arglst->array[6]);
    q330->cmd.calib.qcal.chans = (UINT16) atoi(q330->cmd.arglst->array[7]);
    q330->cmd.calib.qcal.monitor = (UINT16) atoi(q330->cmd.arglst->array[8]);
    q330->cmd.calib.qcal.divisor = (UINT16) atoi(q330->cmd.arglst->array[9]);

    if (q330->cmd.calib.qcal.chans & mask.a) {
        q330->cmd.calib.input = q330->addr.input.a;
    } else if (q330->cmd.calib.qcal.chans & mask.b) {
        q330->cmd.calib.input = q330->addr.input.b;
    } else {
        q330->cmd.calib.input.e300.present = FALSE;
    }

    return StartAtSpecifiedTime(q330, when);
}

BOOL VerifyCalib(Q330 *q330)
{
char *arg;
char *when, *shortcut;


    if (q330->cmd.arglst->count == 0) {
        CalibHelp(q330->cfg, q330->cmd.name, 0, NULL);
        return FALSE;
    }

    arg = (char *) q330->cmd.arglst->array[0];

    if (strcasecmp(arg, "start") == 0) {
        q330->cmd.code = Q330_CMD_CAL_START;
        switch (q330->cmd.arglst->count) {
          case 3:
            when = (char *) q330->cmd.arglst->array[1];
            shortcut = (char *) q330->cmd.arglst->array[2];
            if (!q330GetCalib(shortcut, &q330->addr, q330->cfg, &q330->cmd.calib)) {
                printf("Can't load calibration shortcut '%s' (sensor %s)\n", shortcut, q330->addr.input.a.name);
                return FALSE;
            }
            return StartAtSpecifiedTime(q330, when);
          case 10:
            when = (char *) q330->cmd.arglst->array[1];
            return StartCustom(q330, when);
          default:
            printf("\n*** bad command line ***\n\n");
            CalibStartHelp(q330->cfg, q330->cmd.name);
            return FALSE;
        }
    } else if (strcasecmp(arg, "stop") == 0 || strcasecmp(arg, "off") == 0 || strcasecmp(arg, "abort") == 0) {
        if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);
        q330->cmd.code = Q330_CMD_CAL_STOP;
    } else if (strcasecmp(arg, "status") == 0) {
        if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);
        q330->cmd.code = Q330_CMD_CAL_STATUS;
    } else {
        return UnexpectedArg(q330, arg);
    }
    return TRUE;
}

void calStart(Q330 *q330)
{
QDP_PKT pkt;
int warning;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_STAT stat;
QDP_TYPE_C1_GLOB glob;
UINT32 min_start_time;

/* Grab global status for calibration state and current time */

    qdpInit_C1_STAT(&stat);

    if (!qdp_C1_RQSTAT(q330->qdp, QDP_SRB_GBL, &stat)) {
        printf("Unable to query Q330 for global status.\n");
        return;
    }
    if ((stat.bitmap & QDP_SRB_GBL) == 0) {
        printf("*** ERROR *** qdp_C1_RQSTAT(QDP_SRB_GBL) yields no QDP_SRB_GBL!\n");
        return;
    }

/* Make sure we aren't already running a calibration */

    if (stat.gbl.cal_stat != 0) {
        printf("*****************************************************\n");
        printf("* Q330 already has an active or pending calibration *\n");
        printf("*****************************************************\n");
        return;
    }

/* Determine start time, taking into account settling time */

    min_start_time = stat.gbl.tstamp + q330->cmd.calib.qcal.settle;
    if (q330->cmd.calib.qcal.starttime == 0) {
        q330->cmd.calib.qcal.starttime = min_start_time;
    } else if (q330->cmd.calib.qcal.starttime < min_start_time) {
        q330->cmd.calib.qcal.starttime = min_start_time;
        printf("*** requested start time has been adjusted to accomodate settling time ***\n");
    }

/* Set sensor control lines */

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) {
        printf("ERROR: unable to retrieve global programming!\n");
        return;
    }
    if (!qdp_C1_RQSC(q330->qdp, sc)) {
        printf("ERROR: unable to retrieve sensor control mapping!\n");
        return;
    }
    q330->cmd.calib.qcal.sensor = glob.sensor_map;

        warning = 0;
        if ((q330->cmd.calib.qcal.chans & mask.a) && !qdpUpdateSensorControlBitmap(&q330->cmd.calib.qcal.sensor, sc,  QDP_SC_SENSOR_A_CALIB)) {
            printf("\n");
            printf("*** sensor A calibration enable line is not defined ***\n");
            ++warning;
        }
        if ((q330->cmd.calib.qcal.chans & mask.b) && !qdpUpdateSensorControlBitmap(&q330->cmd.calib.qcal.sensor, sc,  QDP_SC_SENSOR_B_CALIB)) {
            if (!warning) printf("\n");
            printf("*** sensor B calibration enable line is not defined ***\n");
            ++warning;
        }
        if (warning) {
            printf("\n");
            printf("The above warning is of no concern if the sensor does not require cal enable (eg, STS-1),\n");
            printf("otherwise you may want to verify the sensor control mapping ('sc' command)\n");
            printf("\n");
        }

/* If E300 present, enable external calibration */

    if (q330->cmd.calib.input.e300.present) {
        printf("Configuring E300 for external calibration: "); fflush(stdout);
        if (e300Extcal(q330->cmd.calib.input.e300.name)) {
            printf("OK\n");
        } else {
            printf("FAILED\n");
            return;
        }
    }

/* Build and issue calibration start command */

    qdpEncode_C1_QCAL(&pkt, &q330->cmd.calib.qcal);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("calibration start command failed\n");
    } else {
        printf("calibration start command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
        PrintCalib(&q330->cmd.calib.qcal);
    }
}

void calStop(Q330 *q330)
{
QDP_PKT pkt;

    qdpInitPkt(&pkt, QDP_C1_STOP, 0, 0);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("calibration stop command failed\n");
    } else {
        printf("calibration stop command sent to Q330 %s (S/N %016llX)\n", q330->addr.connect.ident, q330->addr.serialno);
    }
}

void calStatus(Q330 *q330)
{
QDP_TYPE_C1_STAT stat;
QDP_PKT pkt;

    if (!qdp_C1_RQSTAT(q330->qdp, QDP_SRB_GBL, &stat)) {
        printf("Unable to query Q330 for global status.\n");
        return;
    }
    if ((stat.bitmap & QDP_SRB_GBL) == 0) {
        printf("*** ERROR *** qdp_C1_RQSTAT(QDP_SRB_GBL) yields no QDP_SRB_GBL!\n");
        return;
    }

    switch (stat.gbl.cal_stat) {
      case 0: printf("calibration is OFF\n"); break;
      case 1: printf("calibration line(s) enabled, signal is OFF\n"); break;
      case 2: printf("calibration signal is ON\n"); break;
      case 3: printf("calibration line(s) enabled, signal is ON\n"); break;
      default:
        printf("calibration status = 0x%02x\n", stat.gbl.cal_stat);
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
 * $Log: calib.c,v $
 * Revision 1.22  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.21  2013/03/01 18:19:57  dechavez
 * Fixed bug that caused custom calibs to think there was an E300 present
 *
 * Revision 1.20  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.19  2010/12/17 19:45:00  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.18  2010/11/24 22:05:37  dechavez
 * nicer error message when not finding calibration shortcut
 *
 * Revision 1.17  2010/11/24 19:24:47  dechavez
 * Use Q330_SENSOR added to Q330_ADDR and Q330_CALIB
 *
 * Revision 1.16  2010/09/29 21:22:50  dechavez
 * If E300 is specified, suppress any sensor control map warnings and automatically enable external calibration
 *
 * Revision 1.15  2010/04/12 21:15:51  dechavez
 * added off and abort aliases for calib stop
 *
 * Revision 1.14  2010/03/31 20:45:56  dechavez
 * call qdpUpdateSensorControlBitmap instead of UpdateBitmap
 *
 * Revision 1.13  2010/03/19 00:36:14  dechavez
 * Check for active calibration before attempting start, added calib status command.
 *
 * Revision 1.12  2009/10/20 22:11:56  dechavez
 * use QDP_SRB_x macros instead of deprecated QDP_STATUS_y
 *
 * Revision 1.11  2009/09/28 22:15:56  dechavez
 * removed tabs
 *
 * Revision 1.10  2009/07/28 18:16:08  dechavez
 * calibration shortcut support
 *
 * Revision 1.9  2009/07/25 17:36:52  dechavez
 * added support for updated Q330_CALIB that allows for multiple calibration
 * sets, however so far only allow use of the default set
 *
 * Revision 1.8  2009/07/23 21:38:37  dechavez
 * fixed calib help
 *
 * Revision 1.7  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 * Revision 1.6  2009/07/10 20:53:22  dechavez
 * fixed bug in setting sensor control line bitmap
 *
 * Revision 1.5  2009/07/09 18:44:36  dechavez
 * Fixed calibration command to set calibrate enable lines and to
 * set start time such that settling time is accomodated
 *
 * Revision 1.4  2009/05/14 18:13:44  dechavez
 * Fixed bug in reporting calibration amplitude
 *
 * Revision 1.3  2008/01/04 19:24:21  dechavez
 * added missing return value in FrequencyValue()
 * (this worked without return under Solaris... scary!!!)
 *
 * Revision 1.2  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
