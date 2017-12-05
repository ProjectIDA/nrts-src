#pragma ident "$Id: trigger.c,v 1.6 2015/12/09 18:42:28 dechavez Exp $"
/*======================================================================
 *
 *  Event detector
 *
 *  This trigger assumes that the incoming high level packets for
 *  each channel span identical packet boundaries.  One consequence
 *  of this is that it will not work in general for packets whose data
 *  are compressed.
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_TRIGGER

#define TRIGGER_DISABLED -1 /* trigger suppressed, everything continuous */
#define TRIGGER_IDLE      0 /* no event in progress */
#define TRIGGER_MANUAL    1 /* manual trigger activated */
#define TRIGGER_ACTIVE    2 /* triggered event in progress */
#define TRIGGER_POST      3 /* dumping post event memory */

/* We just can't memcpy or otherwise assign the QDP_HLPs we get during
 * the callback because we need to save them in pre-event memory.  The
 * data field of the QDP_HLP is constantly overwritten, so we have to
 * make our own copy.  The SAFE_HLP structure includes a private data
 * field that is used to store this copy.  We just make the QDP_HLP
 * data field point to this.  We also include the pointers into the
 * pre-event memory message queue to simplify releasing the packet.
 */

typedef struct {
    UINT8 *data;
    UINT16 nbyte;
    QDP_HLP hlp;
    struct {
        MSGQ *pem;
        MSGQ_MSG *msg;
    } self;
} SAFE_HLP;

typedef struct {
    char ident[QDP_CNAME_LEN+QDP_LNAME_LEN+1]; /* for logging convenience */
    char chn[QDP_CNAME_LEN+1]; /* channel name */
    char loc[QDP_LNAME_LEN+1]; /* location code */
    MSGQ pem;                  /* pre-event memory */
    SAFE_HLP *shlp;            /* working packet */
    DETECTOR engine;           /* libdetect detector engine */
} CHANNEL_DATA;

static struct {
    int state;              /* one of TRIGGER_IDLE, etc., from above */
    Q330_DETECTOR detector; /* event detector */
    LNKLST *chan;           /* list of CHANNEL_DATA (one per triggered channel) */
    struct {
        char path[MAXPATHLEN + 1]; /* if path exists, trigger is in effect */
        INT32 duration;            /* if positive, trigger duration in packets */
    } manual;
    int votes;              /* number of triggered channels */
    UINT32 post;            /* post-event countdown */
    UINT32 flags;           /* runtime options */
} trigger;

static QHLP_DL_FUNC WriteToDiskLoop;      /* function to reformat HLP and write it to disk loop */

static void ReportMisconfiguredDetectorChannels(CHANNEL_DATA *chan)
{
int count = 0;
int UnusedPem;
LNKLST_NODE *crnt;
#define MSGBUFLEN 1023
char msgbuf[MSGBUFLEN+1];
static char *fid = "ReportMisconfiguredDetectorChannels";

    UnusedPem = trigger.detector.pre + 1;

    sprintf(msgbuf, "*** DETECTOR CONFIG ERROR *** detector requires:");

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (msgqNfree(&chan->pem) == UnusedPem) {
            strncat(msgbuf, " ",         sizeof(msgbuf) - strlen(msgbuf) - 1);
            strncat(msgbuf, chan->ident, sizeof(msgbuf) - strlen(msgbuf) - 1);
            ++count;
        }
        crnt = listNextNode(crnt);
    }

    if (count == 0) {
        LogMsg("*** BUG *** %s called yet no missing channels found *** BUG ***", fid);
        Exit(MY_MOD_ID + 1);
    }

    LogMsg(msgbuf);
}

static SAFE_HLP *GetHLP(CHANNEL_DATA *chan)
{
SAFE_HLP *shlp;
MSGQ_MSG *msg;
static char *fid = "GetHLP";

/* If this fails, then it means we ran out of pre-event memory space while
 * waiting for all the detector channels to align.  That happens when we
 * specify one or more detector channels which are not actually being produced.
 */

    if ((msg = msgqGet(&chan->pem, MSGQ_NOWAIT)) == NULL) return NULL;

    shlp = (SAFE_HLP *) msg->data;
    shlp->self.pem = &chan->pem;
    shlp->self.msg = msg;
    return shlp;
}

static SAFE_HLP *ReleaseHLP(SAFE_HLP *shlp)
{
    shlp->hlp.nbyte = 0;
    msgqPut(shlp->self.pem, shlp->self.msg);
    return NULL;
}

static SAFE_HLP *InsertHLP(SAFE_HLP *shlp)
{
    msgqPut(shlp->self.pem, shlp->self.msg);
    return NULL;
}

/* Flush pre-event memory */

static void FlushPreEventMemoryBuffer(DISK_LOOP_DATA *output, CHANNEL_DATA *chan)
{
SAFE_HLP *shlp;
BOOL finished = FALSE;
static char *fid = "FlushPreEventMemoryBuffer";

    logioUpdateWatchdog(Watch, fid);
    while (!finished) {
        if ((shlp = GetHLP(chan)) == NULL) {
            LogMsg("*** ERROR *** GetHLP failed in %s (this should NEVER occur)!", fid);
            Exit(MY_MOD_ID + 2);
        }
        if (shlp->hlp.nbyte == 0) {
            finished = TRUE;
        } else {
            if (output != NULL) (WriteToDiskLoop)(output, &shlp->hlp);
        }
        ReleaseHLP(shlp);
    }
}

/* Write all working packets to disk loop */

static void WriteWorkingPackets(DISK_LOOP_DATA *output)
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WriteWorkingPackets";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) {
            (WriteToDiskLoop)(output, &chan->shlp->hlp);
            chan->shlp = InsertHLP(chan->shlp); /* working packets forwarded to pre-event memory */
        }
        crnt = listNextNode(crnt);
    }
}

/* Write all pre-event memory packets to the disk loop */

static void WritePreEventMemory(DISK_LOOP_DATA *output)
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WritePreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        FlushPreEventMemoryBuffer(output, chan);
        crnt = listNextNode(crnt);
    }
}

/* Toss all pre-event memory packets */

static void TossPreEventMemory()
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "TossPreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        FlushPreEventMemoryBuffer(NULL, chan);
        crnt = listNextNode(crnt);
    }
}

/* Copy a high level packet, preserving the data */

static BOOL CopyHLP(SAFE_HLP *dst, QDP_HLP *src)
{
static char *fid = "ProcessTrigChan:CopyHLP";

    if (src->nbyte > dst->nbyte) {
        if ((dst->data = realloc(dst->data, src->nbyte)) == NULL) return FALSE;
        dst->nbyte = src->nbyte;
    }

    dst->hlp = *src;
    memcpy(dst->data, src->data, src->nbyte);
    dst->hlp.data = dst->data;
    dst->hlp.nbyte = src->nbyte;

    return TRUE;

}

/* Move all working packets to pre-event memory */

static void UpdatePreEventMemory()
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
static char *fid = "UpdatePreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) chan->shlp = InsertHLP(chan->shlp);
        crnt = listNextNode(crnt);
    }
}

/* Intialize a pre-event memory buffer */

static BOOL InitPreEventMemory(MSGQ *pem)
{
int nelem;
MSGQ_MSG *msg;
SAFE_HLP *shlp;
static char *fid = "InitPreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    nelem = trigger.detector.pre + 1; /* pre-event memory plus working packet */

    if (!msgqInit(pem, nelem, nelem, sizeof(SAFE_HLP))) {
        fprintf(stderr, "ERROR: %s: msgqInit: %s\n", fid, strerror(errno));
        return FALSE;
    }

    while (nelem) {
        if ((msg = msgqGet(pem, MSGQ_NOWAIT)) == NULL) {
            fprintf(stderr, "%s: msgqGet: %s (should NEVER OCCUR)", fid, strerror(errno));
            return FALSE;
        }
        shlp = (SAFE_HLP *) msg->data;
        shlp->data = NULL;
        shlp->nbyte = 0;
        shlp->hlp.nbyte = 0;
        msgqPut(pem, msg);
        --nelem;
    }

    return TRUE;
}

/* Create the list of triggered channels */

static LNKLST *BuildChannelList()
{
char *chnloc;
CHANNEL_DATA new;
LNKLST_NODE *crnt;
LNKLST *chan, *name;
static char *fid = "InitTrigger:BuildChannelList";

    logioUpdateWatchdog(Watch, fid);
    if ((chan = listCreate()) == NULL) {
        fprintf(stderr, "%s: listCreate: %s\n", fid, strerror(errno));
        return NULL;
    }

/* Crack the channel list (must be of the form stalc:stalc...) */

    if ((name = utilStringTokenList(trigger.detector.channels, ",/:", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return NULL;
    }

/* Create a CHANNEL_DATA entry for each specified channel */

    crnt = listFirstNode(name);
    while (crnt != NULL) {
        chnloc = (char *) crnt->payload;
        if (strlen(chnloc) != 3 && strlen(chnloc) != 5) {
            fprintf(stderr, "%s: triggered channel name '%s' has wrong length\n", fid, chnloc);
            return NULL;
        }
        memcpy(new.chn, chnloc, 3); new.chn[3] = 0;
        if (strlen(chnloc) == 5) {
            memcpy(new.loc, chnloc+3, 2); new.loc[2] = 0;
            sprintf(new.ident, "%s%s", new.chn, new.loc);
        } else {
            sprintf(new.loc, "  ");
            sprintf(new.ident, "%s", new.chn);
        }
        new.engine = trigger.detector.engine;
        detectInit(&new.engine);
        if (!InitPreEventMemory(&new.pem)) return NULL;
        new.shlp = NULL;

        if (!listAppend(chan, &new, sizeof(CHANNEL_DATA))) {
            fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
            return NULL;
        }
        crnt = listNextNode(crnt);
    }

    listDestroy(name);

/* Return the newly created list */

    return chan;
}

/* See if the current HLP is from a triggered channel (NULL => continous, else triggered) */

static CHANNEL_DATA *GetTriggeredChan(QDP_HLP *hlp)
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan, new;
static char *fid = "ProcessTrigChan:GetTriggeredChan";

    logioUpdateWatchdog(Watch, fid);

/* Immediate return if we are not running the event trigger */

    if (trigger.state == TRIGGER_DISABLED) return NULL;

/* Triggered channels have channel buffers, continuous channels do not */

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (strcmp(hlp->chn, chan->chn) == 0 && strcmp(hlp->loc, chan->loc) == 0) return chan;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

/* Determine if a manual trigger has been set */

static BOOL ManualTrigger()
{
FILE *fp;
static char *fid = "ManualTrigger";

    logioUpdateWatchdog(Watch, fid);

    if ((fp = fopen(trigger.manual.path, "r")) == NULL) return FALSE;

/* Trigger file is present, read trigger duration */

    fscanf(fp, "%d", &trigger.manual.duration);
    fclose(fp);

    if (trigger.manual.duration <= 0) {
        LogMsg("Trigger ON (continuous manual trigger)");
        trigger.state = TRIGGER_MANUAL;
    } else {
        LogMsg("Trigger ON (%ld packet manual trigger)", trigger.manual.duration);
        trigger.post = trigger.manual.duration;
        trigger.state = TRIGGER_POST;
    }

    return TRUE;
}

/* Check to see if all triggered channels are present, with the same data window */

static BOOL CheckAlignment()
{
UINT64 master;
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "ProcessTrigChan:CheckAlignment";

    logioUpdateWatchdog(Watch, fid);

/* Use the working packet from the first channel in the list set the master time stamp */

    if ((crnt = listFirstNode(trigger.chan)) == NULL) {
        LogMsg("PROGRAM ERROR: %s: unexpected NULL list", fid);
        Exit(MY_MOD_ID + 3);
    }

    chan = (CHANNEL_DATA *) crnt->payload;
    if (chan->shlp == NULL) return FALSE;

    master = chan->shlp->hlp.tols;

    crnt = listNextNode(crnt);

/* Now look at the rest, but only if all have the same time stamp */

    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp == NULL || chan->shlp->hlp.tols != master) return FALSE;
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

/* Run the event detector on the working packets.  The HLP data are in network byte
 * order, so we have to byteswap on little endian systems before passing the the
 * detector, and restore to nework byte order when done.
 */

static void EventDetector()
{
QDP_HLP *work;
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
static char *fid = "ProcessTrigChan:EventDetector";

    logioUpdateWatchdog(Watch, fid);

    trigger.votes = 0;

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        work = &chan->shlp->hlp;
        switch (work->format) {
          case QDP_HLP_FORMAT_NOCOMP32:
#ifdef LTL_ENDIAN_HOST
            utilSwapUINT32((UINT32 *) work->data, (long) work->nsamp);
#endif /* LTL_ENDIAN_HOST */
            detectProcessINT32(&chan->engine, (INT32 *) work->data, (UINT32) work->nsamp);
#ifdef LTL_ENDIAN_HOST
            utilSwapUINT32((UINT32 *) work->data, (long) work->nsamp);
#endif /* LTL_ENDIAN_HOST */
            if (chan->engine.state == DETECTOR_STATE_ON) ++trigger.votes;
            break;
          default:
            LogMsg("PROGRAM ERROR: %s: unsupported HLP format type %d", fid, work->format);
            Exit(MY_MOD_ID + 4);
        }
        crnt = listNextNode(crnt);
    }
}

/* Copy newly received packet into the workspace */

static void UpdateWorkingPacket(DISK_LOOP_DATA *output, CHANNEL_DATA *chan, QDP_HLP *new)
{
LNKLST_NODE *crnt;
static char *fid = "UpdateWorkingPacket";

    if (msgqNfree(&chan->pem) == 0) {
        ReportMisconfiguredDetectorChannels(chan);
        Exit(MY_MOD_ID + 5);
    }

    if ((chan->shlp = GetHLP(chan)) == NULL) {
        LogMsg("*** ERROR *** GetHLP failed in %s (this should NEVER occur)!", fid);
        Exit(MY_MOD_ID + 6);
    }
    if (!CopyHLP(chan->shlp, new)) {
        LogMsg("%s: CopyHLP: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
}

static void EventOver()
{
    LogMsg("Event over");
    trigger.state = TRIGGER_IDLE;
    utilDeleteFile(trigger.manual.path);
    trigger.manual.duration = 0;
}

static void LogDetectionDetails()
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
char buf[1024], buf2[1024];
static char *fid = "LogDetectionDetails";

    logioUpdateWatchdog(Watch, fid);

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        buf[0] = 0;
        chan = (CHANNEL_DATA *) crnt->payload;
        sprintf(buf+strlen(buf), "%s ", chan->ident);
        sprintf(buf+strlen(buf), "%s", detectTriggerString(&chan->engine, buf2));
        LogMsg(buf);
        crnt = listNextNode(crnt);
    }
}

/* Log routine */

void LogTriggerData()
{
    if (trigger.flags & QHLP_TRIG_CONTINUOUS) {
        LogMsg("NOTICE: Event trigger suppressed (all channels are continuous)");
        return;
    }

    if (trigger.state == TRIGGER_DISABLED) {
        LogMsg("event trigger DISABLED");
        return;
    }

    switch (trigger.detector.engine.type) {
      case DETECTOR_TYPE_STALTA:
        LogMsg("STA/LTA event detector: %s %d %lu %lu %lu %lu %.2f %.2f",
            trigger.detector.channels,
            trigger.detector.votes,
            trigger.detector.pre,
            trigger.detector.pst,
            trigger.detector.engine.stalta.config.len.sta,
            trigger.detector.engine.stalta.config.len.lta,
            trigger.detector.engine.stalta.config.ratio.on,
            trigger.detector.engine.stalta.config.ratio.off
        );
        break;
      default:
        LogMsg("UNEXPECTED TYPE %d event detector: %s =  %d %lu %lu",
            trigger.detector.engine.type,
            trigger.detector.channels,
            trigger.detector.votes,
            trigger.detector.pre,
            trigger.detector.pst
        );
    }

    LogMsg("Manual trigger path = %s", trigger.manual.path);

}

/* Initialization routine */

BOOL InitTrigger(Q330_CFG *cfg, ISI_DL *dl, char *metadir, QHLP_DL_FUNC saveFunc, UINT32 trigflags)
{
static char *fid = "InitTrigger";

    WriteToDiskLoop = saveFunc;

    trigger.flags = trigflags;
    if (trigger.flags & QHLP_TRIG_CONTINUOUS) {
        trigger.state = TRIGGER_DISABLED;
        return TRUE;
    }

    if (!q330GetDetector(dl->sys->site, cfg, &trigger.detector)) return FALSE;
    if (!trigger.detector.enabled) {
        trigger.state = TRIGGER_DISABLED;
        LogMsg("No detector found for site '%s'", dl->sys->site);
    } else {
        trigger.state = TRIGGER_IDLE;
        sprintf(trigger.manual.path, "%s/trigger", metadir != NULL ? metadir : "/dev/null");
        if ((trigger.chan = BuildChannelList()) == NULL) {
            fprintf(stderr, "%s: BuildChannelList: %s\n", fid, strerror(errno));
            return FALSE;
        }
        if (utilFileExists(trigger.manual.path)) utilDeleteFile(trigger.manual.path);
    }

    logioUpdateWatchdog(Watch, fid);
    return TRUE;
}

/* Process a newly created high level packet */

void ProcessTrigChan(DISK_LOOP_DATA *output, QDP_HLP *hlp)
{
CHANNEL_DATA *chan;
static char *fid = "ProcessTrigChan";

    logioUpdateWatchdog(Watch, fid);

    if (output == NULL || hlp == NULL) {
        LogMsg("%s: NULL input(s)!\n", fid);
        Exit(MY_MOD_ID + 8);
    }

/* If the packet isn't a triggered channel, then we just save it */

    if (trigger.flags & QHLP_TRIG_CONTINUOUS || (chan = GetTriggeredChan(hlp)) == NULL) {
        (WriteToDiskLoop)(output, hlp);
        return;
    }

/* Must be a triggered channel, save it */

    UpdateWorkingPacket(output, chan, hlp);

/* Don't do any detection processing until all channels are aligned */

    if (!CheckAlignment()) return;

/* Run the event detector on all channels (unless we are already on manually) */

    if (trigger.state != TRIGGER_MANUAL) EventDetector();

/* If the trigger is off, it can be turned on either by voters or by the operator */

    if (trigger.state == TRIGGER_IDLE) {
        if (trigger.votes >= trigger.detector.votes) {
            LogMsg("Event ON (%d votes)", trigger.votes);
            LogDetectionDetails();
            if (!utilCreateFile(trigger.manual.path, 0, FALSE, 0)) {
                LogMsg("%s: ProcessTrigChan: utilCreateFile: %s: %s", fid, trigger.manual.path, strerror(errno));
                Exit(MY_MOD_ID + 9);
            }
            WritePreEventMemory(output);
            trigger.state = TRIGGER_ACTIVE;
        } else if (ManualTrigger()) {
            TossPreEventMemory();
        }
    }

/* Look for re-triggers */

    if (trigger.state == TRIGGER_POST && trigger.votes >= trigger.detector.votes) {
        LogMsg("Event Retrigger (%d votes)", trigger.votes);
        LogDetectionDetails();
        trigger.state = TRIGGER_ACTIVE;
    }

/* Look for operator abort of an active trigger */

    if (trigger.state != TRIGGER_IDLE && !utilFileExists(trigger.manual.path)) {
        LogMsg("Trigger OFF (terminated by operator)");
        trigger.state = TRIGGER_IDLE;
    }

/* Look for detriggers */

    if (trigger.state == TRIGGER_ACTIVE && trigger.votes < trigger.detector.votes) {
        LogMsg("Event trigger off");
        if ((trigger.post = trigger.detector.pst) > 0) {
            trigger.state = TRIGGER_POST;
        } else {
            EventOver();
        }
    }

    switch (trigger.state) {
      case TRIGGER_IDLE:
        UpdatePreEventMemory();
        return;

      case TRIGGER_ACTIVE:
      case TRIGGER_MANUAL:
        WriteWorkingPackets(output);
        return;

      case TRIGGER_POST:
        WriteWorkingPackets(output);
        if (--trigger.post == 0) EventOver();
        return;
    }

    LogMsg("%s: unexpected switch fall through, state = %d", fid, trigger.state);
    Exit(MY_MOD_ID + 11);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: trigger.c,v $
 * Revision 1.6  2015/12/09 18:42:28  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.5  2010/05/10 16:26:20  dechavez
 * skip the event detector if a manual trigger is in progress... this prevents
 * the manual trigger from terminating automatically by having an event come
 * in and trigger/detrigger
 *
 * Revision 1.4  2010/03/12 00:49:02  dechavez
 * initial release
 *
 */
