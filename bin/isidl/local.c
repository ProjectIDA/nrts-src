#pragma ident "$Id: local.c,v 1.16 2016/02/12 21:56:27 dechavez Exp $"
/*======================================================================
 *
 *  Process locally acquired data.  Update headers as appropriate
 *  and write to disk loop.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_LOCAL

void ProcessLocalData(LOCALPKT *local)
{
BOOL ok;
static char *fid = "ProcessLocalData";

    local->raw.hdr.status = ISI_RAW_STATUS_OK;

    MUTEX_LOCK(&local->mutex);

        switch (local->raw.hdr.desc.type) {
          case ISI_TYPE_QDPLUS:
            CompleteQdplusHeader(&local->raw, local->dl);
            break;
        }

    /* Write to the disk loop */

        ok = WritePacketToDisk(local->dl, &local->raw);

    MUTEX_UNLOCK(&local->mutex);

    if (!ok) {
        LogMsg(LOG_INFO, "%s: WritePacketToDisk failed", fid);
        SetExitStatus(MY_MOD_ID + 2);
    }
}

static BOOL InitLocalPacket(LOCALPKT *local, int len, int comp, int type, ISI_DL *dl)
{
void *ida;
static char *fid = "InitLocalPacket";

    local->dl = dl;

    MUTEX_INIT(&local->mutex);

    if (!isiInitRawPacket(&local->raw, NULL, len)) return FALSE;

    local->raw.hdr.len.used   = len;
    local->raw.hdr.len.native = len;
    local->raw.hdr.desc.comp  = comp;
    local->raw.hdr.desc.type  = type;
    local->raw.hdr.desc.order = ISI_ORDER_UNDEF;
    local->raw.hdr.desc.size  = sizeof(UINT8);
    strlcpy(local->raw.hdr.site, local->dl->sys->site, ISI_SITELEN+1);

    return TRUE;
}

BOOL InitLocalProcessor(ISIDL_PAR *par)
{
Q330 *q330;
int i, len, comp, type;
static char *fid = "InitLocalProcessor";

    if (par->source != SOURCE_Q330) return TRUE;

/* initialize the raw packets */

    len  = QDPLUS_PKTLEN;
    comp = ISI_COMP_NONE;
    type = ISI_TYPE_QDPLUS;

    for (i = 0; i < par->q330.count; i++) {
        q330 = (Q330 *) par->q330.array[i];
        if (!InitLocalPacket(&q330->local, len, comp, type, par->dl[0])) {
            fprintf(stderr, "%s: InitLocalPacket: %s", fid, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: local.c,v $
 * Revision 1.16  2016/02/12 21:56:27  dechavez
 * removed old code related to barometer, ars, and pre-IDA10 packet support
 *
 * Revision 1.15  2015/12/09 18:34:54  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.14  2014/08/28 22:05:41  dechavez
 * ProcessLocalData() greatly simplified, now used WritePacketToDisk() for
 * output, and only QDPLUS packets get any extra processing.
 *
 * Revision 1.13  2011/12/15 18:36:19  dechavez
 * Fixed bug with setting offset to tstamp in packet header in ProcessLocalData()
 *
 * Revision 1.12  2011/11/07 17:48:30  dechavez
 * accomodated move of mseed to ISI_DL_OPTIONS
 *
 * Revision 1.11  2011/11/03 18:02:52  dechavez
 * Populate ISI_RAW_HEADER mseed field (not tested)
 *
 * Revision 1.10  2011/10/19 16:21:07  dechavez
 * changed static 91011bug to variable daybug to accomodate new behavior in these crazy clocks
 *
 * Revision 1.9  2011/10/14 17:23:41  dechavez
 * added bug91011 argument to CompleteIda9Header() call
 *
 * Revision 1.8  2011/10/13 18:08:55  dechavez
 * 9/10/11 bug support, use new ISI_DL_OPTIONS for isidlWriteToDiskLoop()
 *
 * Revision 1.7  2010/04/01 21:51:40  dechavez
 * replaced all Exit() calls to either set exit status and continue, or set status and hold
 *
 * Revision 1.6  2007/05/03 20:27:16  dechavez
 * added liss support
 *
 * Revision 1.5  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.4  2007/02/08 22:53:51  dechavez
 * LOG_ERR to LOG_INFO, use LOCALPKT handle
 *
 * Revision 1.3  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.2  2006/07/10 21:09:48  dechavez
 * complete headers for IDA5 and IDA6 packets
 *
 * Revision 1.1  2006/06/30 18:20:47  dechavez
 * initial release
 *
 */
