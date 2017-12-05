#pragma ident "$Id: cmd.c,v 1.24 2017/09/13 23:42:37 dauerbach Exp $"
/*======================================================================
 *
 *  Parse command and arguments
 *
 *====================================================================*/
#include "q330cc.h"

COMMAND_MAP *CommandMap;

COMMAND_MAP map[] = {
    {Q330_CMD_CALIB,          "calib", "start, stop calibration"},
    {Q330_CMD_CALIB,            "cal", NULL},
    {Q330_CMD_STATUS,        "status", "status reports"},
    {Q330_CMD_STATUS,           "soh", NULL},
    {Q330_CMD_CO,                "co", "checkout report"},
    {Q330_CMD_CO,          "checkout", NULL},
    {Q330_CMD_COMBO,          "combo", "print combo packet"},
    {Q330_CMD_EP,               "ep",  "environmental processor reports"},
    {Q330_CMD_GPS,              "gps", "GPS command and control"},
    {Q330_CMD_PULSE,          "pulse", "pulse control lines"},
    {Q330_CMD_AMASS,          "amass", "automatic mass recentering report"},
    {Q330_CMD_DCP,              "dcp", "channel offset and gains report"},
    {Q330_CMD_FIX,              "fix", "fixed values after reboot report"},
    {Q330_CMD_GLOB,            "glob", "global programming report"},
    {Q330_CMD_ID,                "id", "device identifier report"},
    {Q330_CMD_MAN,              "man", "manufacturer's area report"},
    {Q330_CMD_IFCONFIG,    "ifconfig", "view or set IP addresses"},
    {Q330_CMD_SC,                "sc", "view or set sensor control map"},
    {Q330_CMD_SPP,              "spp", "view or set operational limits"},
    {Q330_CMD_INPUT,          "input", "view or set channel inputs"},
    {Q330_CMD_PREAMP,        "preamp", "view or set channel gain (preamp)"},
    {Q330_CMD_SAVE,            "save", "save current programming to EEPROM"},
    {Q330_CMD_TOKENS,        "tokens", "view DP tokens for a particular data port"},
    {Q330_CMD_FLUSH,          "flush", "flush a data port"},
    {Q330_CMD_RESYNC,        "resync", "resync Q330"},
    {Q330_CMD_REBOOT,          "boot", "reboot Q330"},
    {Q330_CMD_REBOOT,        "reboot", NULL},
    {Q330_CMD_SAVEBOOT,   "save boot", "save current programming and then reboot"},
    {Q330_CMD_SAVEBOOT, "save reboot", NULL},
    {Q330_CMD_XML,              "xml", "XML dump of current programming"},
    {Q330_CMD_CONFIG,        "config", "configure digitizer"},
    {-1, NULL, NULL}
};

int CommandCode(char *command)
{
int i;

    for (i = 0; CommandMap[i].command != NULL; i++) {
        if (strcasecmp(command, CommandMap[i].command) == 0) return CommandMap[i].code;
    }
    return Q330_CMD_UNDEFINED;
}

BOOL BadArgLstCount(Q330 *q330)
{
    printf("ERROR: wrong number of arguments for command '%s'\n", q330->cmd.name);
    return FALSE;
}

BOOL UnexpectedArg(Q330 *q330, char *arg)
{
    printf("ERROR: uexpected %s argument '%s'\n", q330->cmd.name, arg);
    return FALSE;
}

static void PrintArgList(Q330 *q330)
{
int i;
char *arg;

    if (q330->cmd.arglst->count == 0) {
        printf("Argument list is empty\n");
    } else {
        printf("Argument list contains %d entries\n", q330->cmd.arglst->count);
        for (i = 0; i < q330->cmd.arglst->count; i++) printf("%d: %s\n", i+1, (char *) q330->cmd.arglst->array[i]);
    }
}

BOOL VerifyQ330Cmd(Q330 *q330)
{
char *arg;
static char *fid = "VerifyQ330Cmd";

    if (q330->cmd.name == NULL) {
        printf("ERROR: no command given\n");
        return FALSE;
    }

    if (!listSetArrayView(q330->cmd.arglst)) {
        printf("%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    switch (CommandCode(q330->cmd.name)) {
      case Q330_CMD_REBOOT:   return VerifyReboot(q330);
      case Q330_CMD_SAVE:     return VerifySave(q330);
      case Q330_CMD_RESYNC:   return VerifyResync(q330);
      case Q330_CMD_GPS:      return VerifyGps(q330);
      case Q330_CMD_CALIB:    return VerifyCalib(q330);
      case Q330_CMD_IFCONFIG: return VerifyIfconfig(q330);
      case Q330_CMD_STATUS:   return VerifyStatus(q330);
      case Q330_CMD_ID:       return VerifyId(q330);
      case Q330_CMD_FIX:      return VerifyFix(q330);
      case Q330_CMD_GLOB:     return VerifyGlob(q330);
      case Q330_CMD_SC:       return VerifySC(q330);
      case Q330_CMD_PULSE:    return VerifyPulse(q330);
      case Q330_CMD_AMASS:    return VerifyAmass(q330);
      case Q330_CMD_DCP:      return VerifyDcp(q330);
      case Q330_CMD_SPP:      return VerifySpp(q330);
      case Q330_CMD_MAN:      return VerifyMan(q330);
      case Q330_CMD_CO:       return VerifyCheckout(q330);
      case Q330_CMD_COMBO:    return VerifyCombo(q330);
      case Q330_CMD_TOKENS:   return VerifyTokens(q330);
      case Q330_CMD_FLUSH:    return VerifyFlush(q330);
      case Q330_CMD_XML:      return VerifyXml(q330);
      case Q330_CMD_CONFIG:   return VerifyConfig(q330);
      case Q330_CMD_INPUT:    return VerifyInput(q330);
      case Q330_CMD_EP:       return VerifyEp(q330);
      case Q330_CMD_PREAMP:   return VerifyPreamp(q330);
    }

    printf("ERROR: unrecognized command '%s'\n", q330->cmd.name);
    return FALSE;
}

void InitCommandMap()
{
    CommandMap = map;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: cmd.c,v $
 * Revision 1.24  2017/09/13 23:42:37  dauerbach
 * Implement q330 preamp command to turn 'on' or 'off' preamp for sensor 'a' or 'b'
 *
 * Revision 1.23  2016/06/23 20:31:24  dechavez
 * added combo support
 *
 * Revision 1.22  2016/01/21 19:38:31  dechavez
 * added environmenal processor support
 *
 * Revision 1.21  2014/01/24 00:07:16  dechavez
 * added 'reboot' alias for 'boot' and 'save reboot' alias for 'save boot' commands
 *
 * Revision 1.20  2011/12/20 22:29:10  dechavez
 * changed boot commands from reboot to boot
 *
 * Revision 1.19  2010/11/15 23:06:56  dechavez
 * added flush support
 *
 * Revision 1.18  2010/04/07 19:50:37  dechavez
 * added "input" command
 *
 * Revision 1.17  2010/03/19 00:35:47  dechavez
 * added calib status
 *
 * Revision 1.16  2009/10/29 19:36:46  dechavez
 * added config support
 *
 * Revision 1.15  2009/09/22 19:46:48  dechavez
 * xml command
 *
 * Revision 1.14  2009/09/15 23:15:35  dechavez
 * added support for "tokens" command
 *
 * Revision 1.13  2009/07/23 21:44:38  dechavez
 * reordered list
 *
 * Revision 1.12  2009/07/23 21:38:24  dechavez
 * changed spp description
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
 * Revision 1.7  2009/01/05 22:47:36  dechavez
 * added save command
 *
 * Revision 1.6  2008/10/02 23:05:10  dechavez
 * added ifconfig, status, and fixvar
 *
 * Revision 1.5  2008/03/11 22:20:08  dechavez
 * remove trigger
 *
 * Revision 1.4  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
