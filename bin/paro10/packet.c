#pragma ident "$Id: packet.c,v 1.5 2015/12/07 19:13:00 dechavez Exp $"
/*======================================================================
 *
 *  Build IDA10 packets and write to disk loop.
 *
 *====================================================================*/
#include "paro10.h"

#define MY_MOD_ID PARO10_MOD_PACKET

/* Current time stamp (to the closest second) from host clock */

UINT64 CurrentSecond()
{
#define EpochConversionFactor (SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC)
UINT64 tstamp;

    tstamp = utilCurrentSecond() - EpochConversionFactor;
    return tstamp;
}

/* Fill in the IDA10.8 header */

static void StartNewPacket(BAROMETER *bp, UINT64 now)
{
#define STRBUFLEN 256
char strbuf[STRBUFLEN];
UINT8 *ptr, descriptor;
static char *fid = "StartNewPacket";

    bp->clock.tstamp = bp->tols = now;

    if (BarometerDebugEnabled()) LogMsg(LOG_INFO, "%s: tofs = %s", fid, utilTimeString(bp->clock.tstamp, 1000, 0, 0));
    ptr = bp->pkt.payload;
    memset(ptr, 0xee, IDA10_FIXEDRECLEN);

    /* 10.8 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 8;  /* subformat 8 */
    ptr += utilPackBytes(ptr, (UINT8 *) bp->sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) bp->nname, IDA105_NNAME_LEN);
    ptr += ida10PackGenericTtag(ptr, &bp->clock);
    ptr += utilPackUINT32(ptr, 0); /* seqno will get updated when written to disk */
    ptr += utilPackUINT32(ptr, 0); /* timestamp will get updated when written to disk */
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* stream name, a concatenation of channel and location codes */

    ptr += utilPackBytes(ptr, (UINT8 *) bp->cname, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    bp->pnsamp = ptr;
    ptr += utilPackUINT16(ptr, 0); /* will get updated when flushed */

    /* sample rate */

    ptr += utilPackINT16(ptr, bp->srfact);
    ptr += utilPackINT16(ptr, bp->srmult);
}

static void StuffSample(BAROMETER *bp, UINT64 sint, INT32 value, UINT64 now)
{
int offset;
#define STRBUFLEN 256
char strbuf[STRBUFLEN];

/* Ignore missed readings */

    if (value == PARO10_NO_SENSOR || value == PARO10_MISSED_SAMPLE) return;

/* Reset packet if starting a new one */

    if (bp->nsamp == 0) StartNewPacket(bp, now);

/* Insert the sample into the appropriate spot */

    offset = IDA10_FIXEDHDRLEN + (bp->nsamp*sizeof(INT32));
    utilPackINT32(bp->pkt.payload + offset, value);

    if (BarometerDebugEnabled()) {
        if (value != PARO10_MISSED_SAMPLE) {
            LogMsg(LOG_INFO, "append %s value=%ld, nsamp = %d/%d", bp->cname, value, bp->nsamp+1, bp->maxsamp);
        } else {
            LogMsg(LOG_INFO, "append %s value=**MISSED**, nsamp = %d/%d", bp->cname, value, bp->nsamp+1, bp->maxsamp);
        }
    }

    bp->tols = now;
    if (++bp->nsamp == bp->maxsamp) {
        if (bp->first) {
            LogMsg(LOG_INFO, "initial %s packet completed", bp->cname);
            bp->first = FALSE;
        }
        FlushPacket(bp);
    }
}

static void PrintMissedSamplesWarning(BAROMETER *bp, UINT64 now, UINT32 ElapsedMsec, INT32 missed)
{
char nowstr[1024], timstr[1024];

    if (missed == 0 || bp->nsamp == 0) return;

    utilTimeString(now, 1000, nowstr, 1024);
    utilTimeString(bp->clock.tstamp, 1000, timstr, 1024);
    if (missed <  0) {
        LogMsg(LOG_INFO, "WTF? Did time just go backwards? tstamp=%s now=%s\n", nowstr, timstr);
        return;
    }

    if (missed == 1) {
        LogMsg(LOG_INFO, "WARNING: missed %d %s sample! now=%s, tstamp=%s", missed, bp->cname, nowstr, timstr);
    } else {
        LogMsg(LOG_INFO, "WARNING: missed %d %s samples! now=%s, tstamp=%s", missed, bp->cname, nowstr, timstr);
    }
}

static void PrintBackwardsClockWarning(UINT64 now, UINT64 tols)
{
char nowstr[1024], tolstr[1024];

    utilTimeString(now, 1000, nowstr, 1024);
    utilTimeString(tols, 1000, tolstr, 1024);

    LogMsg(LOG_INFO, "WARNING: clock jumped backwards? now=%s, tols=%s", nowstr, tolstr);;
}

THREAD_FUNC PacketThread(void *argptr)
{
BAROMETER *bp;
BOOL ready;
INT32 value, missed, i;
UINT64 now, NanoSint;
UINT32 ElapsedMsec, PollInterval;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "building %d sample IDA10.8 packets, sample interval = %lu msec", bp->maxsamp, bp->MsecSint);

    PollInterval = (bp->MsecSint / 4);
    NanoSint = bp->MsecSint * NANOSEC_PER_MSEC;

/* Loop forever, reading and saving readings */

    bp->clock.tstamp = CurrentSecond();
    bp->clock.status.receiver = 0;
    bp->clock.status.generic = IDA10_GENTAG_LOCKED | IDA10_GENTAG_DERIVED;

    bp->tols = bp->clock.tstamp;

    while (1) {
        now = CurrentSecond();
        if (now < bp->tols) {
            PrintBackwardsClockWarning(now, bp->tols);
        } else {
            ElapsedMsec = (UINT32) ((now - bp->tols) / NANOSEC_PER_MSEC);
            if (ElapsedMsec >= bp->MsecSint) {
                if ((missed = (ElapsedMsec / bp->MsecSint) - 1) > 0) {
                    PrintMissedSamplesWarning(bp, now, ElapsedMsec, missed);
                    FlushPacket(bp);
                }
                value = GetSample(bp);
                StuffSample(bp, NanoSint, value, now);
            }
        }
        utilDelayMsec(PollInterval);
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
 * $Log: packet.c,v $
 * Revision 1.5  2015/12/07 19:13:00  dechavez
 * changes needed for clean Mac OS X builds
 *
 * Revision 1.4  2012/12/14 19:08:32  dechavez
 * Get time stamp to the closest second, to addres the problem of tiny time tears across packet boundaries
 *
 * Revision 1.3  2012/10/31 17:25:02  dechavez
 * Major rework of timestamp logic, to accomodate host clock going backwards.
 * Also, istead of inserting dummy values when samples are missed, instead
 * a partial packet is flushed.  Ie, introduce gaps when samples are missed.
 * Lots of changes, some new bugs may have been introduced...
 *
 * Revision 1.2  2012/10/25 22:15:38  dechavez
 * initial release
 *
 */
