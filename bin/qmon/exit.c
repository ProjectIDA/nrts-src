#pragma ident "$Id: exit.c,v 1.2 2010/10/25 16:08:15 dechavez Exp $"
/*======================================================================
 *
 * Clean shutdowns
 *
 *====================================================================*/
#include "qmon.h"

static BOOL ShutdownFlag = FALSE;

void SetShutdownFlag(void)
{
    ShutdownFlag = TRUE;
}

BOOL ShutdownInProgress(void)
{
    return ShutdownFlag;
}

void GracefulExit(QMON *qmon, int status)
{
int i;


/* Set the shutdown flag and give any digitizer threads time to exit */

    DisplayShutdownMessage(qmon);
        ShutdownFlag = TRUE;
        for (i = 0; i < 2; i++) {
            sleep(qmon->interval);
            UpdateStatus(qmon);
        }
    ClearCmdWindow(qmon);

/* restore the terminal */

    move(QMON_ROWS, 0);
    refresh();
    endwin();

    exit(status);
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
 * $Log: exit.c,v $
 * Revision 1.2  2010/10/25 16:08:15  dechavez
 * print/clear shutdown message
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
