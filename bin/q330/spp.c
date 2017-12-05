#pragma ident "$Id: spp.c,v 1.4 2010/12/13 23:03:22 dechavez Exp $"
/*======================================================================
 *
 *  Request Slave Processor Parameters
 *
 *====================================================================*/
#include "q330cc.h"

void SppHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ default | { MaxMainCur MinOffT MinPSV MaxAntCur MinTemp MaxTemp Thys Vhys VCO }\n", name);
    printf("If given with no arguments, this command prints out the current operational limits.\n");
    printf("You may use 'default' to load the default, or enter custom values in the order shown.\n");
}

BOOL VerifySpp(Q330 *q330)
{
char *arg;
float voltage;
static QDP_TYPE_C1_SPP DefaultSPP = Q330_DEFAULT_TYPE_C1_SPP;

    q330->cmd.code = Q330_CMD_SPP;
    switch (q330->cmd.arglst->count) {

      case 0:
        q330->cmd.update = FALSE;
        return TRUE;

      case 1:
        arg = (char *) q330->cmd.arglst->array[0];
        if (strcasecmp(arg, "default") != 0) {
            SppHelp(q330->cmd.name, 0, NULL);
            return FALSE;
        }
        q330->cmd.spp = DefaultSPP;
        q330->cmd.update = TRUE;
        return TRUE;

      case 9:
        q330->cmd.spp.max_main_current    = atoi((char *) q330->cmd.arglst->array[0]);
        q330->cmd.spp.min_off_time        = atoi((char *) q330->cmd.arglst->array[1]);
                                  voltage = atof((char *) q330->cmd.arglst->array[2]);
        q330->cmd.spp.min_ps_voltage      = (INT16) ((voltage * 1000.0) / 150.0);
        q330->cmd.spp.max_antenna_current = atoi((char *) q330->cmd.arglst->array[3]);
        q330->cmd.spp.min_temp            = atoi((char *) q330->cmd.arglst->array[4]);
        q330->cmd.spp.max_temp            = atoi((char *) q330->cmd.arglst->array[5]);
        q330->cmd.spp.temp_hysteresis     = atoi((char *) q330->cmd.arglst->array[6]);
                                  voltage = atof((char *) q330->cmd.arglst->array[7]);
        q330->cmd.spp.volt_hysteresis     = (INT16) ((voltage * 1000.0) / 150.0);
        q330->cmd.spp.default_vco         = atoi((char *) q330->cmd.arglst->array[8]);
        return TRUE;
    }

    SppHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void spp(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C1_SPP spp;

    if (q330->cmd.update) {
        qdpEncode_C1_SPP(&pkt, &q330->cmd.spp);
        if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
            printf("qdp_C1_SSPP failed\n");
            return;
        }
    }

    if (!qdp_C1_RQSPP(q330->qdp, &spp)) {
        printf("qdp_C1_RQSPP failed\n");
        return;
    }

    qdpPrint_C1_SPP(stdout, &spp);

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
 * $Log: spp.c,v $
 * Revision 1.4  2010/12/13 23:03:22  dechavez
 * use defaults from include/q330.h
 *
 * Revision 1.3  2009/07/23 20:31:04  dechavez
 * improved help support
 *
 * Revision 1.2  2009/02/23 22:13:06  dechavez
 * permit setting individual or default values
 *
 * Revision 1.1  2009/02/04 17:32:45  dechavez
 * initial release
 *
 */
