#pragma ident "$Id: push.c,v 1.4 2014/08/28 21:43:12 dechavez Exp $"
/*======================================================================
 *
 *  Push completed packets to remote disk loop
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "paro10.h"

#define MY_MOD_ID PARO10_MOD_PUSH

static ISI_PUSH *ph = NULL;
static MUTEX mutex;

static void PrintPartialPacketMessage(BAROMETER *bp)
{
char tofstr[1024], tolstr[1024];

    utilTimeString(bp->clock.tstamp, 1000, tofstr, 1024);
    utilTimeString(bp->tols, 1000, tolstr, 1024);

    LogMsg(LOG_INFO, "WARNING: partial %s packet flushed: tofs=%s tols=%s nsamp=%d", bp->cname, tofstr, tolstr, bp->nsamp);

}

void FlushPacket(BAROMETER *bp)
{
static BOOL first = TRUE;
static char *fid = "FlushPacket";

    if (ph == NULL) {
        LogMsg(LOG_INFO, "WTF? ******** %s called with NULL ph ********", fid);
        return;
    }

    if (bp->nsamp == 0) return;
    if (bp->nsamp < bp->maxsamp) PrintPartialPacketMessage(bp);

    MUTEX_LOCK(&mutex);
        BlockShutdown(fid);
            utilPackUINT16(bp->pnsamp, bp->nsamp);
            isiPushRawPacket(ph, bp->pkt.payload, bp->pkt.hdr.len.used, ISI_TYPE_IDA10);
            bp->nsamp = 0;
        UnblockShutdown(fid);
    MUTEX_UNLOCK(&mutex);

    if (first) {
        first = FALSE;
        LogMsg(LOG_INFO, "initial packet delivered to ISI push server@%s:%%d\n", ph->server, ph->port);
    }
}

void StartPacketPusher(char *server, int port, LOGIO *lp, int depth, char *sname, char *nname)
{
IACP_ATTR attr = IACP_DEFAULT_ATTR;
static char *fid = "StartPacketPusher";

    MUTEX_INIT(&mutex);

    if ((ph = isiPushInit(server, port, &attr, lp, LOG_INFO, IDA10_FIXEDRECLEN, depth, FALSE)) == NULL) {
        LogMsg(LOG_INFO, "ERROR: %s:isiPushInit: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 5);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * $Log: push.c,v $
 * Revision 1.4  2014/08/28 21:43:12  dechavez
 * print a message the first time a packet is delivered to the server
 *
 * Revision 1.3  2012/10/31 17:22:17  dechavez
 * log partial packets, reset nsamp after flushing packet
 *
 * Revision 1.2  2012/10/25 22:15:38  dechavez
 * initial release
 *
 */
