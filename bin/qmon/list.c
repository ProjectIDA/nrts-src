#pragma ident "$Id: list.c,v 1.4 2011/01/25 18:53:32 dechavez Exp $"
/*======================================================================
 *
 * Digitizer list window
 *
 *====================================================================*/
#include "qmon.h"

#define MAX_Q330_PER_LINE 8
#define MAX_Q330 16
#define TAG_INTERVAL (QMON_MAX_NAME_LEN + 1)

#define TAG_OFFSET 58
static char *tag = "Host ";

void UpdateHts(QMON *qmon)
{
    mvwprintw(qmon->win.lst, 0, TAG_OFFSET + strlen(tag), "%s", utilLttostr(time(NULL), 0, NULL));;
    wrefresh(qmon->win.lst);
}

void DrawDigitizerList(QMON *qmon)
{
DIGITIZER *digitizer;
LNKLST_NODE *crnt;
int row = 0, col = 0, count = 0;
static char *fid = "DrawDigitizerList";

    wmove(qmon->win.lst, row, col);

    crnt = listFirstNode(qmon->avail);
    while (crnt != NULL && count < MAX_Q330) {
        digitizer = (DIGITIZER *) crnt->payload;
        if (count == MAX_Q330_PER_LINE) ++row;
        col = (count % MAX_Q330_PER_LINE) * TAG_INTERVAL;
        wmove(qmon->win.lst, row, col);
        if (isActiveDigitizer(digitizer)) wattron(qmon->win.lst, A_BOLD);
        if (digitizer->flags) wattron(qmon->win.lst, A_STANDOUT);
        waddnstr(qmon->win.lst, digitizer->addr->connect.ident, QMON_MAX_NAME_LEN);
        wattroff(qmon->win.lst, A_BOLD);
        wattroff(qmon->win.lst, A_STANDOUT);
        ++count;
        crnt = listNextNode(crnt);
    }
    mvwaddstr(qmon->win.lst, 0, TAG_OFFSET, tag);
    UpdateHts(qmon);
    wrefresh(qmon->win.lst);
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
 * $Log: list.c,v $
 * Revision 1.4  2011/01/25 18:53:32  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.3  2010/12/13 22:45:34  dechavez
 * initial Q330 command support
 *
 * Revision 1.2  2010/10/19 20:56:11  dechavez
 * removed unneeded refresh() from UpdateHts()
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
