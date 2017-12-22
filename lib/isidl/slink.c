#pragma ident "$Id: slink.c,v 1.18 2017/12/20 23:56:58 dechavez Exp $"
/*======================================================================
 *
 * Tee incoming ISI data into an IRIS SeedLink server (ringserver).
 *
 * Uses Chad Trabant's libdali for stuffing MiniSeed records into the
 * backend of ringserver.
 *
 *====================================================================*/
#include "isi/dl.h"
#include "nrts/dl.h"
#include "mseed.h"
#include "dmc/libmseed.h"
#include "dmc/libdali.h"

static LOGIO *lp = NULL; /* this is wrong, but I'm not worrying about it */

static BOOL QueueIsEmpty(ISI_DL_SEEDLINK *slink)
{
BOOL retval;

    MUTEX_LOCK(&slink->queue.mutex);
        retval = slink->queue.empty;
    MUTEX_UNLOCK(&slink->queue.mutex);

    return retval;
}

/* This is the mseedPackRecord() callback function.  Since this is the only place
 * that will ever see the data, we are safe in putting just the pointer in the
 * message queue.
 */

static void EnqueueMiniSeedRecord(void *argptr, MSEED_PACKED *packed)
{
MSEED_HANDLE *handle;
ISI_DL_SEEDLINK *slink;
static char *fid = "EnqueueMiniSeedRecord";

    slink = (ISI_DL_SEEDLINK *) argptr;
    MUTEX_LOCK(&slink->queue.mutex);

        if (listCount(slink->queue.list) == slink->queue.depth) {
            if (++slink->queue.dropped == 1) {
                logioMsg(slink->lp, LOG_INFO, "*** WARNING *** %s: queue is full (%d packets), MiniSEED packet dropped",
                    fid, slink->queue.depth
                );
            }
        } else {
            if (!listAppend(slink->queue.list, &packed, sizeof(MSEED_PACKED *))) {
                logioMsg(slink->lp, LOG_INFO, "*** ERROR *** %s: listAppend: %s", fid, strerror(errno));
                logioMsg(slink->lp, LOG_INFO, "*** ERROR *** %s: SeedLink export terminated", fid);
                slink->enabled = FALSE;
            }
            SEM_POST(&slink->queue.sem);
        }
        slink->queue.empty = FALSE;

    MUTEX_UNLOCK(&slink->queue.mutex);
}

/* Get the list entry for the next item in the queue */

static LNKLST_NODE *DequeueMiniSEED(ISI_DL_SEEDLINK *slink)
{
BOOL retval;
LNKLST_NODE *node;
MSEED_HANDLE *handle;
static char *fid = "DequeueMiniSEED";

    MUTEX_LOCK(&slink->queue.mutex);

        handle = (MSEED_HANDLE *) slink->mseed;

        if ((node = listFirstNode(slink->queue.list)) == NULL) {
            slink->queue.empty = TRUE;
            slink->queue.dropped = 0;
        }

    MUTEX_UNLOCK(&slink->queue.mutex);

    return node;
}

/* relentlessly try to connect to the ringserver */

static void ConnectToRingServer(ISI_DL_SEEDLINK *slink)
{
DLCP *dlcp;

    dlcp = (DLCP *) slink->dlcp;

    if (dlcp->link > 0) return;

    while (1) {
        if (dl_connect(dlcp) != -1) {
            logioMsg(slink->lp, LOG_INFO, "connected to SeedLink@%s", slink->rsaddr);
            slink->first.sent = TRUE;
            return;
        }
        utilDelayMsec(slink->retry);
    }
}

/* Send a packet to the ringerserver via dlink */

static void SendToRingServer(ISI_DL_SEEDLINK *slink, MSEED_PACKED *packed)
{
DLCP *dlcp;
char streamid[100];
hptime_t begtime, endtime;
static int writeack = 0;
MSEED_HANDLE *handle;
static char *fid = "SendToRingServer";
char string[1024];

    dlcp = (DLCP *) slink->dlcp;
    handle = (MSEED_HANDLE *) slink->mseed;

    begtime = mseedConvertToHptime(packed->hdr.tstamp);
    endtime = mseedConvertToHptime(packed->hdr.endtime);

    while (dl_write(dlcp, packed->data, packed->len, packed->hdr.ident, begtime, endtime, writeack) < 0) {
        logioMsg(slink->lp, LOG_WARN, "%s: dl_write failed", fid);
        dl_disconnect(dlcp);
        ConnectToRingServer(slink);
    }

    if (slink->first.sent) {
        slink->first.sent = FALSE;
        logioMsg(slink->lp, LOG_INFO, "initial MiniSEED packet delivered to SeedLink@%s", slink->rsaddr);
    }
}

/* Main loop.  Take full MiniSEED packets from the queue and send to the ringserver */

static THREAD_FUNC RingServerWriteThread(void *argptr)
{
LNKLST_NODE *node;
MSEED_PACKED **ptrPacked;
ISI_DL_SEEDLINK *slink;
static char *fid = "RingServerWriteThread";

    slink = (ISI_DL_SEEDLINK *) argptr;
    SEM_POST(&slink->sem);

    ConnectToRingServer(slink);

    while (1) {
        SEM_WAIT(&slink->queue.sem);
        while ((node = DequeueMiniSEED(slink)) != NULL) {
            ptrPacked = (MSEED_PACKED **) node->payload;
            SendToRingServer(slink, *ptrPacked);
            free(*ptrPacked);
            MUTEX_LOCK(&slink->queue.mutex);
                listDestroyNode(node);
            MUTEX_UNLOCK(&slink->queue.mutex);
        }
    }
}

typedef struct {
    char server[MAXPATHLEN+1];
    int port;
    int reclen;
    int depth;
    char nname[MAXPATHLEN+1];
} SERVER_INFO;

static BOOL ParseServerString(LOGIO *lp, char *string, SERVER_INFO *info)
{
#define NUM_TOKEN 5
int ntoken;
char *token[NUM_TOKEN], *copy;
static char *fid = "ParseServerString";

    if (string == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL config string not allowed\n", fid);
        errno = EINVAL;
        return FALSE;
    }

    if ((copy = strdup(string)) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: strdup: %s", fid, strerror(errno));
        return FALSE;
    }

/* expecting strings of the form server:port:len:depth:net, eg "localhost:16000:512:500:II" */

    if ((ntoken = utilParse(copy, token, ":", NUM_TOKEN, 0)) < 0) {
        logioMsg(lp, LOG_ERR, "%s: utilParse: %s\n", fid, strerror(errno));
        free(copy);
        return FALSE;
    }
    if (ntoken != NUM_TOKEN) {
        logioMsg(lp, LOG_ERR, "%s: illegal slink parameter string '%s'\n", fid, string);
        errno = EINVAL;
        free(copy);
        return FALSE;
    }

/* 1st token is the name/IP of the server */

    strncpy(info->server, token[0], MAXPATHLEN);

/* 2nd token is the port */

    if ((info->port = atoi(token[1])) < 1) {
        logioMsg(lp, LOG_ERR, "%s: illegal port value '%s'\n", fid, token[1]);
        errno = EINVAL;
        free(copy);
        return FALSE;
    }

/* 3rd token is the record length (must be a power of 2) */

    info->reclen = atoi(token[2]);
    if (info->reclen < MSEED_MIN_RECLEN || info->reclen > MSEED_MAX_RECLEN || util_powerof(info->reclen, 2) < 0) {
        logioMsg(lp, LOG_ERR, "%s: illegal reclen value '%s' (must be a power of 2 between %d and %d)\n", fid, token[2], MSEED_MIN_RECLEN, MSEED_MAX_RECLEN);
        errno = EINVAL;
        free(copy);
        return FALSE;
    }

/* 4th token is the queue depth */

    if ((info->depth = (UINT32) atoi(token[3])) < 1) {
        logioMsg(lp, LOG_ERR, "%s: illegal depth value '%s'\n", fid, token[3]);
        errno = EINVAL;
        free(copy);
        return FALSE;
    }

/* 5th and final token is the network code */

    strncpy(info->nname, token[4], MAXPATHLEN);

    free(copy);
    return TRUE;
}

/* Intercept the libdali and libmseed log messages */

static void my_log_print(char *string)
{
    logioMsg(lp, LOG_INFO, string);
}

static void my_diag_print(char *string)
{
    logioMsg(lp, LOG_ERR, string);
}

void isidlLogSeedLinkOption(LOGIO *logio, int level, ISI_DL *dl)
{
MSEED_HANDLE *handle;

    if (dl == NULL) return;

    if (!dl->slink.enabled) {
        logioMsg(logio, level, "SeedLink export disabled");
        return;
    }

    handle = (MSEED_HANDLE *) dl->slink.mseed;
    logioMsg(logio, level, "SeedLink export enabled: %s:%d:%d:%s",
        dl->slink.rsaddr,
        handle->reclen,
        dl->slink.queue.depth,
        handle->netid
    );
    if (dl->slink.debug > 0) {
        mseedSetDebug(handle, dl->slink.debug);
        logioMsg(logio, level, "SeedLink debug enabled at level %d\n", dl->slink.debug);
    }
}

static BOOL InitRecordQ(LOGIO *lp, RECORDQ *recordq, int depth)
{
static char *fid = "InitRecordQ";

    MUTEX_INIT(&recordq->mutex);
    if ((recordq->list = listCreate()) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return FALSE;
    }
    SEM_INIT(&recordq->sem, 0, 1);
    recordq->depth = depth;
    recordq->dropped = 0;
    recordq->empty = TRUE;

    return TRUE;
}

BOOL isidlSetSeedLinkOption(ISI_DL *dl, char *argstr, char *ident, int debug)
{
LNKLST *tlist;
SERVER_INFO info;
MSEED_HANDLE *handle;
static char *fid = "isidlSetSeedLinkOption";

    if (dl == NULL || argstr == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (dl->slink.enabled) {
        logioMsg(dl->lp, LOG_ERR, "%s: multiple SeedLink servers not allowed\n", fid);
        errno = EINVAL;
        return FALSE;
    }

    dl->slink.lp              = dl->lp;
    dl->slink.retry           = ISI_DL_DEFAULT_SLINK_RETRY; /* no option to change */
    dl->slink.first.acquired  = TRUE; /* thread gets launched on receipt of first packet */
    dl->slink.first.sent      = TRUE;
    dl->slink.enabled         = TRUE;
    dl->slink.debug           = debug;

    if (!ParseServerString(dl->lp, argstr, &info)) return FALSE;
    sprintf(dl->slink.rsaddr, "%s:%d", info.server, info.port);
    dl->slink.queue.depth = info.depth;

    if (!InitRecordQ(dl->lp, &dl->slink.queue, info.depth)) {
        logioMsg(dl->lp, LOG_ERR, "%s: InitRecordQ\n", fid);
        return FALSE;
    }

    if ((dl->slink.dlcp = (void *) dl_newdlcp(dl->slink.rsaddr, ident)) == 0) {
        logioMsg(dl->slink.lp, LOG_ERR, "%s: dl_newdlcp: %s", fid, strerror(errno));
        return FALSE;
    }

    dl_loginit(0, (void *) my_log_print, NULL, (void *) my_diag_print, NULL);

    if ((handle = mseedCreateHandle(dl->lp, info.reclen, MSEED_QC_RAW, -1, EnqueueMiniSeedRecord, (void *) &dl->slink)) == NULL) {
        logioMsg(dl->slink.lp, LOG_ERR, "%s: mseedCreateHandle: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!mseedSetNetid(handle, info.nname)) {
        logioMsg(dl->slink.lp, LOG_ERR, "%s: mseedSetNetid: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!mseedSetStaid(handle, dl->sys->site)) {
        logioMsg(dl->slink.lp, LOG_ERR, "%s: mseedSetStaid: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!mseedSetDebug(handle, 1)) {
        logioMsg(dl->slink.lp, LOG_ERR, "%s: mseedSetDebug: %s", fid, strerror(errno));
        return FALSE;
    }

    dl->slink.mseed = handle;

    return TRUE;
}

static BOOL IDA1012BranchTaken(ISI_DL_SEEDLINK *slink, ISI_RAW_PACKET *raw)
{
MSEED_PACKED *packed;

    if (raw->hdr.desc.type != ISI_TYPE_IDA10) return FALSE;
    if (ida10SubFormatCode(raw->payload) != IDA10_SUBFORMAT_12) return FALSE;

    if ((packed = (MSEED_PACKED *) malloc(sizeof(MSEED_PACKED))) == NULL) return FALSE; /* no way to flag the error, but it will certainly show up elsewhere */

/* IDA10.12 means encapsulated 512-byte Miniseed, hence the use of explicit values */

    packed->len = 512;
    memcpy(packed->data, &raw->payload[64], packed->len);

    if (!mseedUnpackHeader(&packed->hdr, packed->data)) {
        free(packed);
        return FALSE;
    }

    EnqueueMiniSeedRecord((void *) slink, packed);

    return TRUE;
}

void isidlFeedSeedLink(ISI_DL_SEEDLINK *slink, ISI_RAW_PACKET *raw)
{
time_t QuitDelay;
MSEED_HANDLE *handle;
MSEED_RECORD record;
static char *fid = "isidlFeedSeedLink";

/* Nothing to do if we aren't running a SeedLink server */

    if (slink == NULL || !slink->enabled) return;
    handle = (MSEED_HANDLE *) slink->mseed;

/* Special handling for IDA10.12 packets */

    if (IDA1012BranchTaken(slink, raw)) return;

/* NULL raw input means flush all incomplete MiniSEED packets */

    if (raw == NULL) {
        logioMsg(slink->lp, LOG_INFO, "flushing all partial SeedLink packets");
        mseedFlushHandle(handle);
        /* give RingServerWriteThread time to drain the queue */
        QuitDelay = time(NULL) + 10; /* 10 seconds max */
        while (!QueueIsEmpty(slink) && time(NULL) < QuitDelay) sleep(1);
        return;
    }

/* First time through, launch the packet forwarding thread */

    if (slink->first.acquired) {
        SEM_INIT(&slink->sem, 0, 1);
        if (!THREAD_CREATE(&slink->tid, RingServerWriteThread, (void *) slink)) {
            logioMsg(slink->lp, LOG_ERR, "*** ERROR *** %s: can't start RingServerWriteThread!", fid);
            logioMsg(slink->lp, LOG_WARN, "** NOTICE ** SeedLink feed terminated");
            slink->enabled = FALSE;
            return;
        }
        SEM_WAIT(&slink->sem);
        slink->first.acquired = FALSE;
    }

/* At this point we have waveform data in an ISI_RAW_PACKET, convert to a MSEED_RECORD */

    memset(&record, 0, sizeof(MSEED_RECORD));
    if (!mseedConvertIsiRawPacket(handle, &record, raw)) {
        logioMsg(slink->lp, LOG_ERR, "*** ERROR *** %s: mseedConvertIsiRawPacket: ", fid, strerror(errno));
        logioMsg(slink->lp, LOG_WARN, "** NOTICE ** SeedLink feed terminated");
        slink->enabled = FALSE;
        return;
    }

/* Add the record to the handle.  The library will automatically generate packed
 * and compressed MiniSeed as the data are available, and pass those data back to
 * the application using the user supplied function.
 */

    if (!mseedAddRecordToHandle(handle, &record)) {
        logioMsg(slink->lp, LOG_ERR, "*** ERROR *** %s: mseedAddRecordToHandle: %s", fid, strerror(errno));
        logioMsg(slink->lp, LOG_WARN, "** NOTICE ** SeedLink feed terminated");
        slink->enabled = FALSE;
        return;
    }

    return;
}

/* Revision History
 *
 * $Log: slink.c,v $
 * Revision 1.18  2017/12/20 23:56:58  dechavez
 * added IDA1012BranchTaken() (untested)
 *
 * Revision 1.17  2015/12/04 22:11:18  dechavez
 * added missing return value to InitRecordQ()
 *
 * Revision 1.16  2014/08/28 21:23:22  dechavez
 * include dmc/libmseed.h since it is now gone from isi/dl.h
 *
 * Revision 1.15  2014/08/19 18:53:01  dechavez
 * reworked to use MSEED_PACKED instead of MSEED_RECORD (libmseed 2.1.0)
 *
 * Revision 1.14  2014/08/11 18:31:01  dechavez
 * major rework to use redesigned libmseed
 *
 * Revision 1.13  2014/04/30 15:54:22  dechavez
 * rework to use MSTemplateHandle clock quality and status bit support
 *
 * Revision 1.12  2013/03/27 22:58:11  dechavez
 * fixed "n" arg to strncpy for mseed.net
 *
 * Revision 1.11  2013/03/08 19:56:01  dechavez
 * fixed bug with timeout logic during queue flush
 *
 * Revision 1.10  2013/03/08 18:29:54  dechavez
 * added ability to flush partial MiniSEED packets
 *
 * Revision 1.9  2013/02/07 23:33:12  dechavez
 * Memory leak was NOT fixed in 1.21.1 (sigh).  Reworked buffering. Again.
 *
 * Revision 1.8  2013/02/07 21:11:52  dechavez
 * fixed LNKLST_MODE memory leak in PopMiniSEED
 *
 * Revision 1.7  2013/02/07 18:12:47  dechavez
 * reworked to use RECORDQ for buffering output data
 *
 * Revision 1.6  2013/02/05 21:23:15  dechavez
 * fixed time stamp truncation error (MAJOR BUG!)
 *
 * Revision 1.5  2013/01/23 23:48:44  dechavez
 * init slink->mseed.msr to NULL, instead of using msr_init()
 *
 * Revision 1.4  2013/01/23 23:35:01  dechavez
 * fixed uninitialized slink->mseed.msr BUG, added extra sanity checks, improved(?) logging
 *
 * Revision 1.3  2013/01/22 22:32:17  dechavez
 * don't print message when tossing full MiniSEED packets after queue buffer is full
 *
 * Revision 1.2  2013/01/22 21:26:51  dechavez
 * fixed memory link in SendToRingServer(), set msgq log pointer
 *
 * Revision 1.1  2013/01/14 22:12:20  dechavez
 * created
 *
 */
