#pragma ident "$Id: main.c,v 1.24 2017/09/13 23:42:37 dauerbach Exp $"
/*======================================================================
 *
 *  Q330 command and control
 *
 *====================================================================*/
#include "q330cc.h"

int main (int argc, char **argv)
{
Q330 q330;
int exitstatus = 0;

    init(argc, argv, &q330);

    switch (q330.cmd.code) {

      case Q330_CMD_REBOOT:
        boot(&q330);
        break;

      case Q330_CMD_SAVE:
        save(&q330);
        break;

      case Q330_CMD_SAVEBOOT:
        save(&q330);
        sleep(5);
        boot(&q330);
        break;

      case Q330_CMD_RESYNC:
        resync(&q330);
        break;

      case Q330_CMD_GPS_ON:
        gpsON(&q330);
        break;

      case Q330_CMD_GPS_OFF:
        gpsOFF(&q330);
        break;

      case Q330_CMD_GPS_COLD:
        gpsColdStart(&q330);
        break;

      case Q330_CMD_GPS_CNF:
        gpsCnf(&q330);
        break;

      case Q330_CMD_GPS_ID:
        gpsId(&q330);
        break;

      case Q330_CMD_CAL_START:
        calStart(&q330);
        break;

      case Q330_CMD_CAL_STOP:
        calStop(&q330);
        break;

      case Q330_CMD_CAL_STATUS:
        calStatus(&q330);
        break;

      case Q330_CMD_IFCONFIG:
        ifconfig(&q330);
        break;

      case Q330_CMD_STATUS:
        status(&q330);
        break;

      case Q330_CMD_FIX:
        fix(&q330);
        break;

      case Q330_CMD_GLOB:
        glob(&q330);
        break;

      case Q330_CMD_SC:
        sc(&q330);
        break;

      case Q330_CMD_PULSE:
        pulse(&q330);
        break;

      case Q330_CMD_AMASS:
        amass(&q330);
        break;

      case Q330_CMD_DCP:
        dcp(&q330);
        break;

      case Q330_CMD_SPP:
        spp(&q330);
        break;

      case Q330_CMD_MAN:
        man(&q330);
        break;

      case Q330_CMD_CO:
        checkout(&q330);
        break;

      case Q330_CMD_ID:
        id(&q330);
        break;

      case Q330_CMD_TOKENS:
        tokens(&q330);
        break;

      case Q330_CMD_FLUSH:
        flush(&q330);
        break;

      case Q330_CMD_XML:
        xml(&q330);
        break;

      case Q330_CMD_CONFIG:
        config(&q330);
        break;

      case Q330_CMD_PREAMP:
        preamp(&q330);
        break;

      case Q330_CMD_INPUT:
        input(&q330);
        break;

      case Q330_CMD_EP_CNF:
        EpCnf(&q330);
        break;

      case Q330_CMD_EP_DELAY:
        EpDelay(&q330);
        break;

      case Q330_CMD_COMBO:
        combo(&q330);
        break;

      default:
        printf("ERROR: command '%s' is unsupported\n", q330.cmd.name);
        exitstatus = 1;
        break;

    }

    GracefulExit(&q330, exitstatus);
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
 * $Log: main.c,v $
 * Revision 1.24  2017/09/13 23:42:37  dauerbach
 * Implement q330 preamp command to turn 'on' or 'off' preamp for sensor 'a' or 'b'
 *
 * Revision 1.23  2016/06/23 20:31:24  dechavez
 * added combo support
 *
 * Revision 1.22  2016/01/23 00:23:40  dechavez
 * added Q330_CMD_EP_DELAY support
 *
 * Revision 1.21  2016/01/21 19:38:31  dechavez
 * added environmenal processor support
 *
 * Revision 1.20  2011/10/13 18:23:19  dechavez
 * GracefulExit()s
 *
 * Revision 1.19  2010/11/15 23:06:57  dechavez
 * added flush support
 *
 * Revision 1.18  2010/06/14 19:26:39  dechavez
 * removed unused variables
 *
 * Revision 1.17  2010/04/07 19:50:37  dechavez
 * added "input" command
 *
 * Revision 1.16  2010/03/19 00:35:47  dechavez
 * added calib status
 *
 * Revision 1.15  2009/10/29 19:36:46  dechavez
 * added config support
 *
 * Revision 1.14  2009/09/22 19:46:48  dechavez
 * xml command
 *
 * Revision 1.13  2009/09/15 23:15:34  dechavez
 * added support for "tokens" command
 *
 * Revision 1.12  2009/07/25 17:38:26  dechavez
 * set exit status
 *
 * Revision 1.11  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 * Revision 1.10  2009/07/10 18:34:34  dechavez
 * added id command
 *
 * Revision 1.9  2009/02/04 17:50:33  dechavez
 * support for spp, dcp, man, gps cnf, gps id, save (re)boot
 *
 * Revision 1.8  2009/01/24 00:14:49  dechavez
 * support for amass, sc, and pulse commands
 *
 * Revision 1.7  2009/01/05 22:47:20  dechavez
 * added save command
 *
 * Revision 1.6  2008/10/02 23:05:10  dechavez
 * added ifconfig, status, and fixvar
 *
 * Revision 1.5  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
