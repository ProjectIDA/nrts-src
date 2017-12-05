#pragma ident "$Id: pop.c,v 1.8 2015/12/23 22:49:43 dechavez Exp $"
/*======================================================================
 *
 *  Pop-up windows
 *
 *====================================================================*/
#include "qmon.h"

#define POP_DUR 10

static struct {
    WINDOW *win;
    time_t tstamp;
} pop = {NULL, 0};

typedef struct {
    int nrow;
    int ncol;
    char **row;
} MESSAGE;

static MESSAGE *CreateMessage(char *string)
{
LNKLST *list;
MESSAGE *message;
int i, j, len, nrow, ncol = 0;

    if ((list = utilStringTokenList(string, "\n", 0)) == NULL) return NULL;
    if (!listSetArrayView(list)) return NULL;

    for (i = 0; i < list->count; i++) {
        len = strlen((char *) list->array[i]);
        if (ncol < len) ncol = len;
    }

    ncol += 2; /* include left and right borders */
    nrow = list->count + 2; /* include top and bottom borders */

    if ((message = (MESSAGE *) malloc(sizeof(MESSAGE))) == NULL) return NULL;
    if ((message->row = (char **) malloc(nrow * sizeof(char *))) == NULL) return NULL;

    for (i = 0; i < nrow; i++) {
        message->row[i] = (char *) malloc((ncol+1) * sizeof(char)); /* add 1 for NULL */
        if (message->row[i] == NULL) return NULL;
    }

    memset(message->row[0], ' ', ncol); message->row[0][ncol] = 0;
    for (i=1, j=0; i < nrow-1; i++, j++) sprintf(message->row[i], " %s ", (char *) list->array[j]);
    memset(message->row[nrow-1], ' ', ncol); message->row[nrow-1][ncol] = 0;

    listDestroy(list);

    message->nrow = nrow;
    message->ncol = ncol;

    return message;
}

static void DestroyMessage(MESSAGE *message)
{
int i;

    if (message == NULL) return;
    for (i = 0; i < message->nrow; i++) free(message->row[i]);
    free(message);
}

static void DisplayMessage(QMON *qmon, MESSAGE *message)
{
int i, j, row, col, npad;

    row = (QMON_ROWS / 2) - (message->nrow / 2);
    col = (QMON_COLS / 2) - (message->ncol / 2);

    if ((pop.win = newwin(message->nrow, message->ncol, row, col)) == NULL) return;
    wattron(pop.win, A_STANDOUT);

    wclear(pop.win);
    for (i = 0; i < message->nrow; i++) {
        mvwaddstr(pop.win, i, 0, message->row[i]);
        npad = message->ncol - strlen(message->row[i]);
        for (j = 0; j < npad; j++) wprintw(pop.win, " ");
    }
    wrefresh(pop.win);
    pop.tstamp = time(NULL);

    UpdateStatus(qmon);
}

void PopupMessage(QMON *qmon, char *string)
{
MESSAGE *message;

    if ((message = CreateMessage(string)) == NULL) return;
    DisplayMessage(qmon, message);
    DestroyMessage(message);
}

void PopupQ330Help(QMON *qmon)
{
int i;
static char *top =
"Q330 commands                            \n"
"-------------                            \n";

    strcpy(qmon->poptxt, top);
    for (i = 0; CommandMap[i].command != NULL; i++) {
        if (CommandMap[i].description != NULL) {
            sprintf(qmon->poptxt + strlen(qmon->poptxt), "%11s - %s\n", CommandMap[i].command, CommandMap[i].description);
        }
    }
    PopupMessage(qmon, qmon->poptxt);
}

void PopupHelp(QMON *qmon, int argc, char **argv)
{
static char *HaveDiskLoopsHelpMessage =
"  ESC - refresh display                  \n"
"  TAB - toggle single/dual digitizer mode\n"
"  --> - shift display one digitizer right\n"
"  <-- - shift display one digitizer left \n"
"  isi - monitor ISI  disk loops          \n"
" nrts - monitor NRTS disk loops          \n"
" q330 - list Q330 commands               \n"
"    q - quit                             \n";
static char *DoNotHaveDiskLoopsHelpMessage =
"  ESC - refresh display                  \n"
"  TAB - toggle single/dual digitizer mode\n"
"  --> - shift display one digitizer right\n"
"  <-- - shift display one digitizer left \n"
" q330 - list Q330 commands               \n"
"    q - quit                             \n";
char *text;

    text = qmon->mon != NULL ? HaveDiskLoopsHelpMessage : DoNotHaveDiskLoopsHelpMessage;
    if (argc <= 1) PopupMessage(qmon, text);
    if (argc == 2) {
        if (strcasecmp(argv[1], "q330") == 0) {
            PopupQ330Help(qmon);
        } else {
            PopupMessage(qmon, text);
        }
    }
}

void PopupUnrecognized(QMON *qmon, char *command)
{
#define MAXCMDLEN 32

    memset(qmon->poptxt, 0, QMON_DIM);
    strncpy(qmon->poptxt, command, MAXCMDLEN);
    strcat(qmon->poptxt, " unrecognized");
    PopupMessage(qmon, qmon->poptxt);
}

void PopupRegerr(QMON *qmon, int errcode)
{
#define MAXCMDLEN 32

    switch (errcode) {
      case QDP_ERR_BUSY: PopupMessage(qmon, QMON_MSG_BUSY); break;
      default:
        sprintf(qmon->poptxt, "registration error: %s", qdpErrcodeString(errcode));
        PopupMessage(qmon, qmon->poptxt);
    }
}

void ClearPopup(void)
{
    if (pop.win != NULL) {
        wclear(pop.win);
        wrefresh(pop.win);
        delwin(pop.win);
        pop.win = NULL;
    }
}

BOOL ActivePopup(void)
{
    if (pop.win == NULL) return FALSE;
    if (time(NULL) - pop.tstamp > POP_DUR) ClearPopup();

    return pop.win == NULL ? FALSE : TRUE;
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
 * $Log: pop.c,v $
 * Revision 1.8  2015/12/23 22:49:43  dechavez
 * don't include "nrts" and "isi" commands in the help message if there are
 * no disk loops available to display
 *
 * Revision 1.7  2011/07/27 23:46:16  dechavez
 * added auto-clear popup feature, added isi and nrts commands to help message
 *
 * Revision 1.6  2011/01/07 23:00:49  dechavez
 * added PopupQ330Help(), changed PopupHelp() to accept args ("q330" only one supported)
 *
 * Revision 1.5  2011/01/03 21:31:43  dechavez
 * removed 'help' line from help message (to make room for 'input')
 *
 * Revision 1.4  2010/12/27 22:45:38  dechavez
 * zero token (CR) command now just clears any pop ups (no more selection shift)
 *
 * Revision 1.3  2010/12/13 22:42:18  dechavez
 * center in the middle of the screen instead of intentionally up one extra row
 * removed PopupBadCount()
 * added PopupRegerr()
 *
 * Revision 1.2  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.1  2010/12/08 19:15:50  dechavez
 * created
 *
 */
