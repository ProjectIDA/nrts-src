#pragma ident "$Id: wcfg.c,v 1.21 2016/01/04 21:32:05 dechavez Exp $"
/*======================================================================
 * 
 * Q330 cfg file support
 *
 *====================================================================*/
#include "q330.h"

void q330PrintAddr(FILE *fp, Q330_ADDR *addr)
{

    if (fp == NULL || addr == NULL) return;

    fprintf(fp, " %6s",   addr->connect.ident);
    if (addr->connect.type == QIO_UDP) {
        fprintf(fp, "     %6d",   addr->connect.tag);
    } else {
        fprintf(fp, " %10s",   addr->connect.iostr);
    }
    fprintf(fp, "  %-12s", addr->input.a.name);
    fprintf(fp, " %-12s", addr->input.b.name);
    fprintf(fp, "\n");
}

void q330PrintAuth(FILE *fp, Q330_AUTH *auth)
{
    if (fp == NULL || auth == NULL) return;

    fprintf(fp, "%7u", auth->tag);
    fprintf(fp, " %016llX", auth->serialno);
    fprintf(fp, " %5d", auth->code[QDP_CFG_PORT]);
    fprintf(fp, " %5d", auth->code[QDP_SFN_PORT]);
    fprintf(fp, " %5d", auth->code[QDP_LOGICAL_PORT_1]);
    fprintf(fp, " %5d", auth->code[QDP_LOGICAL_PORT_2]);
    fprintf(fp, " %5d", auth->code[QDP_LOGICAL_PORT_3]);
    fprintf(fp, " %5d", auth->code[QDP_LOGICAL_PORT_4]);
    fprintf(fp, "\n");
}

void q330WriteAddr(FILE *fp, Q330_CFG *cfg)
{
int i;
LNKLST_NODE *crnt;
Q330_ADDR *addr;
static char *hdr =
"# The q330.cfg file describes system and sensor assignments and consists\n"
"# of 4 arguments:\n"
"#\n"
"#   1 - The /etc/hosts name of the digitizer.\n"
"#   2 - The digitizer property tag number (if network connection)\n"
"#       else /etc/hosts name of Digi or serial device path\n"
"#   3 - Complex name describing sensor A input\n"
"#   4 - Complex name describing sensor B input\n"
"#\n"
"# The complex input names must exactly match the complex input names\n"
"# in the 'calib' configuration file.  These names consist of up to\n"
"# three ':' delimited strings in the form sensor[:e300[:x]] where only\n"
"# 'sensor' is required and must exactly match a name from the 'sensor'\n"
"# config file.\n"
"#\n"
"# For most sensors, the complex name will normally be just the name\n"
"# from the sensor file.  For the STS-1, if an E300 is present then the\n"
"# /etc/hosts name of the Digi which is connected to the E300 console\n"
"# should follow (eg, 'sts1:e300').  An optional third parameter can\n"
"# be specified for all sensors, if there is a need to differentiate\n"
"# between instruments with similiar control lines but different amplitude\n"
"# response.  No other significance is given to this argument... it is\n"
"# used solely for constructing unique complex names for the 'calib' table.\n"
"#\n"
"# For instance, an STS-1 with a SIB that has resistors on the calib\n"
"# input should be represented with the tag 'r' in the final field.  A\n"
"# high-gain STS-2 should be represented with the tag 'hi'.  Other tags\n"
"# can be assigned as desired.  Some examples:\n"
"#\n"
"# sts1:e300:r - STS-1 with an E330 and resistors in the SIB\n"
"# sts1::r     - STS-1 with old feedback boxes and resistors in the SIB\n"
"# sts2::hi    - high gain STS-2\n"
"#\n"
"#  Name      TagNo  Sensor A     Sensor B\n";

    fprintf(fp, "%s", hdr);
    crnt = listFirstNode(cfg->addr);
    crnt = listNextNode(crnt); /* skip over built-in console entry */
    while (crnt != NULL) {
        addr = (Q330_ADDR *) crnt->payload;
        q330PrintAddr(fp, addr);
        crnt = listNextNode(crnt);
    }
}

void q330WriteAuth(FILE *fp, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_AUTH *auth;
static char *hdr =
"#\n"
"# The auth file gives the serial numbers and authorization\n"
"# codes for each known unit.\n"
"#\n"
"# It consists of 8 arguments:\n"
"#\n"
"#  1 - KMI property tag\n"
"#  2 - internal serial number\n"
"#  3 - authorization code for configuration port\n"
"#  4 - authorization code for special functions port\n"
"#  5 - authorization code for data port 1\n"
"#  6 - authorization code for data port 2\n"
"#  7 - authorization code for data port 3\n"
"#  8 - authorization code for data port 4\n"
"#\n"
"# TagNo  Serial Number     cfg   sfn   dp1   dp2   dp3   dp4\n";
     
    fprintf(fp, "%s", hdr);
    crnt = listFirstNode(cfg->auth);
    while (crnt != NULL) {
        auth = (Q330_AUTH *) crnt->payload;
        q330PrintAuth(fp, auth);
        crnt = listNextNode(crnt);
    }
}

void q330WriteSensor(FILE *fp, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_SENSOR *sensor;
static char *hdr =
"#\n"
"# The sensor file defines the control lines, and consists of 5 arguments\n"
"#\n"
"#  1 - Unique name for the instrument, up to 10 characters long.\n"
"#  2 - The active state.  Use 'hi' for active high, 'lo' for active low, and 'na' for other.\n"
"#  3 - Sensor A control lines (contents described below)\n"
"#  4 - Sensor B control lines\n"
"#  5 - Quote delimited description of the sensor\n"
"#\n"
"#  The control lines are described by a set of up to 8 comma delimited strings,\n"
"#  each of the form line:ctl where line is the physical control line number\n"
"#  (1 thru 8) and ctl is a 3 character code describing the operation thus:\n"
"#\n"
"#      ctl  Operation\n"
"#      ---  ---------\n"
"#      idl  line is idle\n"
"#      cen  mass recenter\n"
"#      cal  calibration enable\n"
"#      cap  capacitive coupling\n"
"#      lck  lock\n"
"#      unl  unlock\n"
"#      dpr  deploy/remove\n"
"#      ax1  auxiliary 1\n"
"#      ax2  auxiliary 2\n"
"#\n"
"# Name       hi/lo      Sensor A Lines           Sensor B Lines       Description\n"
"# --------   ----   -----------------------  -----------------------  -----------\n";


    fprintf(fp, "%s", hdr);
    crnt = listFirstNode(cfg->sensor);
    while (crnt != NULL) {
        sensor = (Q330_SENSOR *) crnt->payload;
        fprintf(fp, "  %-10s", sensor->name);
        fprintf(fp, " %3s", q330SensorHiLoString(sensor));
        fprintf(fp, "   ");
        fprintf(fp, " %23s", q330SensorCtrlLineString(sensor->scA, NULL));
        fprintf(fp, " ");
        fprintf(fp, " %23s", q330SensorCtrlLineString(sensor->scB, NULL));
        fprintf(fp, " \"%s\"", sensor->desc);
        fprintf(fp, "\n");
        crnt = listNextNode(crnt);
    }
}

void q330WriteDetector(FILE *fp, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_DETECTOR *detector;
static char *hdr =
"#\n"
"# The detector file is used to define the qhlp event detector and consists\n"
"# of a variable number of items.  The first 6 are always the same:\n"
"#\n"
"#   1 - The site name for which this is to be applied, or 'default'\n"
"#   2 - A comma delimited string listing the names of the triggered channels\n"
"#   3 - mininum number of 'on' channels required to declare an event\n"
"#   4 - Pre-event memory, in packets\n"
"#   5 - Post-event memory, in packets\n"
"#   6 - Type of detector.  Currently only 'stalta' is supported.\n"
"#\n"
"# The remaining parameters are specific to the type of detector.  For stalta\n"
"#\n"
"#   7 - STA window length, in samples\n"
"#   8 - LTA window length, in samples\n"
"#   9 - STA/LTA ratio needed to transition from off to on\n"
"#  10 - STA/LTA ratio needed to transition from on to off\n"
"#\n"
"# Site       Channels     Votes Pre Post   Type  STA  LTA   On  Off\n";

    fprintf(fp, "%s", hdr);
    crnt = listFirstNode(cfg->detector);
    while (crnt != NULL) {
        detector = (Q330_DETECTOR *) crnt->payload;
        fprintf(fp, " %7s", detector->name);
        fprintf(fp, " %17s", detector->channels);
        fprintf(fp, " %4d", detector->votes);
        fprintf(fp, " %3d", detector->pre);
        fprintf(fp, " %4d", detector->pst);
        switch (detector->engine.type) {
          case DETECTOR_TYPE_STALTA:
            fprintf(fp, "  stalta");
            fprintf(fp, " %3d", detector->engine.stalta.config.len.sta);
            fprintf(fp, " %4d", detector->engine.stalta.config.len.lta);
            fprintf(fp, " %4.1f", detector->engine.stalta.config.ratio.on);
            fprintf(fp, " %4.1f", detector->engine.stalta.config.ratio.off);
            break;
          default:
            fprintf(fp, "*** ERROR *** detector type '%d' is unrecognized!\n", detector->engine.type);
            break;
        }
        fprintf(fp, "\n");
        crnt = listNextNode(crnt);
    }
}

void q330WriteCalib(FILE *fp, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_CALIB *calib;
char *prev = NULL;
static char *hdr =
"#\n"
"# The calib file specifies calibration shortcuts and consists of 8 parameters\n"
"#\n"
"#   1 - Name of this shortcut\n"
"#   2 - Complex sensor name for which these parameters apply\n"
"#   3 - Waveform code (0=sine 1=red noise 2=white noise 3=step 4=random binary\n"
"#   4 - Amplitude bits shifted (0=-6db 1=-12db, etc)\n"
"#   5 - duration in seconds\n"
"#   6 - settling time in seconds\n"
"#   7 - trailer time in seconds\n"
"#   8 - frequency divider (1 to 255)\n"
"#       1=1Hz sine or 125Hz update freq, 20=0.05Hz sine or 6.26Hz update\n"
"#\n"
"# The shortcut name (1st parameter) should be reused as needed for different\n"
"# sensors. The complex sensor name below must match exactly with the names\n"
"# given for q330.cfg table.\n"
"#\n"
"# name    sensor         wf  amp    dur set trl div\n";

    fprintf(fp, "%s", hdr);

    crnt = listFirstNode(cfg->calib);
    while (crnt != NULL) {
        calib = (Q330_CALIB *) crnt->payload;
        if (prev != NULL && strcmp(prev, calib->input.name) != 0) fprintf(fp, "\n");
        fprintf(fp, "  %-7s",  calib->name);
        fprintf(fp, " %-12s", calib->input.name);
        fprintf(fp, " %4d",   calib->qcal.waveform);
        fprintf(fp, " %4d",   calib->qcal.amplitude);
        fprintf(fp, " %6d",   calib->qcal.duration);
        fprintf(fp, " %3d",   calib->qcal.settle);
        fprintf(fp, " %3d",   calib->qcal.trailer);
        fprintf(fp, " %3d",   calib->qcal.divisor);
        fprintf(fp, "\n");
        prev = calib->input.name;
        crnt = listNextNode(crnt);
    }
}

static void PrintLcqList(FILE *fp, LNKLST *list)
{
LNKLST_NODE *crnt;
Q330_LCQ_ENTRY *entry;
static char *hdr =
"# Chn Lc Src 1  2  Sint\n"
"# --- -- ----- -- ------\n";

    fprintf(fp, "%s", hdr);

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        entry = (Q330_LCQ_ENTRY *) crnt->payload;
        fprintf(fp, " ");
        fprintf(fp, " %3s", entry->chn);
        fprintf(fp, " %2s", entry->loc);
        fprintf(fp, "  0x%02x", entry->src[0]);
        fprintf(fp, " %2d", entry->src[1]);
        fprintf(fp, " %6.3lf", entry->sint);
        fprintf(fp, "\n");
        crnt = listNextNode(crnt);
    }
}

void q330WriteLcq(FILE *fp, Q330_CFG *cfg)
{
LNKLST_NODE *crnt;
Q330_LCQ *lcq;
static char *hdr =
"# The lcq (logical channel queue) file defines token sets configuring\n"
"# data port output streams.  It consists of blocks of 5 parameters, \n"
"# prefixed with a single parameter line that is the name of the token set.\n"
"# The 5 parameters in each block are\n"
"#\n"
"#   1 - channel code\n"
"#   2 - location code\n"
"#   3 - LCQ src byte 1, hexadecimal\n"
"#   4 - LCQ src byte 2, decimal\n"
"#   5 - sample interval seconds\n"
"#\n";

    fprintf(fp, "%s", hdr);

    crnt = listFirstNode(cfg->lcq);
    while (crnt != NULL) {
        lcq = (Q330_LCQ *) crnt->payload;
        fprintf(fp, "\n");
        fprintf(fp, "%s\n", lcq->name);
        PrintLcqList(fp, lcq->list);
        crnt = listNextNode(crnt);
    }
}

BOOL q330SaveAddr(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_ADDR);
        path = new;
    } else {
        path = cfg->path.addr;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteAddr(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveAuth(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_AUTH);
        path = new;
    } else {
        path = cfg->path.auth;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteAuth(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveSensor(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_SENSOR);
        path = new;
    } else {
        path = cfg->path.addr;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteSensor(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveDetector(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_DETECTOR);
        path = new;
    } else {
        path = cfg->path.addr;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteDetector(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveCalib(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_CALIB);
        path = new;
    } else {
        path = cfg->path.addr;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteCalib(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveLcq(char *root, Q330_CFG *cfg)
{
FILE *fp;
char *path, new[MAXPATHLEN+1];

    if (root != NULL) {
        sprintf(new, "%s/%s", root, Q330_NAME_SUBDIR);
        if (util_mkpath(new, 0755) != 0) return FALSE;
        sprintf(new, "%s/%s", root, Q330_NAME_LCQ);
        path = new;
    } else {
        path = cfg->path.addr;
    }

    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    q330WriteLcq(fp, cfg);
    fclose(fp);

    return TRUE;

}

BOOL q330SaveCfg(char *root, Q330_CFG *cfg)
{

    if (!q330SaveAddr(root, cfg)) return FALSE;
    if (!q330SaveAuth(root, cfg)) return FALSE;
    if (!q330SaveSensor(root, cfg)) return FALSE;
    if (!q330SaveDetector(root, cfg)) return FALSE;
    if (!q330SaveCalib(root, cfg)) return FALSE;
    if (!q330SaveLcq(root, cfg)) return FALSE;

    return TRUE;
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
 * $Log: wcfg.c,v $
 * Revision 1.21  2016/01/04 21:32:05  dechavez
 * make src2 string two characters long (EP sources, for example)
 *
 * Revision 1.20  2015/12/04 23:20:35  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.19  2014/01/30 22:36:33  dechavez
 * moved dpr line to below unl
 *
 * Revision 1.18  2014/01/30 19:59:16  dechavez
 * added dpr (deploy/remove) to hdr (comments)
 *
 * Revision 1.17  2011/04/14 21:00:34  dechavez
 * changed lcq output header
 *
 * Revision 1.16  2011/04/12 20:51:07  dechavez
 * added q330SaveLcq(), q330WriteLcq(), PrintLcqList()
 *
 * Revision 1.15  2011/02/01 19:43:58  dechavez
 * don't include built-in console entry in q330WriteAddr()
 *
 * Revision 1.14  2011/01/25 18:06:40  dechavez
 * use ident and tag/iostr from connect field in q330PrintAddr()
 *
 * Revision 1.13  2010/12/21 21:33:08  dechavez
 * changed q330.cfg header text to improve(?) description of complex sensor names
 *
 * Revision 1.12  2010/12/21 19:55:05  dechavez
 * added q330PrintAuth(), cleaned up the various q330WriteX routines
 *
 * Revision 1.11  2010/12/17 19:33:00  dechavez
 * libq330 2.0.0
 *
 * Revision 1.10  2010/12/13 22:55:02  dechavez
 * added q330SaveCfg()
 *
 * Revision 1.9  2010/11/30 20:19:34  dechavez
 * cleaned up header text
 *
 * Revision 1.8  2010/11/24 19:17:48  dechavez
 * print sensor list to fp instead of stdout
 *
 * Revision 1.7  2010/11/24 18:34:53  dechavez
 * added sensors to addr and calib
 *
 * Revision 1.6  2010/09/29 21:26:18  dechavez
 * support for e300 calibration parameter
 *
 * Revision 1.5  2010/03/31 20:24:08  dechavez
 * removed calibration port
 *
 * Revision 1.4  2010/03/22 21:44:00  dechavez
 * added support for printing non-default calibration ports
 *
 * Revision 1.3  2010/03/12 00:30:54  dechavez
 * added support for calibration stuff in Q330_ADDR
 *
 * Revision 1.2  2009/07/28 18:10:53  dechavez
 * added q330PrintCalShortcuts()
 *
 * Revision 1.1  2009/07/25 17:29:24  dechavez
 * created
 *
 */
