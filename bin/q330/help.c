#pragma ident "$Id: help.c,v 1.13 2017/09/13 23:42:37 dauerbach Exp $"
/*======================================================================
 *
 *  Help messages
 *
 *====================================================================*/
#include "q330cc.h"

void help(Q330_CFG *cfg, int argc, char **argv)
{
int i;

    if (argc <= 2) {
        PrintCommandLineHelp(argv[0]);
        printf("\n");
        printf("Command set (for additional help, use 'q330 help <cmd>')\n");
        for (i = 0; CommandMap[i].command != NULL; i++) {
            if (CommandMap[i].description != NULL) printf("%11s - %s\n", CommandMap[i].command, CommandMap[i].description);
        }
        printf("\n");
        exit(0);
    }

    switch (CommandCode(argv[2])) {
      case Q330_CMD_REBOOT:
        RebootHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_SAVE:
        SaveHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_RESYNC:
        ResyncHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_GPS:
        GpsHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_CALIB:
        CalibHelp(cfg, argv[2], argc, argv);
        break;
      case Q330_CMD_IFCONFIG:
        IfconfigHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_STATUS:
        StatusHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_ID:
        IdHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_FIX:
        FixHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_GLOB:
        GlobHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_SC:
        SCHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_PULSE:
        PulseHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_AMASS:
        AmassHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_DCP:
        DcpHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_SPP:
        SppHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_MAN:
        ManHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_CO:
        CheckoutHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_TOKENS:
        TokensHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_XML:
        XmlHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_CONFIG:
        ConfigHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_PREAMP:
        PreampHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_INPUT:
        InputHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_FLUSH:
        FlushHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_EP:
        EpHelp(argv[2], argc, argv);
        break;
      case Q330_CMD_COMBO:
        ComboHelp(argv[2], argc, argv);
        break;
      default:
        printf("%s: no such command\n", argv[2]);
    }

    exit(0);
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
 * $Log: help.c,v $
 * Revision 1.13  2017/09/13 23:42:37  dauerbach
 * Implement q330 preamp command to turn 'on' or 'off' preamp for sensor 'a' or 'b'
 *
 * Revision 1.12  2016/06/23 20:31:24  dechavez
 * added combo support
 *
 * Revision 1.11  2016/02/04 00:33:10  dechavez
 * use PrintCommandLineHelp() from init.c for command line summary
 *
 * Revision 1.10  2016/01/21 19:38:31  dechavez
 * added environmenal processor support
 *
 * Revision 1.9  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.8  2010/11/15 23:06:57  dechavez
 * added flush support
 *
 * Revision 1.7  2010/04/07 20:26:44  dechavez
 * added input command
 *
 * Revision 1.6  2009/10/29 19:36:46  dechavez
 * added config support
 *
 * Revision 1.5  2009/09/22 19:46:48  dechavez
 * xml command
 *
 * Revision 1.4  2009/09/15 23:15:35  dechavez
 * added support for "tokens" command
 *
 * Revision 1.3  2009/07/28 18:14:49  dechavez
 * pass configuration to CalibHelp()
 *
 * Revision 1.2  2009/07/23 21:53:11  dechavez
 * *** empty log message ***
 *
 * Revision 1.1  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 */
