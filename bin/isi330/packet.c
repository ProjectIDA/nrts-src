#pragma ident "$Id: packet.c,v 1.2 2018/01/11 23:29:27 dauerbach Exp $"
/*======================================================================
 *
 *  proto-packet functions
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_PACKET

static ISI_PUSH *ph = NULL;

void FlushRecord(UINT8 *rawmseed)
{
    static char *fid = "FlushRecord";
    UINT8 ida1012[IDA10_FIXEDRECLEN];
    static BOOL FirstRecord = TRUE;

    if (rawmseed == NULL) return;

    if (mseed512ToIDA1012(rawmseed, ida1012, NULL, NULL) == NULL) {
        LogMsg("ERROR: %s: mseed512ToIDA1012: %s", fid, strerror(errno));
        return;
    }

    if (!isiPushRawPacket(ph, ida1012, IDA10_FIXEDRECLEN, ISI_TYPE_IDA10)) {
        LogMsg("ERROR: %s: isiPushRawPacket: %s", strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
    if (FirstRecord) {
        LogMsg("initial packet enqueued to ISI push server@%s:%d\n", ph->server, ph->port);
        FirstRecord = FALSE;
    }

    return;
}

void StartRecordPusher(char *server, int port, LOGIO *lp, int depth, char *sname, char *nname)
{
IACP_ATTR attr = IACP_DEFAULT_ATTR;
static char *fid = "StartRecordPusher";

    if ((ph = isiPushInit(server, port, &attr, lp, LOG_INFO, IDA10_FIXEDRECLEN, depth, FALSE)) == NULL) {
        LogMsg("ERROR: %s:isiPushInit: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
