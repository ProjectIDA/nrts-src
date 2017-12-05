#pragma ident "$Id"
/*======================================================================
 *
 *  various strings
 *
 *====================================================================*/
#include "q330.h"

static char *WaveformString(UINT16 value)
{
int i;
static struct {
    char *text;
    char *description;
    UINT16 code;
} WaveformMap[] = {
    {"sine",  "sine wave",     QDP_QCAL_SINE},
    {"red",   "red noise",     QDP_QCAL_RED},
    {"white", "white noise",   QDP_QCAL_WHITE},
    {"step",  "step",          QDP_QCAL_STEP},
    {"rb",    "random binary", QDP_QCAL_RB},
    {NULL, NULL, -1}
};
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

char *q330CalibParamString(QDP_TYPE_C1_QCAL *calib, char *buf)
{
float freq;
static char mt_unsafe[1024];

    if (buf == NULL) buf = mt_unsafe;

    freq = FrequencyValue(calib);

    buf[0] = 0;

    sprintf(buf+strlen(buf), "     start time = %s\n", StartTimeString(calib->starttime));
    sprintf(buf+strlen(buf), "       waveform = %s\n", WaveformString(calib->waveform));
    sprintf(buf+strlen(buf), "      amplitude = -%ddb\n", 6 * (calib->amplitude + 1));
    sprintf(buf+strlen(buf), "       duration = %hu seconds\n", calib->duration);
    sprintf(buf+strlen(buf), "  settling time = %hu seconds\n", calib->settle);
    sprintf(buf+strlen(buf), "   trailer time = %hu seconds\n", calib->trailer);
    sprintf(buf+strlen(buf), "calibrate chans = %s\n", ChannelString(calib->chans));
    sprintf(buf+strlen(buf), "  monitor chans = %s\n", ChannelString(calib->monitor));
    sprintf(buf+strlen(buf), "      frequency = %.4f Hz (%.4f sec period)\n", freq, 1.0 / freq);

    return buf;
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
 * $Log: string.c,v $
 * Revision 1.1  2010/12/13 22:56:09  dechavez
 * created
 *
 */
