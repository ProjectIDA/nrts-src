#pragma ident "$Id: util.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Various useful funcionts
 *
 *====================================================================*/
#include "wgmcd.h"

/* Determine the next sequence number for this site by reading the most recent
 * packet in the disk loop (if any) and incrementing.
 */

BOOL GetNextSeqno(ISI_DL *dl, WG_OBSPKT_SEQNO *seqno)
{
struct {
    ISI_DL dl;
    ISI_DL_SYS sys;
} snapshot;
ISI_RAW_PACKET *raw;
WG_WGOBS1 wgobs1;
static char *fid = "GetNextSeqno";

    if (!isidlSnapshot(dl, &snapshot.dl, &snapshot.sys)) {
        LogMsg(LOG_ERR, "%s: isidlSnapshot: %s", fid, strerror(errno));
        return FALSE;
    }

    if (!isiIsValidIndex(snapshot.sys.index.yngest)) {
        seqno->valid = 0;
        return TRUE;
    }

    if ((raw = isiAllocateRawPacket(snapshot.sys.maxlen)) == NULL) {
        LogMsg(LOG_ERR, "%s: isiAllocateRawPacket(%d): %s", fid, snapshot.sys.maxlen, strerror(errno));
        return FALSE;
    }

    if (!isidlReadDiskLoop(dl, raw, snapshot.sys.index.yngest)) {
        LogMsg(LOG_ERR, "%s: isidlReadDiskLoop: %s", fid, strerror(errno));
        return FALSE;
    }

    if (raw->hdr.desc.type != ISI_TYPE_WGOBS1) {
        LogMsg(LOG_ERR, "%s: unexpected packet type '%d'!", fid, raw->hdr.desc.type);
        return FALSE;
    }

    wgUnpackWGOBS1(raw->payload, &wgobs1);
    seqno->valid = 1;
    seqno->value = wgobs1.seqno + 1;

    isiDestroyRawPacket(raw);

    return TRUE;
}

/* Revision History
 *
 * $Log: util.c,v $
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
