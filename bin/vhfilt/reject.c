#pragma ident "$Id: reject.c,v 1.2 2015/10/30 22:28:08 dechavez Exp $"
/*======================================================================
 *
 * Keep track of streams that we are rejecting.
 *
 *====================================================================*/
#include "vhfilt.h"

#define MY_MOD_ID VHFILT_MOD_REJECT

void RejectUnsupportedFormat(MSEED_RECORD *record)
{
char *ident;
LNKLST_NODE *crnt;
static LNKLST *head = NULL;
static char *fid = "ReadInput:RejectUnsupportedFormat";

    if (head == NULL && (head = listCreate()) == NULL) {
        LogErr("ERROR: %s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        ident = (char *) crnt->payload;
        if (strcmp(ident, record->hdr.ident) == 0) return;
    }

    if (!listAppend(head, (void *) record->hdr.ident, strlen(record->hdr.ident)+1)) {
        LogErr("ERROR: %s: listAppend: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    LogMsg(0, "Rejecting %s streams (unsupported data type %s)\n", record->hdr.ident, mseedDataFormatString(record->hdr.format));
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: reject.c,v $
 * Revision 1.2  2015/10/30 22:28:08  dechavez
 * initial production release
 *
 */
