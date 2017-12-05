#pragma ident "$Id: process.c,v 1.10 2015/12/23 22:48:43 dechavez Exp $"
/*======================================================================
 *
 *  Process user commands
 *
 *====================================================================*/
#include "qmon.h"

#define MAXTOKEN QMON_COLS

void ProcessCommand(QMON *qmon, char *cmdstr)
{
int ntoken;
static char *token[MAXTOKEN];
static char *fid = "ProcessCommand";

    if (ActivePopup()) RefreshDisplay(qmon);

    if ((ntoken = utilParse(cmdstr, token, " ", MAXTOKEN, 0)) == 0) {
        if (qmon->mode == QMON_MODE_NRTS) isidlIncrMonNRTSchn(qmon->mon);
        return;
    }

/* check for explicit digitizer names */

    if (ntoken == 1 && UpdateDigitizerSelection(qmon, token[0], NULL    )) return;
    if (ntoken == 2 && UpdateDigitizerSelection(qmon, token[0], token[1])) return;

/* other qmon commands */

    if (strcasecmp(token[0], "help") == 0) {
        PopupHelp(qmon, ntoken, token);
        return;
    }

    if (ntoken == 1) {

        if (strcasecmp(token[0], "q") == 0 || strcasecmp(token[0], "quit") == 0) {
            GracefulExit(qmon, 0);
            return;
        }

        if (strcasecmp(token[0], "q330") == 0) {
            PopupQ330Help(qmon);
            return;
        }

        if (strcasecmp(token[0], "debug") == 0) {
            qmon->PopDebugMessage = TRUE;
            return;
        }

        if (strcasecmp(token[0], "i") == 0 || strcasecmp(token[0], "isi") == 0) {
            if (qmon->mon != NULL) {
                SetISIdisplay(qmon);
            } else {
                PopupMessage(qmon, "no ISI disk loops avaible");
            }
            return;
        }

        if (strcasecmp(token[0], "n") == 0 || strcasecmp(token[0], "nrts") == 0) {
            if (qmon->mon != NULL) {
                SetNRTSdisplay(qmon);
            } else {
                PopupMessage(qmon, "no NRTS disk loops avaible");
            }
            return;
        }

        if (strcasecmp(token[0], "forever") == 0) {
            SetMaxIdle(-1);
            PopupMessage(qmon, "idle timeout disabled");
            return;
        }
    }

/* everthing else is assumed to be a command for the Q330 */

    Q330Command(qmon, ntoken, token);
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
 * $Log: process.c,v $
 * Revision 1.10  2015/12/23 22:48:43  dechavez
 * Only support the "isi" and "nrts" commands when there are disk loops underneath
 * to display.  Give a pop-up message if they aren't.
 *
 * Revision 1.9  2011/07/27 23:45:08  dechavez
 * added i/isi, n/nrts, and forever commands.  Added code to handle empty CR
 * to shift NRTS channels, when in NRTS display mode
 *
 * Revision 1.8  2011/01/07 22:59:30  dechavez
 * allowed "help" command to have args (processed by PopupHelp()) and added
 * "q330" command to list Q330 commands
 *
 * Revision 1.7  2010/12/27 22:45:38  dechavez
 * zero token (CR) command now just clears any pop ups (no more selection shift)
 *
 * Revision 1.6  2010/12/13 22:45:34  dechavez
 * initial Q330 command support
 *
 * Revision 1.5  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.4  2010/12/08 19:17:21  dechavez
 * added Q330Command
 *
 * Revision 1.3  2010/10/22 22:58:11  dechavez
 * added ENTER=ShiftSelection() and +=AddDigitizer() commands
 *
 * Revision 1.2  2010/10/19 20:54:25  dechavez
 * accept empty commands gracefully
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
