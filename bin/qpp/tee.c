#pragma ident "$Id: tee.c,v 1.5 2015/12/09 18:43:37 dechavez Exp $"
/*======================================================================
 *
 *  Tee packets to disk, with optional event triggering.  The trigger
 *  code is derived from qhlp's process.c file... this should really
 *  be encapsulated into some library.
 *
 *====================================================================*/
#include "qpp.h"
#include "q330.h"
#include "detect.h"

#define TRIGGER_DISABLED -1 /* trigger suppressed, everything continuous */
#define TRIGGER_IDLE      0 /* no event in progress */
#define TRIGGER_ACTIVE    1 /* triggered event in progress */
#define TRIGGER_CALIB     2 /* calibration trigger in progress */
#define TRIGGER_POST      3 /* dumping post event memory */

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
    int votes;              /* number of triggered channels */
    UINT32 post;            /* post-event countdown */
    BOOL cal_in_progress;   /* calibrator status from most recent C1_STAT */
    UINT64 serialno;        /* serial number of digitizer with active calibration */
} trigger;

static BOOL TrigChanMatch(QDP_HLP *hlp, CHANNEL_DATA *chan)
{
    if (strcmp(hlp->loc, chan->loc) != 0) return FALSE;
    if (strcmp(hlp->chn, chan->chn) == 0) return TRUE;
    if (strcasecmp(hlp->chn, "enn") == 0 && strcasecmp(chan->chn, "en1") == 0) return TRUE;
    if (strcasecmp(hlp->chn, "ene") == 0 && strcasecmp(chan->chn, "en2") == 0) return TRUE;
    return FALSE;
}

static char *TeeDir = NULL;

void TeeHLP(QDP_HLP *hlp)
{
int trecs = DEFAULT_TRECS, errnum;
static ISI_SEQNO seqno = {ISI_UNDEF_SEQNO_SIG, 0LL};
static gzFile *gz = NULL;
char path[MAXPATHLEN+1];
UINT8 buf[IDA10_FIXEDRECLEN];
static struct {
    UINT64 crnt;
    UINT64 prev;
} suffix = {0LL, 0LL};
static char *fid = "TeeHLP";

    if (hlp == NULL) {
        if (gz != NULL) gzclose(gz);
        return;
    }

    if (seqno.signature == ISI_UNDEF_SEQNO_SIG) seqno.signature = time(NULL);

    if (!qdpHlpToIDA10(buf, hlp, (UINT32) seqno.counter++)) {
        LogMsg("%s: *** ERROR *** qdpHlpToIDA10: %s", fid, strerror(errno));
        exit(1);
    }

    suffix.crnt = (seqno.counter / trecs) * trecs;
    if (suffix.crnt != suffix.prev || gz == NULL) {
        if (gz != NULL) gzclose(gz);
        sprintf(path, "%s/%08x%016llx.gz", TeeDir, seqno.signature, suffix.crnt);
        if ((gz = gzopen(path, "wb")) == NULL) {
            LogMsg("%s: *** ERROR *** gzopen: %s: %s", fid, path, strerror(errno));
            exit(1);
        }
    }

    if (gzwrite(gz, buf, IDA10_FIXEDRECLEN) != IDA10_FIXEDRECLEN) LogMsg("%s: *** WARNING *** gzwrite: %s", fid, gzerror(gz, &errnum));

    suffix.prev = suffix.crnt;
}

void CheckCalibrationStatus(UINT64 serialno, QDP_LCQ *lcq)
{
REAL64 tstamp;

    if (!trigger.cal_in_progress) {
        if (lcq->cal_in_progress) {
            tstamp = (qdpRootTimeTag(&lcq->mn232) / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
            LogMsg("Calibration detected @ %s", utilDttostr(tstamp, 0, NULL));
            trigger.cal_in_progress = TRUE;
            trigger.serialno = serialno;
        }
    } else {
        if (serialno != trigger.serialno) return;
        if (!lcq->cal_in_progress) {
            tstamp = (qdpRootTimeTag(&lcq->mn232) / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
            LogMsg("Calibration finished @ %s", utilDttostr(tstamp, 0, NULL));
            trigger.cal_in_progress = FALSE;
        }
    }
}

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

    if (count == 0) LogMsg("*** BUG *** %s called yet no missing channels found *** BUG ***", fid);

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

static void FlushPreEventMemoryBuffer(CHANNEL_DATA *chan)
{
SAFE_HLP *shlp;
BOOL finished = FALSE;
static char *fid = "FlushPreEventMemoryBuffer";

    while (!finished) {
        if ((shlp = GetHLP(chan)) == NULL) {
            LogMsg("*** ERROR *** GetHLP failed in %s (this should NEVER occur)!", fid);
            exit(1);
        }
        if (shlp->hlp.nbyte == 0) {
            finished = TRUE;
        } else {
            TeeHLP(&shlp->hlp);
        }
        ReleaseHLP(shlp);
    }
}

/* Write all working packets to tee file */

static void WriteWorkingPackets()
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WriteWorkingPackets";

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) {
            TeeHLP(&chan->shlp->hlp);
            chan->shlp = InsertHLP(chan->shlp); /* working packets forwarded to pre-event memory */
        }
        crnt = listNextNode(crnt);
    }
}

/* Write all pre-event memory packets to the disk loop */

static void WritePreEventMemory()
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WritePreEventMemory";

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        FlushPreEventMemoryBuffer(chan);
        crnt = listNextNode(crnt);
    }
}

/* Copy a high level packet, preserving the data */
    
static BOOL CopyHLP(SAFE_HLP *dst, QDP_HLP *src)
{
static char *fid = "TeeCallback:CopyHLP";

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

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) chan->shlp = InsertHLP(chan->shlp);
        crnt = listNextNode(crnt);
    }
}

static void LogTriggerData()
{
    if (trigger.state == TRIGGER_DISABLED) {
        LogMsg("Event trigger disabled (all channels are continuous)");
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
}

/* Check to see if all triggered channels are present, with the same data window */

static BOOL CheckAlignment()
{
UINT64 master;
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "TeeCallback:CheckAlignment";

/* Use the working packet from the first channel in the list set the master time stamp */

    if ((crnt = listFirstNode(trigger.chan)) == NULL) {
        LogMsg("PROGRAM ERROR: %s: unexpected NULL list", fid);
        exit(1);
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
static char *fid = "TeeCallback:EventDetector";

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
            exit(1);
        }
        crnt = listNextNode(crnt);
    }
}

/* Copy newly received packet into the workspace */

static void UpdateWorkingPacket(CHANNEL_DATA *chan, QDP_HLP *new)
{
LNKLST_NODE *crnt;
static char *fid = "UpdateWorkingPacket";

    if (msgqNfree(&chan->pem) == 0) {
        ReportMisconfiguredDetectorChannels(chan);
        exit(1);
    }

    if ((chan->shlp = GetHLP(chan)) == NULL) {
        LogMsg("*** ERROR *** GetHLP failed in %s (this should NEVER occur)!", fid);
        exit(1);
    }
    if (!CopyHLP(chan->shlp, new)) {
        LogMsg("%s: CopyHLP: %s", fid, strerror(errno));
        exit(1);
    }
}

static void TriggerOff()
{
    if (trigger.cal_in_progress) {
        LogMsg("Event over while calibration active, trigger remains ON");
        trigger.state = TRIGGER_CALIB;
    } else {
        LogMsg("Trigger OFF");
        trigger.state = TRIGGER_IDLE;
    }
}

static void LogDetectionDetails()
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
char buf[1024], buf2[1024];
static char *fid = "LogDetectionDetails";

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
 
/* See if the current HLP is from a triggered channel (NULL => continous, else triggered) */

static CHANNEL_DATA *GetTriggeredChan(QDP_HLP *hlp)
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan, new;
static char *fid = "TeeCallback:GetTriggeredChan";
 
/* Immediate return if we are not running the event trigger */

    if (trigger.state == TRIGGER_DISABLED) return NULL;
 
/* Triggered channels have channel buffers, continuous channels do not */
 
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (TrigChanMatch(hlp, chan)) return chan;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

/* Intialize a pre-event memory buffer */

static BOOL InitPreEventMemory(MSGQ *pem)
{
int nelem;
MSGQ_MSG *msg;
SAFE_HLP *shlp;
static char *fid = "InitPreEventMemory";

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

static BOOL InitTrigger(char *cfgstr)
{

#define DELIMITERS " \t"
#define MAX_TOKEN  32
#define QUOTE      0
int ntoken;
char *token[MAX_TOKEN];
char defstr[] = DEFAULT_DETECTOR;
static char *fid = "InitTee:InitTrigger";

    if (cfgstr == NULL) {
        trigger.state = TRIGGER_DISABLED;
        return TRUE;
    }

    ntoken = utilParse(cfgstr, token, DELIMITERS, MAX_TOKEN, QUOTE);
    if (ntoken != 1 && ntoken < 7) {
        fprintf(stderr, "%s: *** ERROR *** illegal detector string '%s'\n", fid, cfgstr);
        return FALSE;
    }

    strlcpy(trigger.detector.channels, token[0], MAXPATHLEN+1);
    if (ntoken == 1) ntoken = utilParse(defstr, token, DELIMITERS, MAX_TOKEN, QUOTE);

    trigger.detector.name[0] = 0; /* unused */
    trigger.detector.votes = atol(token[1]);
    trigger.detector.pre = (UINT32) atol(token[2]);
    trigger.detector.pst = (UINT32) atol(token[3]);
     
    if (strcasecmp(token[4], "stalta") != 0 || ntoken != 9) {
        fprintf(stderr, "%s: *** ERROR *** illegal detector string '%s'\n", fid, cfgstr);
        return FALSE;
    }
    trigger.detector.engine.type = DETECTOR_TYPE_STALTA;
    trigger.detector.engine.stalta.config.len.sta = (UINT32) atol(token[5]);
    trigger.detector.engine.stalta.config.len.lta = (UINT32) atol(token[6]);
    trigger.detector.engine.stalta.config.ratio.on = (REAL32) atof(token[7]);
    trigger.detector.engine.stalta.config.ratio.off = (REAL32) atof(token[8]);
    trigger.detector.enabled = TRUE;

    trigger.state = TRIGGER_IDLE;
    trigger.cal_in_progress = FALSE;

    if ((trigger.chan = BuildChannelList()) == NULL) return FALSE;

    return TRUE;
}

BOOL InitTee(QDP_HLP_RULES *rules, char *dir, char *detect)
{

    if ((TeeDir = strdup(dir)) == NULL) {
        perror("strdup");
        return FALSE;
    }

    if (util_mkpath(TeeDir, 0755) != 0) {
        fprintf(stderr, "*** ERROR *** unable to mkdir %s: %s\n", TeeDir, strerror(errno));
        return FALSE;
    }

    if (!InitTrigger(detect)) return FALSE;

    return qdpInitHLPRules(
        rules,
        IDA10_DEFDATALEN,
        QDP_HLP_FORMAT_NOCOMP32,
        TeeCallback,
        (void *) NULL,
        QDP_HLP_RULE_FLAG_LCASE | QDP_HLP_RULE_FLAG_STRICT
    );
}

void TeeCallback(void *unused, QDP_HLP *hlp)
{
CHANNEL_DATA *chan;
static char *fid = "TeeCallback";

    if (hlp == NULL) {
        LogMsg("%s: *** ERROR *** NULL input(s)!", fid);
        exit(1);
    }

/* Go directly to disk if we aren't triggering */

    if (trigger.state == TRIGGER_DISABLED) {
        TeeHLP(hlp);
        return;
    }

/* Go directly to disk if this isn't one of the triggered channels */

    if ((chan = GetTriggeredChan(hlp)) == NULL) {
        TeeHLP(hlp);
        return;
    }

/* Must be a triggered channel, save a copy */

    UpdateWorkingPacket(chan, hlp);

/* Don't do any detection processing until all channels are aligned */

    if (!CheckAlignment()) return;

/* Run the event detector on all channels */

    EventDetector();

/* If the trigger is off, it can be turned on either by voters or by a calibration */

    if (trigger.state == TRIGGER_IDLE) {
        if (trigger.votes >= trigger.detector.votes) {
            LogDetectionDetails();
            WritePreEventMemory();
            LogMsg("Trigger ON (%d votes)", trigger.votes);
            trigger.state = TRIGGER_ACTIVE;
        } else if (trigger.cal_in_progress) {
            LogMsg("Trigger ON (calibration in progress)");
            trigger.state = TRIGGER_CALIB;
        }
    }

/* Look for re-triggers */

    if (trigger.state == TRIGGER_POST && trigger.votes >= trigger.detector.votes) {
        LogMsg("Event Retrigger (%d votes)", trigger.votes);
        LogDetectionDetails();
        trigger.state = TRIGGER_ACTIVE;
    }

/* Look for detriggers */

    if (trigger.state == TRIGGER_ACTIVE && trigger.votes < trigger.detector.votes) {
        LogMsg("Event over");
        if ((trigger.post = trigger.detector.pst) > 0) {
            trigger.state = TRIGGER_POST;
        } else {
            TriggerOff();
        }
    }

/* Look for calibration off transitions */

    if (trigger.state == TRIGGER_CALIB && !trigger.cal_in_progress) {
        LogMsg("Trigger OFF (calibration over)");
        trigger.state = TRIGGER_IDLE;
    }

    switch (trigger.state) {
      case TRIGGER_IDLE:
        UpdatePreEventMemory();
        return;

      case TRIGGER_ACTIVE:
      case TRIGGER_CALIB:
        WriteWorkingPackets();
        return;

      case TRIGGER_POST:
        WriteWorkingPackets();
        if (--trigger.post == 0) TriggerOff();
        return;
    }

    LogMsg("%s: unexpected switch fall through, state = %d", fid, trigger.state);
    exit(1);
}


/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: tee.c,v $
 * Revision 1.5  2015/12/09 18:43:37  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.4  2013/01/18 23:41:25  dechavez
 * don't quit when encountering gzwrite errors
 *
 * Revision 1.3  2010/03/31 20:53:02  dechavez
 * QDP_HLP_RULE_FLAG_STRICT option hardcoded
 *
 * Revision 1.2  2009/07/10 15:07:03  dechavez
 * Accept enn and ene in place of en1 and en2 when triggering
 *
 * Revision 1.1  2009/07/09 22:32:20  dechavez
 * created
 *
 */
