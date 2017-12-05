#pragma ident "$Id: pkt.c,v 1.1 2016/04/28 23:01:50 dechavez Exp $"
/*======================================================================
 *
 *  Packet builder facility.
 *
 *====================================================================*/
#include "spm10.h"

#define MY_MOD_ID SPM10_MOD_PKT

static LNKLST *head = NULL;

#define MAXSAMP (IDA10_DEFDATALEN / sizeof(INT32))
#define MAXBYTE (MAXSAMP * sizeof(INT32))

#define NsecToSec(nsec) ((REAL64) nsec / NANOSEC_PER_SEC)

void ResetPacket(PROTO_PACKET *pkt)
{
    pkt->nsamp = 0;
    utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
    memset(&pkt->buf[pkt->offset.data], 0xee, MAXBYTE);
}

static BOOL ConsistentPacket(PROTO_PACKET *pkt, LINE_SAMPLE *sample)
{
INT64 timetear;

    if (pkt->sint != sample->sint) {
        LogMsg(LOG_INFO, "WARNING: %s sample interval change from ", pkt->ident);
        LogMsg(LOG_INFO, "%.3lf to %.3lf\n", NsecToSec(pkt->sint), NsecToSec(sample->sint));
        return FALSE;
    }

    if ((timetear = (pkt->nsamp > 0) ? sample->tstamp.gentag.tstamp - pkt->tons : 0) != 0) {
        LogMsg(LOG_INFO, "WARNING: %s time tear of %.3lf sec\n", pkt->ident, NsecToSec(timetear));
        return FALSE;
    }

    return TRUE;
}

static void InitProtoPacket(PROTO_PACKET *new, LINE_SAMPLE *sample)
{
INT16 srfact, srmult;
UINT8 *ptr, descriptor;

    strcpy(new->ident, sample->ident);
    new->sint = sample->sint;
    new->tons = sample->tstamp.gentag.tstamp;

    memset(new->buf, 0, IDA10_FIXEDRECLEN);
    ptr = new->buf;

    /* 10.8 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 8;  /* subformat 8 */
    ptr += utilPackBytes(ptr, (UINT8 *) sample->sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) sample->nname, IDA105_NNAME_LEN);
    new->offset.tofs = (int) (ptr - new->buf);
    ptr += ida10PackGenericTtag(ptr, &sample->tstamp.gentag);
    ptr += utilPackUINT32(ptr, 0); /* seqno will get updated when written to disk */
    ptr += utilPackUINT32(ptr, 0); /* timestamp will get updated when written to disk */
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* use chnloc for stream name */

    ptr += utilPackBytes(ptr, (UINT8 *) sample->chnloc, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = sample->gain;

    /* number of samples will get updated with total when flushed */

    new->offset.nsamp = (int) (ptr - new->buf);
    ptr += utilPackINT16(ptr, new->nsamp = 0);

    /* sample rate */

    ida10SintToFactMult(((REAL64) sample->sint / NANOSEC_PER_SEC), &srfact, &srmult);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    /* the first sample */

    new->offset.data = (int) (ptr - new->buf);

    ResetPacket(new);
}

static LNKLST_NODE *LocateProtoPacket(LINE_SAMPLE *sample)
{
LNKLST_NODE *crnt;
PROTO_PACKET*pkt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        pkt = (PROTO_PACKET *) crnt->payload;
        if (strcmp(sample->ident, pkt->ident) == 0) return crnt;
        crnt = listNextNode(crnt);
    }

    errno = ENOENT;
    return NULL;
}

static LNKLST_NODE *CreateProtoPacket(LINE_SAMPLE *sample)
{
PROTO_PACKET *new;

    if ((new = (PROTO_PACKET *) malloc(sizeof(PROTO_PACKET))) == NULL) {
        LogMsg(LOG_INFO, "CreateProtoPacket: malloc: %s", strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    InitProtoPacket(new, sample);

    return listAppend(head, new, sizeof(PROTO_PACKET)) ? LocateProtoPacket(sample) : NULL;
}

static LNKLST_NODE *GetProtoPacket(LINE_SAMPLE *sample)
{
LNKLST_NODE *node;
PROTO_PACKET*pkt;
static char *fid = "GetProtoPacket";

/* If we have a packet in progress, ensure time and sample rate are OK */

    if ((node = LocateProtoPacket(sample)) != NULL) {
        pkt = (PROTO_PACKET *) node->payload;
        if (!ConsistentPacket(pkt, sample)) {
            FlushPacket(pkt);
            node = NULL;
        }
    }

    if (node == NULL && (node = CreateProtoPacket(sample)) == NULL) {
        LogMsg(LOG_INFO, "ERROR: %s: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }
    return node;
}

void AppendSample(LINE_SAMPLE *sample)
{
LNKLST_NODE *node;
PROTO_PACKET *pkt;

    node = GetProtoPacket(sample);
    pkt = (PROTO_PACKET *) node->payload;

    if (pkt->nsamp == 0) ida10PackGenericTtag(&pkt->buf[pkt->offset.tofs], &sample->tstamp.gentag);
    utilPackINT32(&pkt->buf[pkt->offset.data + (pkt->nsamp * sizeof(INT32))], sample->value);
    pkt->tons = sample->tstamp.gentag.tstamp + sample->sint;
    if (++pkt->nsamp == MAXSAMP) FlushPacket(pkt);
}

void FlushAllPackets()
{
LNKLST_NODE *crnt;
PROTO_PACKET*pkt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        pkt = (PROTO_PACKET *) crnt->payload;
        FlushPacket(pkt);
        crnt = listNextNode(crnt);
    }
}

void InitPacketBuilder(void)
{
static char *fid = "InitPacketBuilder";

    if ((head = listCreate()) == NULL) {
        LogMsg(LOG_INFO, "ERROR: %s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: pkt.c,v $
 * Revision 1.1  2016/04/28 23:01:50  dechavez
 * initial release
 *
 */
