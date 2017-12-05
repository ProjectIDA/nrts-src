#pragma ident "$Id: process.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 *
 * Build and manage IDA10 packets
 *
 *====================================================================*/
#include "siomet.h"

#define MY_MOD_ID SIOMET_MOD_PACKET

static BOOL Debug = FALSE;
static PROTO_PACKET Ta, Ua, Pa;
#define NsecToSec(nsec) ((REAL64) nsec / NANOSEC_PER_SEC)

void ToggleProcessThreadVerbosity(void)
{
    Debug = !Debug;
    LogMsg("scan logging %s", Debug ? "enabled" : "disabled");
}

static void InitProtoPacket(PROTO_PACKET *pkt, char *sname, char *nname, char *cname, UINT64 nsint)
{
    strcpy(pkt->sname, sname);
    strcpy(pkt->nname, nname);
    strcpy(pkt->cname, cname);
    pkt->nsint = nsint;
    pkt->nsamp = 0;
}

void InitPacketBuilder(char *sname, char *nname, SIOMET_INFO *info)
{
    InitProtoPacket(&Ta, sname, nname, info->ta, info->nsint);
    InitProtoPacket(&Ua, sname, nname, info->ua, info->nsint);
    InitProtoPacket(&Pa, sname, nname, info->pa, info->nsint);
}

static void AppendSample(PROTO_PACKET *pkt, INT32 value, UINT64 tstamp)
{
INT64 tear;

    /* sanity check on the time stamp */

    if (pkt->nsamp > 0) {
        if ((tear = (tstamp - pkt->tons)) != 0) {
            LogMsg("WARNING: %s:%s time tear of %.3lf sec\n", pkt->sname, pkt->cname, NsecToSec(tear));
            FlushPacket(pkt);
        }
    } else {
        pkt->tofs = tstamp;
        pkt->tons = pkt->tofs;
    }

    pkt->data[pkt->nsamp++] = value;
    pkt->tons += pkt->nsint;

    if (pkt->nsamp == MAXSAMP) FlushPacket(pkt);
}

void ProcessScan(SIOMET_SCAN *scan, SIOMET_INFO *info, UINT64 tstamp)
{
UINT32 value;

    if (Debug) LogMsg("ProcessScan: T=%.2fC H=%.1f%% P=%.4fbar\n", scan->Ta, scan->Ua, scan->Pa);
    AppendSample(&Ta, scan->iTa, tstamp);
    AppendSample(&Ua, scan->iUa, tstamp);
    AppendSample(&Pa, scan->iPa, tstamp);
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

/* Revision History
 *
 * $Log: process.c,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
