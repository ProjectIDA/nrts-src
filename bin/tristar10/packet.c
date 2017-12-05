#pragma ident "$Id: packet.c,v 1.4 2015/12/09 18:44:43 dechavez Exp $"
/*======================================================================
 *
 *  proto-packet functions
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "tristar10.h"

#define MY_MOD_ID TRISTAR10_MOD_PACKET

LNKLST *head;
static ISI_PUSH *ph = NULL;

#define MAXSAMP (IDA10_DEFDATALEN / sizeof(INT32))
#define MAXBYTE (MAXSAMP * sizeof(INT32))

#define NsecToSec(nsec) ((REAL64) nsec / NANOSEC_PER_SEC)

static void ResetPacket(PROTO_PACKET *pkt)
{
    pkt->nsamp = 0;
    utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
    memset(&pkt->buf[pkt->offset.data], 0xee, MAXBYTE);
}

void FlushPacket(PROTO_PACKET *pkt)
{
IDA10_TS ts;

    if (pkt->nsamp > 0) {
        utilPackINT16(&pkt->buf[pkt->offset.nsamp], pkt->nsamp);
        isiPushRawPacket(ph, pkt->buf, IDA10_FIXEDRECLEN, ISI_TYPE_IDA10);
        ResetPacket(pkt);
    }
    if (DebugEnabled()) {
        ida10UnpackTS(pkt->buf, &ts);
        LogMsg("%s", ida10TStoString(&ts, NULL));
    }
}

void FlushAllPackets(void)
{
LNKLST_NODE *crnt;

    LogMsg("flushing all packets to disk loop server");
    crnt = listFirstNode(head);
    while (crnt != NULL) {
        FlushPacket((PROTO_PACKET *) crnt->payload);
        crnt = listNextNode(crnt);
    }
}

static BOOL ExpectedTstamp(PROTO_PACKET *pkt, IDA10_GENTAG *ttag)
{
INT64 timetear;

    if ((timetear = (pkt->nsamp > 0) ? ttag->tstamp - pkt->tons : 0) != 0) {
        LogMsg("WARNING: %s time tear of %.3lf sec\n", pkt->ident, NsecToSec(timetear));
        return FALSE;
    }

    return TRUE;
}

static PROTO_PACKET *LocateProtoPacket(LNKLST *head, char *chnloc)
{
LNKLST_NODE *crnt;
PROTO_PACKET *pkt, *unreached;
static char *fid = "AppendSampleToPacket:LocateProtoPacket";

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        pkt = (PROTO_PACKET *) crnt->payload;
        if (strcmp(chnloc, pkt->ident) == 0) return pkt;
        crnt = listNextNode(crnt);
    }

    LogMsg("%s: FATAL ERROR: can't locate `%s' packet!!!\n", fid, chnloc);
    Exit(MY_MOD_ID + 1);
    return unreached; /* to calm some compilers */
}

static void AppendSampleToPacket(IDA10_GENTAG *ttag, READING *reading, UINT64 nsint)
{
PROTO_PACKET *pkt;

    if (!reading->live) return; /* ignore any dead channels */

    pkt = LocateProtoPacket(head, reading->chnloc);
    if (!ExpectedTstamp(pkt, ttag)) FlushPacket(pkt);

    if (!reading->fresh) {
        if (++reading->missed > MAX_MISSED) {
            LogMsg("channel `%s' declared dead (too many consecutive missed readings)\n", reading->chnloc);
            reading->live = FALSE;
            FlushPacket(pkt);
            return;
        }
        LogMsg("missed `%s' reading, re-using previous value\n", reading->chnloc);
    }

    if (pkt->nsamp == 0) ida10PackGenericTtag(&pkt->buf[pkt->offset.tofs], ttag);

    utilPackINT32(&pkt->buf[pkt->offset.data + (pkt->nsamp * sizeof(INT32))], reading->value);
    pkt->tons = ttag->tstamp + nsint;
    if (++pkt->nsamp == MAXSAMP) FlushPacket(pkt);

    reading->fresh = FALSE;
}

void StuffScan(IDA10_GENTAG *ttag, SCAN *scan, UINT64 nsint)
{
    LogScan(scan);

    AppendSampleToPacket(ttag, &scan->aerrs, nsint);

    AppendSampleToPacket(ttag, &scan->aes01, nsint);
    AppendSampleToPacket(ttag, &scan->aea01, nsint);
    AppendSampleToPacket(ttag, &scan->aef01, nsint);
    AppendSampleToPacket(ttag, &scan->aev01, nsint);
    AppendSampleToPacket(ttag, &scan->aec01, nsint);
    AppendSampleToPacket(ttag, &scan->ae101, nsint);
    AppendSampleToPacket(ttag, &scan->ae201, nsint);

    AppendSampleToPacket(ttag, &scan->aes02, nsint);
    AppendSampleToPacket(ttag, &scan->aea02, nsint);
    AppendSampleToPacket(ttag, &scan->aef02, nsint);
    AppendSampleToPacket(ttag, &scan->aev02, nsint);
    AppendSampleToPacket(ttag, &scan->aec02, nsint);
}

static void InitProtoPacket(PROTO_PACKET *new, char *chnloc, UINT8 gain, char *sname, char *nname, UINT64 nsint)
{
INT16 srfact, srmult;
UINT8 *ptr, descriptor;
IDA10_GENTAG dummy;

    memset(&dummy, 0, sizeof(IDA10_GENTAG));

    strcpy(new->ident, chnloc);

    memset(new->buf, 0, IDA10_FIXEDRECLEN);
    ptr = new->buf;

    /* 10.8 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 8;  /* subformat 8 */
    ptr += utilPackBytes(ptr, (UINT8 *) sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) nname, IDA105_NNAME_LEN);
    new->offset.tofs = (int) (ptr - new->buf);
    ptr += ida10PackGenericTtag(ptr, &dummy);
    ptr += utilPackUINT32(ptr, 0); /* seqno will get updated when written to disk */
    ptr += utilPackUINT32(ptr, 0); /* timestamp will get updated when written to disk */
    ptr += IDA108_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* use chnloc for stream name */

    ptr += utilPackBytes(ptr, (UINT8 *) chnloc, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = gain;

    /* number of samples will get updated with total when flushed */

    new->offset.nsamp = (int) (ptr - new->buf);
    ptr += utilPackINT16(ptr, new->nsamp = 0);

    /* sample rate */

    ida10SintToFactMult(((REAL64) nsint / NANOSEC_PER_SEC), &srfact, &srmult);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    /* the first sample */

    new->offset.data = (int) (ptr - new->buf);
    
    ResetPacket(new);
}

static void CreatePacket(char *chnloc, UINT8 gain, char *sname, char *nname, UINT64 nsint)
{
PROTO_PACKET *new;
static char *fid = "StartPacketPusher:InitPacketList:CreatePacket";

    if ((new = (PROTO_PACKET *) malloc(sizeof(PROTO_PACKET))) == NULL) {
        LogMsg("%s: malloc: %s", strerror(errno));
        Exit(MY_MOD_ID + 2);
    }

    InitProtoPacket(new, chnloc, gain, sname, nname, nsint);

    if (!listAppend(head, new, sizeof(PROTO_PACKET))) {
        LogMsg("%s: listAppend: %s", strerror(errno));
        Exit(MY_MOD_ID + 3);
    }
}

static void InitPacketList(char *sname, char *nname, UINT64 nsint)
{
static char *fid = "StartPacketPusher:InitPacketList";

    if ((head = listCreate()) == NULL) {
        LogMsg("%s: listCreate: %s", strerror(errno));
        Exit(MY_MOD_ID + 4);
    }

/* Here we hard-code the list of channels we want to acquire.  The associated
 * meta-knowledge about what those channels are is hard-coded in MoxaThread().
 * Both here and there will need to be changed if we want to acquire more
 * channels in the future.  It will also be necessary to adjust the queue depth
 * in isiPushInit(), too.  And be careful with CONVERSION_GAIN.  It needs to
 * get consistently applied both here and in MoxaThread().
 */

    CreatePacket(AERRS,        1       , sname, nname, nsint); /* reading validity flag */

    CreatePacket(AES01,        1       , sname, nname, nsint); /* charger control state */
    CreatePacket(AEA01,        1       , sname, nname, nsint); /* charger alarms */
    CreatePacket(AEF01,        1       , sname, nname, nsint); /* charger faults */
    CreatePacket(AEV01, CONVERSION_GAIN, sname, nname, nsint); /* battery voltage */
    CreatePacket(AEC01, CONVERSION_GAIN, sname, nname, nsint); /* charge current */
    CreatePacket(AE101, CONVERSION_GAIN, sname, nname, nsint); /* battery sense */
    CreatePacket(AE201, CONVERSION_GAIN, sname, nname, nsint); /* array voltage */

    CreatePacket(AES02,        1       , sname, nname, nsint); /* load controller state */
    CreatePacket(AEA02,        1       , sname, nname, nsint); /* load controller alarms */
    CreatePacket(AEF02,        1       , sname, nname, nsint); /* load controller faults */
    CreatePacket(AEV02, CONVERSION_GAIN, sname, nname, nsint); /* load voltage */
    CreatePacket(AEC02, CONVERSION_GAIN, sname, nname, nsint); /* load current */
}

void StartPacketPusher(SERVER *dl, LOGIO *lp, int depth, char *sname, char *nname, UINT64 nsint)
{
IACP_ATTR attr = IACP_DEFAULT_ATTR;

    if ((ph = isiPushInit(dl->server, dl->port, &attr, lp, LOG_INFO, IDA10_FIXEDRECLEN, depth, FALSE)) == NULL) {
        LogMsg("ERROR: isiPushInit: %s", strerror(errno));
        Exit(MY_MOD_ID + 5);
    }

    InitPacketList(sname, nname, nsint);
}

/* Revision History
 *
 * $Log: packet.c,v $
 * Revision 1.4  2015/12/09 18:44:43  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.3  2012/05/02 18:30:49  dechavez
 * *** initial production release ***
 *
 * Revision 1.2  2012/04/26 17:35:52  dechavez
 * split sample into slave1 and slave2, and added aex channel
 *
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
