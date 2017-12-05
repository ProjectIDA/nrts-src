#pragma ident "$Id: sc.c,v 1.7 2010/12/28 23:34:00 dechavez Exp $"
/*======================================================================
 *
 *  Manipulate sensor control map
 *
 *====================================================================*/
#include "qmon.h"

static BOOL AddIfUnique(LNKLST *list, Q330_INPUT *new)
{
LNKLST_NODE *crnt;
Q330_INPUT *input;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        input = (Q330_INPUT *) crnt->payload;
        if (strcmp(input->name, new->name) == 0) return TRUE;
        crnt = listNextNode(crnt);
    }

    return listAppend(list, new, sizeof(Q330_INPUT));
}

static LNKLST *CreateSensorList(QMON *qmon)
{
LNKLST *list;
LNKLST_NODE *crnt;
Q330_CALIB *calib;

    if ((list = listCreate()) == NULL) return NULL;
    crnt = listFirstNode(qmon->cfg->calib);
    while (crnt != NULL) {
        calib = (Q330_CALIB *) crnt->payload;
        if (!AddIfUnique(list, &calib->input)) {
            listDestroy(list);
            return NULL;
        }
        crnt = listNextNode(crnt);
    }

    return list;
}

static void HelpMesg(QMON *qmon, int argc, char **argv)
{
int i;
LNKLST *list;
LNKLST_NODE *crnt;
Q330_INPUT *input;

    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "usage: sc auto | sensorA sensorB\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "Recognized sensor names:\n");
    if ((list = CreateSensorList(qmon)) != NULL) {
        crnt = listFirstNode(list);
        while (crnt != NULL) {
            input = (Q330_INPUT *) crnt->payload;
            sprintf(qmon->poptxt+strlen(qmon->poptxt), "%11s -", input->name);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), " %s", input->sensor.desc);
            if (input->e300.present) sprintf(qmon->poptxt+strlen(qmon->poptxt), " + E300");
            if (input->extra.present) sprintf(qmon->poptxt+strlen(qmon->poptxt), " + %s", input->extra.name);
            sprintf(qmon->poptxt+strlen(qmon->poptxt), "\n");
            crnt = listNextNode(crnt);
        }
        listDestroy(list);
    }
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "      other - sensor present, no control lines\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "       none - no sensor\n");
    PopupMessage(qmon, qmon->poptxt);
}

BOOL SensorControlMismatch(DIGITIZER *digitizer, UINT32 *actual)
{
UINT32 expected[QDP_NSC];

    q330InitializeSensorControl(expected, &digitizer->addr->input.a.sensor, &digitizer->addr->input.b.sensor);
    return memcmp(expected, actual, sizeof(UINT32) * QDP_NSC) == 0 ? FALSE : TRUE;
}

void ExecuteSc(QMON *qmon, int argc, char **argv)
{
int errcode;
QDP_PKT pkt;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_GLOB glob;
Q330_INPUT tmp;
struct {
    Q330_INPUT a;
    Q330_INPUT b;
} new, crnt;
BOOL mismatch;

    crnt.a = qmon->active->addr->input.a; /* to make code easier to read */
    crnt.b = qmon->active->addr->input.b;

    if (argc == 2) {
        if (strcasecmp(argv[1], "auto") == 0) {
            new.a = crnt.a;
            new.b = crnt.b;
        } else {
            PopupMessage(qmon, "BOTH sensor A and sensor B must be specified (or use 'auto')");
            return;
        }
    } else if (argc == 3) {
        if (!q330ParseComplexInputName(&new.a, argv[1], &errcode)) {
            sprintf(qmon->poptxt, "ERROR: can't parse complex sensor A name '%s'", argv[1]);
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
        if (!q330GetSensor(new.a.sensor.name, qmon->cfg, &new.a.sensor)) {
            sprintf(qmon->poptxt, "ERROR: sensor '%s' is unrecognized", new.a.sensor.name);
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
        if (!q330ParseComplexInputName(&new.b, argv[2], &errcode)) {
            sprintf(qmon->poptxt, "ERROR: can't parse complex sensor B name '%s'", argv[1]);
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
        if (!q330GetSensor(new.b.sensor.name, qmon->cfg, &new.b.sensor)) {
            sprintf(qmon->poptxt, "ERROR: sensor '%s' is unrecognized", new.b.sensor.name);
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
    } else {
        HelpMesg(qmon, argc, argv);
        return;
    }

/* If new inputs don't agree with what is in the config file then we'll need to update it */

#define InputChanged(A, B) (strcmp(A.name, B.name) == 0 ? FALSE : TRUE)

    if (InputChanged(crnt.a, new.a) || InputChanged(crnt.b, new.b)) {
        qmon->active->addr->input.a = new.a;
        qmon->active->addr->input.b = new.b;
        qmon->newcfg = TRUE;
        qmon->active->flags |= UNSAVED_CHANGES;
    } else {
        if (!SensorControlMismatch(qmon->active, qmon->active->mon.sc)) return;
    }

    q330InitializeSensorControl(sc, &new.a.sensor, &new.b.sensor);
    if (memcmp(sc, qmon->active->mon.sc, QDP_NSC * sizeof(UINT32)) == 0) return;

/* Set sensor control lines */

    qdpEncode_C1_SC(&pkt, sc);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "Set Physical Interfaces command failed");
        return;
    }

/* Set sensor control bitmap */

    if (!qdp_C1_RQGLOB(qmon->qdp, &glob)) {
        PopupMessage(qmon, "qdp_C1_RQGLOB failed");
        return;
    }
    glob.sensor_map = q330SensorControlBitmap(sc);
    qdpEncode_C1_GLOB(&pkt, &glob);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "Set Global Programming command failed\n");
        return;
    }

    qmon->active->flags |= UNSAVED_CHANGES;
    qmon->active->flags |= REBOOT_REQUIRED;
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
 * $Log: sc.c,v $
 * Revision 1.7  2010/12/28 23:34:00  dechavez
 * added "other" and "none" to list of recognized sensors
 *
 * Revision 1.6  2010/12/27 22:43:08  dechavez
 * Changed help to print list of recognized complex input names, not just sensors.
 * Fixed bug that prevented recognition of sensor changes that did not involve a
 * corresponding change to the sensor control map.
 *
 * Revision 1.5  2010/12/23 22:27:36  dechavez
 * improved help message
 *
 * Revision 1.4  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.3  2010/12/13 23:58:51  dechavez
 * set save flag when doing sensor configuration
 *
 * Revision 1.2  2010/12/13 23:43:23  dechavez
 * support complex sensor names
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
