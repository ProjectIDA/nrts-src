#pragma ident "$Id: resync.c,v 1.1 2010/12/13 22:40:02 dechavez Exp $"
/*======================================================================
 *
 *  Resync Q330
 *
 *====================================================================*/
#include "qmon.h"

void ExecuteResync(QMON *qmon, int argc, char **argv)
{
int port;

    if (argc != 1) {
        PopupMessage(qmon, "usage: resync");
        return;
    }

    if (!qdpCtrl(qmon->qdp, QDP_CTRL_RESYNC, TRUE)) PopupMessage(qmon, "ERROR: resync command failed");
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
 * $Log: resync.c,v $
 * Revision 1.1  2010/12/13 22:40:02  dechavez
 * created
 *
 */
