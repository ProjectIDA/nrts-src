#pragma ident "$Id: flush.c,v 1.3 2015/12/07 19:27:49 dechavez Exp $"
/*======================================================================
 *
 *  Save current programming
 *
 *====================================================================*/
#include "qmon.h"

static void HelpMesg(QMON *qmon, int argc, char **argv)
{
static char *text = "usage: flush { 1 | 2 | 3 | 4 }\n";

    PopupMessage(qmon, text);
}

void ExecuteFlush(QMON *qmon, int argc, char **argv)
{
int port;

    if (argc != 2) {
        HelpMesg(qmon, argc, argv);
        return;
    }

    if ((port = qdpDataPortNumber(atoi(argv[1]))) == QDP_UNDEFINED_PORT) {
        HelpMesg(qmon, argc, argv);
        return;
    }

    if (!qdpFlushDataPort(qmon->qdp, port, TRUE)) PopupMessage(qmon, "qdpFlushDataPort failed");
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
 * $Log: flush.c,v $
 * Revision 1.3  2015/12/07 19:27:49  dechavez
 * replaced TABs with spaces
 *
 * Revision 1.2  2010/12/23 22:01:12  dechavez
 * use qdpDataPortNumber() to get data port value
 *
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
