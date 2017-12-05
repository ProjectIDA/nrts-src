#pragma ident "$Id: input.c,v 1.7 2017/09/06 22:07:52 dechavez Exp $"
/*======================================================================
 *
 * Handle user input
 *
 *====================================================================*/
#include "qmon.h"

#define PROMPT_NORMAL "> "
#define PROMPT_EXIT   "[ shutdown in progress ]"
#define PROMPT_EMPTY  ""

static time_t MaxIdleTime = DEFAULT_MAX_IDLE_TIME;
static int cmdcol = 0;
static int cmdndx = 0;
static char *EmptyMessage = PROMPT_EMPTY;
static char *ShutdownMessage = PROMPT_EXIT;
static char *prompt = PROMPT_NORMAL;

#define ERAS 0007
#define BS   0010
#define TAB  0011
#define DEL  0177
#define ESC  0033
#define CR   0012
#define CLR  0025

void SetCursorPosition(QMON *qmon)
{
    move(qmon->cmdrow, cmdcol);
    wrefresh(qmon->win.cmd);
}

void DrawCmdWindow(QMON *qmon)
{
    wmove(qmon->win.cmd, 0, 0);
    wclrtoeol(qmon->win.cmd);
    waddstr(qmon->win.cmd, prompt);
    cmdcol = strlen(prompt);
    wrefresh(qmon->win.cmd);
}

void DisplayShutdownMessage(QMON *qmon)
{
    prompt = ShutdownMessage;
    DrawCmdWindow(qmon);
}

void ClearCmdWindow(QMON *qmon)
{
    prompt = EmptyMessage;
    DrawCmdWindow(qmon);
}

static void Prompt(QMON *qmon)
{

    DrawCmdWindow(qmon);
    cmdndx = 0;
    cmdcol = strlen(prompt);
    SetCursorPosition(qmon);
    refresh();
}

void SetMaxIdle(time_t value)
{
    MaxIdleTime = value;
}

char *GetUserInput(QMON *qmon)
{
int input, full;
static time_t LastAction = 0;
static char cmdbuf[QMON_COLS+1];

    while (1) {

    /* check for shutdown signal */

        if (ShutdownInProgress()) return NULL;

    /* check for idle time out */

        if (LastAction == 0) LastAction = time(NULL);
        if (MaxIdleTime > 0  && (time(NULL) - LastAction > MaxIdleTime)) return NULL;

    /* read next character (or timeout waiting) */

        input = wgetch(qmon->win.cmd);

        switch (input) {

          case ERR: /* timeout */
            UpdateStatus(qmon);
            break;

          case ESC:
            cmdndx = 0;
            RefreshDisplay(qmon);
            break;

          case TAB:
            cmdndx = 0;
            AddRemDigitizer(qmon);
            break;

          case KEY_BACKSPACE:
          case DEL:
          case ERAS:
            input = BS;
          case BS:
            if (cmdndx != 0) {
                --cmdndx;
                --cmdcol;
                wechochar(qmon->win.cmd, input);
                wechochar(qmon->win.cmd, ' ');
                wechochar(qmon->win.cmd, input);
            } 
            break;

          case CLR:
            Prompt(qmon);
            break;

          case KEY_LEFT:
            switch (qmon->mode) {
              case QMON_MODE_ISI:  isidlDecrMonISI(qmon->mon);  break;
              case QMON_MODE_NRTS: isidlDecrMonNRTS(qmon->mon); break;
              default: ShiftSelectionLeft(qmon);
            }
            break;

          case KEY_RIGHT:
            switch (qmon->mode) {
              case QMON_MODE_ISI:  isidlIncrMonISI(qmon->mon);  break;
              case QMON_MODE_NRTS: isidlIncrMonNRTS(qmon->mon); break;
              default: ShiftSelectionRight(qmon);
            }
            break;

          case CR:
            if (cmdndx == 0 && ActivePopup()) {
                RefreshDisplay(qmon);
            } else {
                cmdbuf[cmdndx] = 0;
                Prompt(qmon);
                return cmdbuf;
            }
            break;

          case 'P':
            if (cmdndx == 0) TogglePercentResentView();
            break;

          case 'D':
            if (cmdndx == 0) ToggleDecimalDegreesView();
            break;

          case '?':
            if (cmdndx == 0) PopupHelp(qmon, 0, NULL);
            break;

          default:
            if (isascii(input)) {
                cmdbuf[cmdndx] = input;
                wechochar(qmon->win.cmd, cmdbuf[cmdndx]);
                ++cmdndx;
                ++cmdcol;
                SetCursorPosition(qmon);
            }
            break;
        }
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
 * $Log: input.c,v $
 * Revision 1.7  2017/09/06 22:07:52  dechavez
 * added 'D' command to immediately invoke ToggleDecimalDegreesView()
 *
 * Revision 1.6  2015/12/23 22:51:53  dechavez
 * mods required after qmon->mon got switched to a pointer
 *
 * Revision 1.5  2013/01/24 22:19:57  dechavez
 * added 'P' command to immediately invoke TogglePercentResentView();
 *
 * Revision 1.4  2011/07/27 23:49:13  dechavez
 * added isi and nrts disk loop increment/decrement support for left/right arrow keys
 *
 * Revision 1.3  2011/01/07 23:02:42  dechavez
 * pass (null) args to PopupHelp() in response to ?
 *
 * Revision 1.2  2010/12/27 22:45:38  dechavez
 * zero token (CR) command now just clears any pop ups (no more selection shift)
 *
 * Revision 1.1  2010/12/08 19:16:06  dechavez
 * formerly cmdio.c
 *
 */
