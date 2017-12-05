#pragma ident "$Id: save.c,v 1.3 2010/12/17 19:53:09 dechavez Exp $"
/*======================================================================
 *
 *  Save current programming
 *
 *====================================================================*/
#include "qmon.h"

static void HelpMesg(QMON *qmon, int argc, char **argv)
{
static char *text = "usage: save [ boot ]\n";

    PopupMessage(qmon, text);
}

static int Verify(QMON *qmon, int argc, char **argv)
{
int cmd;

    switch (argc) {
      case 1:
        cmd = Q330_CMD_SAVE;
        break;
      case 2:
        if (strcasecmp(argv[1], "boot") == 0 || strcasecmp(argv[1], "reboot") == 0) {
            cmd = Q330_CMD_SAVEBOOT;
        } else {
            cmd = Q330_CMD_ERROR;
        }
        break;
      default:
        cmd = Q330_CMD_ERROR;
    }

   return cmd;
}

static BOOL save(QMON *qmon)
{

    return TRUE;
}

void ExecuteSave(QMON *qmon, int argc, char **argv)
{
FILE *fp;
int cmd, errcode;

    if ((cmd = Verify(qmon, argc, argv)) == Q330_CMD_ERROR) {
        HelpMesg(qmon, argc, argv);
        return;
    }

    if (!qdpCtrl(qmon->qdp, QDP_CTRL_SAVE, TRUE)) {
        PopupMessage(qmon, "save to EEPROM command failed\n");
        return;
    }

    if (qmon->newcfg) {
        if ((fp = fopen(qmon->cfg->path.addr, "w")) == NULL) {
            sprintf(qmon->poptxt, "fopen %s: %s", qmon->cfg->path.addr, strerror(errno));
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
        q330WriteAddr(fp, qmon->cfg);
        fclose(fp);
        if ((qmon->cfg = q330ReadCfg(qmon->cfg->root, &errcode)) == NULL) {
            sprintf(qmon->poptxt, "read %s: %s", qmon->cfg->root, strerror(errno));
            PopupMessage(qmon, qmon->poptxt);
            return;
        }
        qmon->newcfg = FALSE;
    }

    qmon->active->flags &= ~UNSAVED_CHANGES;

    if (cmd == Q330_CMD_SAVEBOOT) {
        sleep(5);
        ExecuteBoot(qmon, argc, argv);
    }
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
 * $Log: save.c,v $
 * Revision 1.3  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.2  2010/12/13 23:58:07  dechavez
 * don't clear save flag until after configuration file is updated as well
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
