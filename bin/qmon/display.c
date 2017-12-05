#pragma ident "$Id: display.c,v 1.19 2011/07/27 23:47:14 dechavez Exp $"
/*======================================================================
 *
 * Main display screen
 *
 *====================================================================*/
#include "qmon.h"

void DrawSeparator(WINDOW *win, char value, int count)
{
int i;

    wmove(win, 0, 0);
    for (i = 0; i < QMON_COLS; i++) waddch(win, value);
    wrefresh(win);
}

void ClearToEol(WINDOW *win, int row, int col)
{
    wmove(win, row, col);
    wclrtoeol(win);
}

void PrintHostTime(WINDOW *win, int row)
{
int col = 58;

    ClearToEol(win, row, col);
    mvwprintw(win, row, col, "Host %s", utilLttostr(time(NULL), 0, NULL));
}

void ShowVersionNumber(WINDOW *win)
{
int col;
char version[QMON_COLS+1];

    sprintf(version, " qmon %s ", VersionIdentString);

    col = QMON_COLS/2 - strlen(version)/2;
    mvwprintw(win, 0, col, " qmon %s ", VersionIdentString);
    wrefresh(win);
}

void UpdateStatus(QMON *qmon)
{

    switch (qmon->mode) {
      case QMON_MODE_ISI:  UpdateISI(qmon);  break;
      case QMON_MODE_NRTS: UpdateNRTS(qmon); break;
      default:             UpdateQ330(qmon);
    }

/* move the cursor to the end of the command line */

    SetCursorPosition(qmon);

    refresh();
}

void RefreshDisplay(QMON *qmon)
{

    qmon->mode = QMON_MODE_Q330;

    ClearPopup();

    wclear(qmon->win.lst);
    wclear(qmon->win.sp1);
    wclear(qmon->win.top);
    wclear(qmon->win.sp2);
    wclear(qmon->win.bot);
    wclear(qmon->win.sp3);
    wclear(qmon->win.cmd);

    DrawDigitizerList(qmon);
    DrawSeparator(qmon->win.sp1, QMON_WIN_BOUNDARY_CHAR, QMON_COLS);
    DrawSeparator(qmon->win.sp2, qmon->sp2char,          QMON_COLS);
    DrawSeparator(qmon->win.sp3, QMON_WIN_BOUNDARY_CHAR, QMON_COLS);
    DrawCmdWindow(qmon);
    UpdateStatus(qmon);
    ShowVersionNumber(qmon->win.sp1);
    refresh();
}

BOOL InitWindows(QMON *qmon)
{
int row;
BOOL error;

    if (initscr() == NULL) {
        endwin();
        perror("initscr");
        return FALSE;
    }

    cbreak();
    noecho();
    halfdelay(qmon->interval * 10);

    if (COLS < QMON_COLS || LINES < QMON_ROWS) {
        endwin();
        fprintf(stderr, "Minimum screen size is %d by %d.\n", QMON_COLS, QMON_ROWS);
        return FALSE;
    }

    error = FALSE;
    row = 0;
    if ((qmon->win.lst = newwin(QMON_LST_ROWS, QMON_LST_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_LST_ROWS;
    if ((qmon->win.sp1 = newwin(QMON_SP1_ROWS, QMON_SP1_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_SP1_ROWS;
    if ((qmon->win.top = newwin(QMON_TOP_ROWS, QMON_TOP_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_TOP_ROWS;
    if ((qmon->win.sp2 = newwin(QMON_SP2_ROWS, QMON_SP2_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_SP2_ROWS;
    if ((qmon->win.bot = newwin(QMON_BOT_ROWS, QMON_BOT_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_BOT_ROWS;
    if ((qmon->win.sp3 = newwin(QMON_SP3_ROWS, QMON_SP3_COLS, row, 0)) == NULL) error = TRUE;
    row += QMON_SP3_ROWS;
    if ((qmon->win.cmd = newwin(QMON_CMD_ROWS, QMON_CMD_COLS, row, 0)) == NULL) error = TRUE;
    qmon->cmdrow = row;

    if ((qmon->win.isi = newwin(QMON_ISI_ROWS, QMON_ISI_COLS,   0, 0)) == NULL) error = TRUE;

    keypad(qmon->win.cmd, TRUE);

    refresh();

    if (error) {
        endwin();
        perror("newwin");
        return FALSE;
    }

    return TRUE;
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
 * $Log: display.c,v $
 * Revision 1.19  2011/07/27 23:47:14  dechavez
 * moved q330 specific stuff over to q330.c, added support for multi-mode displays (q330, isi, nrts)
 *
 * Revision 1.18  2011/01/25 18:53:32  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.17  2010/12/27 22:45:38  dechavez
 * zero token (CR) command now just clears any pop ups (no more selection shift)
 *
 * Revision 1.16  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.15  2010/12/13 22:45:34  dechavez
 * initial Q330 command support
 *
 * Revision 1.14  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.13  2010/12/08 19:18:47  dechavez
 * added code to deal with active pop ups
 *
 * Revision 1.12  2010/12/06 17:37:09  dechavez
 * Leave old info on screen when state is QMON_STATE_UNCONNECTED
 *
 * Revision 1.11  2010/12/02 22:59:13  dechavez
 * removed lingering debug 'x'
 *
 * Revision 1.9  2010/12/02 20:15:37  dechavez
 * restored registration state messages for unconnected digitizers, cleaned
 * up clearing of bottom window when toggling between single to dual mode,
 * digitizer name in tagline highlighted only when fully registered
 *
 * Revision 1.8  2010/11/24 22:04:16  dechavez
 * Reverse video calibration stations when anything other than "off".
 * Display "Baro: n/a" when no barometer data are available.
 * Display qmon version ident
 *
 * Revision 1.7  2010/11/24 19:29:42  dechavez
 * reverse video inconsistent sensor control maps
 *
 * Revision 1.6  2010/11/16 21:52:05  dechavez
 * fixed "flashing" effects visible on sites with slow tail circuits by only
 * updating the screen when the monitor data had changed
 *
 * Revision 1.5  2010/10/25 16:08:54  dechavez
 * don't clobber tag line with deregistered state message (leave intact to
 * keep info on the screen after program exit)
 *
 * Revision 1.4  2010/10/24 19:20:21  dechavez
 * Fixed lat instead of lon display bug
 *
 * Revision 1.3  2010/10/20 18:40:45  dechavez
 * calculate clock qual using default clock tokens, added support for barometer status
 *
 * Revision 1.2  2010/10/19 20:56:30  dechavez
 * set clrbot flag if needed
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
