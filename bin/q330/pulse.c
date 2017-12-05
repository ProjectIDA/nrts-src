#pragma ident "$Id: pulse.c,v 1.8 2014/08/11 19:16:16 dechavez Exp $"
/*======================================================================
 *
 *  Pulse sensor control line(s)
 *
 *====================================================================*/
#include "q330cc.h"

#define DEFAULT_DURATION 100

void PulseHelp(char *name, int argc, char **argv)
{
    printf("usage: %s line [dur=duration]\n", name);
    printf("line should be one of: a=calib a=mp a=lock a=unlock b=calib b=mp b=lock b=unlock\n");
    printf("default duration is %.2f seconds\n", (float) DEFAULT_DURATION / 100.0);
}

static char *LineString(int line)
{
int i;
static struct {int line; char *string;} map[] = {
    {QDP_SC_SENSOR_A_CALIB,  "sensor A calibration enable"},
    {QDP_SC_SENSOR_A_CENTER, "sensor A mass recenter"},
    {QDP_SC_SENSOR_A_LOCK,   "sensor A lock"},
    {QDP_SC_SENSOR_A_UNLOCK, "sensor A unlock"},
    {QDP_SC_SENSOR_A_CALIB,  "sensor B calibration enable"},
    {QDP_SC_SENSOR_A_CENTER, "sensor B mass recenter"},
    {QDP_SC_SENSOR_A_LOCK,   "sensor B lock"},
    {QDP_SC_SENSOR_A_UNLOCK, "sensor B unlock"},
    {-1, NULL}
};
static char *undefined = "<LOGIC ERROR IN LineString!!!!>";

    for (i = 0; map[i].string != NULL; i++) if (map[i].line == line) return map[i].string;

    return undefined;
}

BOOL VerifyPulse(Q330 *q330)
{
int i;
char *arg;

    q330->cmd.code = Q330_CMD_PULSE;

    q330->cmd.pulse.duration = DEFAULT_DURATION;
    for (q330->cmd.pulse.line = -1, i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];
        if (strcasecmp(arg, "help") == 0) {
            PulseHelp(q330->cmd.name, 0, NULL);
            return FALSE;

        } else if (strcasecmp(arg, "a=calib") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_CALIB;
        } else if (strcasecmp(arg, "a=mp") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_CENTER;
        } else if (strcasecmp(arg, "a=lock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_LOCK;
        } else if (strcasecmp(arg, "a=unlock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_UNLOCK;

        } else if (strcasecmp(arg, "b=calib") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_CALIB;
        } else if (strcasecmp(arg, "b=mp") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_CENTER;
        } else if (strcasecmp(arg, "b=lock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_LOCK;
        } else if (strcasecmp(arg, "b=unlock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_UNLOCK;

        } else if (strncasecmp(arg, "dur=", strlen("dur=")) == 0) {
            q330->cmd.pulse.duration = (UINT16) (atof(arg+strlen("dur=")) * 100);
        } else {
            printf("ERROR: unrecognized %s argument '%s'\n",  q330-> cmd.name, arg);
            PulseHelp(q330->cmd.name, 0, NULL);
            return FALSE;
        }
    }

    if (q330->cmd.pulse.line < 0) {
        PulseHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }
    if (q330->cmd.pulse.duration < 1) {
        printf("ERROR: illegal duration\n");
        PulseHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    return TRUE;
}

void pulse(Q330 *q330)
{
QDP_PKT pkt;
UINT16 bitmap;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_GLOB glob;

    if (!qdp_C1_RQSC(q330->qdp, sc)) {
        printf("ERROR: unable to retrieve sensor control mapping!\n");
        return;
    }
    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) {
        printf("ERROR: unable to retrieve global programming!\n");
        return;
    }
    bitmap = glob.sensor_map;

    if (!qdpUpdateSensorControlBitmap(&bitmap, sc, q330->cmd.pulse.line)) {
        printf("ERROR: requested %s line is not defined.\n", LineString(q330->cmd.pulse.line));
        printf("Verify the sensor control map ('sc' command).\n");
        return;
    }

    qdpEncode_C1_PULSE(&pkt, bitmap, q330->cmd.pulse.duration);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("pulse sensor control lines command failed\n");
    } else {
        printf("Pulsing %s line ", LineString(q330->cmd.pulse.line));
        printf("for %.2f seconds\n", (float) q330->cmd.pulse.duration / 100.0);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * Revision 1.8  2014/08/11 19:16:16  dechavez
 * removed unused argument to the "illegal duration" printf statement
 *
 * Revision 1.7  2010/03/31 20:45:16  dechavez
 * Moved UpdateBitmap to qdp library as qdpUpdateSensorControlBitmap()
 *
 * Revision 1.6  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.5  2009/07/10 20:52:15  dechavez
 * made UpdateBitmap() a public function (so calib can use it)
 *
 * Revision 1.4  2009/02/04 17:51:04  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.3  2009/01/26 22:24:29  dechavez
 * Fixed bug setting pulse duration, changed default pulse width to one
 * second, fixed bug specifying sensor b recentering, changed flag for
 * recenter line from mass to mp.
 *
 * Revision 1.2  2009/01/26 21:21:52  dechavez
 * fixed bug in setting pulse duration
 *
 * Revision 1.1  2009/01/24 00:13:10  dechavez
 * initial release
 *
 */
