#pragma ident "$Id: sc.c,v 1.13 2013/02/26 21:34:27 dechavez Exp $"
/*======================================================================
 *
 *  Sensor Control Mapping
 *
 *====================================================================*/
#include "q330cc.h"

void SCHelp(char *name, int argc, char **argv)
{
int i;
LNKLST_NODE *cnrt;

    printf("usage: %s auto | sensorA sensorB\n", name);
}

static void CompareCurrentAndExpected(UINT32 *current, UINT32 *expected)
{
    if (memcmp(current, expected, sizeof(UINT32) * QDP_NSC) != 0) {
        printf("\n");
        printf("************************ !! WARNING !! ************************\n");
        printf("*  Sensor control map is inconsistent with assigned sensors!  *\n");
        printf("*    Use \"sc auto\" to correct the Q330 sensor control map.    *\n");
        printf("***************************************************************\n");
    }
}

static BOOL SetInst(Q330 *q330, char *a, char *b)
{
char *want;

    if ((q330->cmd.sc.sensor.a = q330LookupSensor(a, q330->cfg)) == NULL) {
        printf("sensor A type '%s' unrecognized\n", a);
        SCHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    if ((q330->cmd.sc.sensor.b = q330LookupSensor(b, q330->cfg)) == NULL) {
        printf("sensor B type '%s' unrecognized\n", b);
        SCHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    q330->cmd.update = TRUE;
    return TRUE;
}

BOOL VerifySC(Q330 *q330)
{
char *arg;

    q330->cmd.update = FALSE;
    q330->cmd.code = Q330_CMD_SC;

    if (q330->cmd.arglst->count == 0) return TRUE;

    q330->cmd.update = TRUE;
    switch (q330->cmd.arglst->count) {
      case 1:
        arg = (char *) q330->cmd.arglst->array[0];
        if (strcasecmp(arg, "auto") == 0) {
            q330->cmd.sc.sensor.a = &q330->addr.input.a.sensor;
            q330->cmd.sc.sensor.b = &q330->addr.input.b.sensor;
            return TRUE;
        }
        break;

      case 2:
        return SetInst(q330, (char *) q330->cmd.arglst->array[0], (char *) q330->cmd.arglst->array[1]);
        break;
    }

    SCHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

BOOL ConfigSensorControl(Q330 *q330, Q330_SENSOR *a, Q330_SENSOR *b)
{
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;
UINT32 new[QDP_NSC];

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) {
        printf("qdp_C1_RQGLOB failed\n");
        return FALSE;
    }

    q330InitializeSensorControl(new, a, b);
    glob.sensor_map = q330SensorControlBitmap(new);

    qdpEncode_C1_SC(&pkt, new);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Physical Interfaces command failed\n");
        return FALSE;
    }

    qdpEncode_C1_GLOB(&pkt, &glob);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Global Programming command failed\n");
        return FALSE;
    }

    return TRUE;
}


void sc(Q330 *q330)
{
int i, status;
QDP_TYPE_C1_GLOB glob;
UINT32 current[QDP_NSC], expected[QDP_NSC], new[QDP_NSC];

    q330InitializeSensorControl(expected, &q330->addr.input.a.sensor, &q330->addr.input.b.sensor);

/* Get current mapping */

    if (!qdp_C1_RQSC(q330->qdp, current)) return;

/* Print current values */

    if (q330->cmd.update) printf("Current ");
    qdpPrint_C1_SC(stdout, current);

/* All done if we aren't changing anything */

    if (!q330->cmd.update) {
        CompareCurrentAndExpected(current, expected);
        return;
    }

/* Define and install new sensor control line definitions and bitmap */

    if (!ConfigSensorControl(q330, q330->cmd.sc.sensor.a, q330->cmd.sc.sensor.b)) return;

/* Retrieve the freshly installed values, and print them out */

    if (!qdp_C1_RQSC(q330->qdp, new)) return;
    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

    printf("\nUpdated ");
    qdpPrint_C1_SC(stdout, new);
    printf("C1_GLOB SC bitmap = "); utilPrintBinUINT16(stdout, glob.sensor_map); printf("\n");
    printf("Changes will not take effect until configuration is saved and the Q330 is rebooted.\n");
    CompareCurrentAndExpected(new, expected);
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
 * $Log: sc.c,v $
 * Revision 1.13  2013/02/26 21:34:27  dechavez
 * Fixed bug that caused core dump with "q330 help sc"
 *
 * Revision 1.12  2010/12/17 19:45:00  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.11  2010/12/13 23:02:12  dechavez
 * use q330InitializeSensorControl() (which uses the code formerly here)
 *
 * Revision 1.10  2010/11/24 19:22:11  dechavez
 * Exploiting addition of sensor data to Q330_ADDR to perform sensor control map
 * consistency checks and implement "sc auto" command.  Moved  a bunch of sensor
 * control setup code over to libq330.
 *
 * Revision 1.9  2010/06/14 19:26:39  dechavez
 * removed unused variables
 *
 * Revision 1.8  2010/06/08 23:55:16  dechavez
 * corrected lock/unlock line definition error for GURLO and GURHI
 *
 * Revision 1.7  2009/10/21 17:34:04  dechavez
 * removed k1 support, prematurely added
 *
 * Revision 1.6  2009/10/20 23:09:44  dechavez
 * added trillium and k1 sensor control lines
 *
 * Revision 1.5  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.4  2009/07/10 20:51:25  dechavez
 * added support for KS54000
 *
 * Revision 1.3  2009/02/23 22:13:06  dechavez
 * permit setting individual or default values
 *
 * Revision 1.2  2009/02/04 17:47:20  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.1  2009/01/24 00:13:09  dechavez
 * initial release
 *
 */
