#pragma ident "$Id: limit.c,v 1.1 2011/01/07 23:03:31 dechavez Exp $"
/*======================================================================
 *
 *  Set operational limits
 *
 *====================================================================*/
#include "qmon.h"

static void HelpMesg(QMON *qmon, int argc, char **argv)
{
    qmon->poptxt[0] = 0;
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "usage: %s { ItemName ItemValue } | default\n", argv[0]);
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "Where ItemName is:\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "  MaxMainCur - maximum main current (ma)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "  MinOffTime - minimum off time (sec)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "MinPsVoltage - minimum power supply voltage (V)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "   MaxAntCur - maximum antenna current (ma)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "     MinTemp - minimum temperature (deg C)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "     MaxTemp - maximum temperature (deg C)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "    TempHyst - temperature hysteresis (deg C)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "    VoltHyst - voltage hysteresis (V)\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "         VCO - default VCO value\n");
    PopupMessage(qmon, qmon->poptxt);
}

#define DEFAULT_MAX_MAIN_CUR 350
void ExecuteLimit(QMON *qmon, int argc, char **argv)
{
QDP_PKT pkt;
BOOL changed;
QDP_TYPE_C1_SPP new, *crnt, DefaultSpp = QDP_DEFAULT_C1_SPP;

    crnt = &qmon->active->mon.spp;
    new = *crnt;

    if (argc == 2) {
        if (strcasecmp(argv[1], "default") == 0) {
            new = DefaultSpp;
        } else {
            HelpMesg(qmon, argc, argv);
            return;
        }
    } else if (argc == 3) {
        if (strcasecmp(argv[1], "MaxMainCur") == 0) {
            new.max_main_current = (UINT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "MinOffTime") == 0) {
            new.min_off_time = (UINT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "MinPsVoltage") == 0) {
            new.min_ps_voltage = (UINT16) (atof(argv[2]) / 0.15);
        } else if (strcasecmp(argv[1], "MaxAntCur") == 0) {
            new.max_antenna_current = (UINT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "MinTemp") == 0) {
            new.min_temp = (INT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "MaxTemp") == 0) {
            new.max_temp = (INT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "TempHyst") == 0) {
            new.temp_hysteresis = (UINT16) atoi(argv[2]);
        } else if (strcasecmp(argv[1], "VoltHyst") == 0) {
            new.volt_hysteresis = (UINT16) (atof(argv[2]) / 0.15);
        } else if (strcasecmp(argv[1], "VCO") == 0) {
            new.default_vco = (UINT16) atoi(argv[2]);
        } else {
            HelpMesg(qmon, argc, argv);
            return;
        }
    } else {
        HelpMesg(qmon, argc, argv);
        return;
    }

    if (new.max_main_current != crnt->max_main_current) {
        changed = TRUE;
    } else if (new.min_off_time != crnt->min_off_time) {
        changed = TRUE;
    } else if (new.min_ps_voltage != crnt->min_ps_voltage) {
        changed = TRUE;
    } else if (new.max_antenna_current != crnt->max_antenna_current) {
        changed = TRUE;
    } else if (new.min_temp != crnt->min_temp) {
        changed = TRUE;
    } else if (new.max_temp != crnt->max_temp) {
        changed = TRUE;
    } else if (new.temp_hysteresis != crnt->temp_hysteresis) {
        changed = TRUE;
    } else if (new.volt_hysteresis != crnt->volt_hysteresis) {
        changed = TRUE;
    } else if (new.default_vco != crnt->default_vco) {
        changed = TRUE;
    } else {
        changed = FALSE;
    }

    if (!changed) {
        PopupMessage(qmon, "operational limits unchanged");
        return;
    }

    qdpEncode_C1_SPP(&pkt, &new);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "Set Slave Processor Parameters command failed\n");
        return;
    }

    qmon->active->flags |= UNSAVED_CHANGES;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: limit.c,v $
 * Revision 1.1  2011/01/07 23:03:31  dechavez
 * created
 *
 */
