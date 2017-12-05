#pragma ident "$Id: isi.c,v 1.2 2015/12/23 22:51:04 dechavez Exp $"
/*======================================================================
 *
 * ISI display - should never get here if qmon->mon is NULL
 *
 *====================================================================*/
#include "qmon.h"

#define LABEL " ISI Disk Loop Status "
static int StartingRow = 0;

void UpdateISI(QMON *qmon)
{
WINDOW *win;

    win = qmon->win.isi;

    PrintHostTime(win, 0);
    isidlMonUpdateISI(win, qmon->mon, StartingRow);
    wrefresh(win);
    return;
}

static void SetMonDL(QMON *qmon)
{
LNKLST_NODE *crnt;
DIGITIZER *digitizer;
static char target[MAXPATHLEN+1], *name;

    crnt = listFirstNode(qmon->avail);
    while (crnt != NULL) {
        digitizer = (DIGITIZER *) crnt->payload;
        strncpy(target, digitizer->addr->connect.ident, MAXPATHLEN);
        if (digitizer->win[0] != NULL) break;
        crnt = listNextNode(crnt);
    }

/* At this point we have a name, hopefully of the form siteXX. Truncate it. */

    if (strlen(target) > 2) target[strlen(target)-2] = 0;
    isidlSetMonISI(qmon->mon, target);
}

void SetISIdisplay(QMON *qmon)
{
WINDOW *win;
int i, row, col;

    SetMonDL(qmon);
    qmon->mode = QMON_MODE_ISI;
    win = qmon->win.isi;

    wclear(win);

    row = QMON_LST_ROWS;
    wmove(win, row, 0);
    for (i = 0; i < QMON_COLS; i++) waddch(win, QMON_WIN_BOUNDARY_CHAR);
    col = QMON_ISI_COLS/2 - strlen(LABEL)/2;
    mvwprintw(win, row, col, LABEL);
    StartingRow = row + 3;

    isidlMonStaticISI(win, StartingRow);

    UpdateISI(qmon);
    return;
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
 * $Log: isi.c,v $
 * Revision 1.2  2015/12/23 22:51:04  dechavez
 * mods required after qmon->mon got switched to a pointer
 *
 * Revision 1.1  2011/07/27 23:50:56  dechavez
 * created
 *
 */
