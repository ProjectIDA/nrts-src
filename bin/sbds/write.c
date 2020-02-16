#pragma ident "$Id: write.c,v 1.5 2015/07/17 18:55:56 dechavez Exp $"
/*======================================================================
 *
 *  Write incoming messages to an ISI disk loop
 *
 *====================================================================*/
#include "sbds.h"
#include "isi.h"
#include "isi/dl.h"
#include "isi/db.h"

#define MY_MOD_ID SBDS_MOD_DISKLOOP

static MUTEX mutex;
static ISI_DL *dl = NULL;
static ISI_RAW_PACKET *pkt = NULL;
static ISI_DL_OPTIONS options;

BOOL AppSuppliedWritePacketToDisk(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
BOOL ok;
static char *fid = "AppSuppliedWritePacketToDisk";

    BlockShutdown(fid);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_SEQNO)  dl->options.offset.seqno  = ida10SeqnoOffset(raw->payload);
        if (dl->options.flags & ISI_OPTION_INSERT_32BIT_TSTAMP) dl->options.offset.tstamp = ida10TstampOffset(raw->payload);
        ok = !isidlWriteToDiskLoop(dl, raw);
    UnblockShutdown(fid);

    if (!ok) {
        LogMsg(LOG_INFO, "%s: isidlWriteToDiskLoop failed: %s\n", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

BOOL SaveMessage(SBD_MESSAGE *message)
{
char tstamp[256];
BOOL ok;
char *site = NULL;
static char *fid = "SaveMessage";

    if (message == NULL) {
        LogMsg(LOG_INFO, "%s: *** ERROR *** NULL message received", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (message->mo.header.valid) {
        utilLttostr(message->mo.header.tstamp, 0, tstamp);
        LogMsg(LOG_INFO, "MO_HEADER: imei=%s cdr=0x%08x momsn=0x%04x status=%02d tstamp=%s",
            message->mo.header.imei,
            message->mo.header.cdr,
            message->mo.header.momsn,
            message->mo.header.status,
            tstamp
        );
    }

    if (message->mo.payload.valid) {
        LogMsg(LOG_INFO, "MO_PAYLOAD: %d bytes", message->mo.payload.len);
        HexDump(LOG_INFO, message->mo.payload.data, message->mo.payload.len);
    }

    if (message->mo.location.valid) {
        LogMsg(LOG_INFO, "MO_LOCATION: lat=%.4f lon=%.4f cep=%d km\n",
            message->mo.location.lat,
            message->mo.location.lon,
            message->mo.location.cep
        );
    }

    if (message->mo.confirm.valid) {
        LogMsg(LOG_INFO, "MO_CONFIRM: %s\n", message->mo.confirm.status ? "Success" : "FAILURE");
    }

    if (message->mt.header.valid) {
        LogMsg(LOG_INFO, "MT_HEADER: imei=%s myid=0x%08x flags=0x%04x",
            message->mt.header.imei,
            message->mt.header.myid,
            message->mt.header.flags
        );
    }

    if (message->mt.payload.valid) {
        LogMsg(LOG_INFO, "MT_PAYLOAD: %d bytes", message->mt.payload.len);
        HexDump(LOG_INFO, message->mt.payload.data, message->mt.payload.len);
    }

    if (message->mt.confirm.valid) {
        LogMsg(LOG_INFO, "MT_CONFIRM: imei=%s cdr=0x%08x myid=0x%08x status=%d",
            message->mt.confirm.imei,
            message->mt.confirm.cdr,
            message->mt.confirm.myid,
            message->mt.confirm.status
        );
    }

    if (message->mt.priority.valid) {
        LogMsg(LOG_INFO, "MO_PRIORITY: %d\n", message->mt.priority.level);
    }

    MUTEX_LOCK(&mutex);
        pkt->hdr.len.used = pkt->hdr.len.native = message->len;
        pkt->payload = message->body;
        ok = AppSuppliedWritePacketToDisk(dl, pkt);
    MUTEX_UNLOCK(&mutex);

    if (!ok) {
        LogMsg(LOG_INFO, "%s: AppSuppliedWritePacketToDisk: %s", fid, strerror(errno));
        return FALSE;
    }

/* MO payloads are candidates for further processing as ADDOSS packets */

    if (message->mo.header.valid && message->mo.payload.valid) ProcessADDOSS(message);
}

ISI_DL *InitSBDdl(PARAM *par)
{
static UINT32 options = ISI_OPTION_GENERATE_SEQNO;
static char *fid = "InitSBDdl";

    MUTEX_INIT(&mutex); /* to protect the single ISI_RAW_PACKET */

    if ((dl = isidlOpenDiskLoop(&par->glob, par->site, &par->lp, ISI_RDWR, options)) == NULL) {
        LogMsg(LOG_INFO, "%s: can't open `%s' disk loop", fid, par->site);
        GracefulExit(MY_MOD_ID + 1);
    }

    if ((pkt = isiAllocateRawPacket(dl->sys->maxlen)) == NULL) {
        LogMsg(LOG_INFO, "%s: isiAllocateRawPacket: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }
    strncpy(pkt->hdr.site, par->site, ISI_SITELEN);
    pkt->hdr.desc.comp  = ISI_COMP_NONE;
    pkt->hdr.desc.type  = ISI_TYPE_SBD1;
    pkt->hdr.desc.order = ISI_ORDER_UNDEF;
    pkt->hdr.desc.size  = sizeof(UINT8);
    pkt->hdr.status     = ISI_RAW_STATUS_OK;

    LogMsg(LOG_INFO, "ISI disk loop '%s' opened for writing", par->site);

    return dl;
}

/* Revision History
 *
 * $Log: write.c,v $
 * Revision 1.5  2015/07/17 18:55:56  dechavez
 * rework to use user supplied function for writing data, and move of options into ISI_DL handle
 *
 * Revision 1.4  2013/05/11 23:07:02  dechavez
 * added mutex to protect single buffer for disk loop input, plus various changes
 * to support the addition of ADDOSS/IDA10 support
 *
 * Revision 1.3  2013/03/15 17:49:56  dechavez
 * include checks, and log messages, for all possible SBD IEs
 *
 * Revision 1.2  2013/03/13 21:33:42  dechavez
 * print all newly supported (libsbd 1.2.0) IEs, accomodate new SBD_MESSAGE layout
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
