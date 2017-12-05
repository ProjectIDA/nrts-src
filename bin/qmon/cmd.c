#pragma ident "$Id: cmd.c,v 1.12 2017/09/20 19:29:01 dauerbach Exp $"
/*======================================================================
 *
 *  Process Q330 commands
 *
 *====================================================================*/
#include "qmon.h"

COMMAND_MAP *CommandMap;

COMMAND_MAP map[] = {
    {Q330_CMD_SC,                "sc", "set sensor control map"},
    {Q330_CMD_GPS,              "gps", "clock control"},
    {Q330_CMD_PULSE,          "pulse", "pulse control lines"},
    {Q330_CMD_CALIB,            "cal", "calibration control"},
    {Q330_CMD_LIMIT,            "set", "set operational limits"},
    {Q330_CMD_INBITMAP,       "input", "view or set channel inputs"},
    {Q330_CMD_PREAMP,        "preamp", "view or set channel preamps"},
    {Q330_CMD_SAVE,            "save", "save current config"},
    {Q330_CMD_BOOT,            "boot", "reboot Q330"},
    {Q330_CMD_SAVEBOOT,   "save boot", "save current config and then reboot"},
    {Q330_CMD_GPS,              "gsv", NULL},
    {Q330_CMD_CALIB,          "calib", NULL},
    {Q330_CMD_FLUSH,          "flush", NULL},
    {Q330_CMD_RESYNC,        "resync", NULL},
    {Q330_CMD_BOOT,          "reboot", NULL},
    {Q330_CMD_SAVEBOOT, "save reboot", NULL},
    {-1, NULL, NULL}
};

static BOOL IsRecognizedQ330Command(QMON *qmon, int argc, char **argv)
{
int i;

    for (i = 0; map[i].command != NULL; i++) {
        if (strcasecmp(argv[0], map[i].command) == 0) {
            qmon->cmd.code = map[i].code;
            qmon->cmd.argc = argc;
            qmon->cmd.argv = argv;
            return TRUE;
        }
    }

    qmon->cmd.code = Q330_CMD_UNDEFINED;
    qmon->cmd.argc = 0;
    qmon->cmd.argv = NULL;

    return FALSE;
}

static void ExecuteCommand(QMON *qmon, int argc, char **argv)
{
int errcode, suberr;

    if ((qmon->qdp = q330Register(qmon->active->addr, QDP_CFG_PORT, 1, dbglp(), &errcode, &suberr)) == NULL) {
        if (errcode == QDP_ERR_BUSY) PopupRegerr(qmon, errcode);
        return;
    }

    switch (qmon->cmd.code) {

      case Q330_CMD_SC:
        ExecuteSc(qmon, argc, argv);
        break;

      case Q330_CMD_GPS:
        ExecuteGps(qmon, argc, argv);
        break;

      case Q330_CMD_PULSE:
        ExecutePulse(qmon, argc, argv);
        break;

      case Q330_CMD_CALIB:
        ExecuteCalib(qmon, argc, argv);
        break;

      case Q330_CMD_LIMIT:
        ExecuteLimit(qmon, argc, argv);
        break;

      case Q330_CMD_INBITMAP:
        ExecuteInputBitmap(qmon, argc, argv);
        break;

      case Q330_CMD_PREAMP:
        ExecutePreamp(qmon, argc, argv);
        break;

      case Q330_CMD_SAVE:
        ExecuteSave(qmon, argc, argv);
        break;

      case Q330_CMD_BOOT:
        ExecuteBoot(qmon, argc, argv);
        break;

      case Q330_CMD_SAVEBOOT:
        ExecuteSave(qmon, argc, argv);
        sleep(5);
        ExecuteBoot(qmon, argc, argv);
        break;

      case Q330_CMD_FLUSH:
        ExecuteFlush(qmon, argc, argv);
        break;

      case Q330_CMD_RESYNC:
        ExecuteResync(qmon, argc, argv);
        break;

      default:
        PopupMessage(qmon, QMON_MSG_NOT_IMPLEMENTED);
        break;
    }

    if (qmon->qdp != NULL) {
        qdpShutdown(qmon->qdp);
        qmon->qdp = NULL;
    }
}

void Q330Command(QMON *qmon, int argc, char **argv)
{
    if (!IsRecognizedQ330Command(qmon, argc, argv)) {
        PopupUnrecognized(qmon, argv[0]);
    } else if (!SingleActiveDigitizer(qmon)) {
        PopupMessage(qmon, QMON_MSG_NOT_SINGLE);
    } else {
        ExecuteCommand(qmon, argc, argv);
    }
}

void InitCommandMap(void)
{
    CommandMap = map;
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
 * $Log: cmd.c,v $
 * Revision 1.12  2017/09/20 19:29:01  dauerbach
 * added preamp support
 *
 * Revision 1.11  2016/08/26 20:15:38  dechavez
 * Use qdpShutdown() instead of qdpDeregister() when breaking connections
 *
 * Revision 1.10  2016/07/20 22:09:42  dechavez
 * added suberr to q330Register() calls for libqdp 3.13.0 compatibility
 *
 * Revision 1.9  2015/12/07 19:19:56  dechavez
 * removed dead code
 *
 * Revision 1.8  2014/04/18 20:49:54  dechavez
 * added Q330_CMD_SAVEBOOT support
 *
 * Revision 1.7  2011/01/07 23:03:01  dechavez
 * added "set" support
 *
 * Revision 1.6  2011/01/03 21:32:10  dechavez
 * added 'input' command support
 *
 * Revision 1.5  2010/12/23 22:02:54  dechavez
 * connect to QDP_CFG_PORT instead of QDP_PORT_CONFIG (which used to work only
 * because the two happened to match by coincidence but then that got lost following
 * the reorg of symbolic port names in qdp.h)
 *
 * Revision 1.4  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.3  2010/12/13 22:45:34  dechavez
 * initial Q330 command support
 *
 * Revision 1.2  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.1  2010/12/08 19:15:44  dechavez
 * created (but no commands actually supported)
 *
 */
