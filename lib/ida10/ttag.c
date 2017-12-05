#pragma ident "$Id: ttag.c,v 1.33 2015/12/04 22:06:27 dechavez Exp $"
/*======================================================================
 *
 *  Time tag operations
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

#ifdef WIN32
#define NANOSEC_PER_OFISTIC 10000L
#else
#define NANOSEC_PER_OFISTIC 10000LL
#endif

/* SAN phase error in nanoseconds */

#define MAX_PHASE_ERR 51200
#define PHASE_CONVERT_FACTOR  ((REAL64) MSEC_PER_SEC / (REAL64) MAX_PHASE_ERR)

INT64 ida10SanPhaseToNanosec(UINT16 phase)
{
INT64 nanosec, sign;
REAL64 difference, factor = PHASE_CONVERT_FACTOR * NANOSEC_PER_MSEC;

    if (phase < MAX_PHASE_ERR/2) {
        difference = (REAL64) (phase);
        sign = 1;
    } else {
        difference = (REAL64) (MAX_PHASE_ERR - phase);
        sign = -1;
    }
    nanosec = sign * ((INT64) (difference * factor));

    return nanosec;
}

/* Derive the clock independent timing information */

void ida10CompleteTimeTag(IDA10_TTAG *ttag)
{
UINT64 nsec_per_tic, EpochYearFactor = 0;

    if (ttag == NULL) return;

    ttag->status.derived = FALSE;
    ttag->status.percent = -1;

    switch (ttag->type) {

      case IDA10_TIMER_SAN:
        ttag->ext = (UINT64) (ttag->san.ext * NANOSEC_PER_SEC);
        ttag->sys = (UINT64) (ttag->san.sys.sec * NANOSEC_PER_SEC) + (UINT64) (ttag->san.sys.msc * NANOSEC_PER_MSEC);
        ttag->hz  = (ttag->san.sys.sec) * NANOSEC_PER_SEC + ida10SanPhaseToNanosec(ttag->san.phase);
        ttag->status.receiver = ttag->san.status.receiver;
        ttag->status.init = ttag->san.status.init;
        ttag->status.avail = ttag->san.status.avail;
        ttag->status.suspect = ttag->san.status.suspect;
        ttag->status.locked = ttag->san.status.locked;
        break;

      case IDA10_TIMER_OFIS:
        ttag->ext = (UINT64) (ttag->ofis.ext * NANOSEC_PER_SEC);
        ttag->sys = ttag->ofis.sys * NANOSEC_PER_OFISTIC;
        ttag->hz  = ttag->ofis.pps * NANOSEC_PER_OFISTIC;
        ttag->status.receiver = ttag->ofis.status.receiver;
        ttag->status.init = ttag->ofis.status.init;
        ttag->status.avail = ttag->ofis.status.avail;
        ttag->status.suspect = ttag->ofis.status.suspect;
        ttag->status.locked = ttag->ofis.status.locked;
        break;

      case IDA10_TIMER_Q330:
#define INTERNAL_Q330_CLOCK_LOCKED_THRESHOLD  80
#define INTERNAL_Q330_CLOCK_SUSPECT_THRESHOLD 10
        ttag->sys = ttag->ext = ttag->hz =
            (UINT64) (Q330_TO_SAN_EPOCH + ttag->q330.seqno + ttag->q330.offset.sec) * NANOSEC_PER_SEC +
            (INT64) ttag->q330.offset.usec * NANOSEC_PER_USEC +
            (INT64) ttag->q330.offset.index -
            (INT64) ttag->q330.delay * NANOSEC_PER_USEC;
        ttag->status.receiver = ttag->q330.qual.bitmap;
        ttag->status.init = (ttag->q330.qual.bitmap & QDP_CQ_HAS_BEEN_LOCKED) == 0 ? FALSE : TRUE;
        ttag->status.avail = (ttag->q330.qual.bitmap & QDP_PLL_MASK) == QDP_PLL_OFF ? FALSE : TRUE;
        ttag->status.locked = (ttag->q330.qual.percent >= INTERNAL_Q330_CLOCK_LOCKED_THRESHOLD) ? TRUE : FALSE;
        ttag->status.suspect = (ttag->q330.qual.percent < INTERNAL_Q330_CLOCK_SUSPECT_THRESHOLD) ? TRUE : FALSE;
        ttag->status.percent = ttag->q330.qual.percent;
        break;

      case IDA10_TIMER_GENERIC:
        ttag->sys = ttag->ext = ttag->hz = ttag->gen.tstamp;
        ttag->status.receiver = ttag->gen.status.receiver;
        ttag->status.init = TRUE;
        ttag->status.avail = TRUE;
        ttag->status.suspect = (ttag->gen.status.generic & IDA10_GENTAG_SUSPICIOUS) ? TRUE : FALSE;
        ttag->status.locked  = (ttag->gen.status.generic & IDA10_GENTAG_LOCKED)     ? TRUE : FALSE;
        ttag->status.derived = (ttag->gen.status.generic & IDA10_GENTAG_DERIVED)    ? TRUE : FALSE;
        break;

      case IDA10_TIMER_IDA9:
        ttag->ext = (UINT64) (ttag->ida9.ext - SAN_EPOCH_TO_1970_EPOCH) * NANOSEC_PER_SEC;
        ttag->sys = ((UINT64) ttag->ida9.sys * (UINT64) ttag->ida9.mult) * NANOSEC_PER_MSEC;
        ttag->hz  = ((UINT64) ttag->ida9.hz  * (UINT64) ttag->ida9.mult) * NANOSEC_PER_MSEC;
        ttag->status.receiver = ttag->ida9.code;
        switch (ttag->ida9.qual) {
          case 1:
            ttag->status.avail   = TRUE;
            ttag->status.suspect = FALSE;
            ttag->status.locked  = TRUE;
            ttag->status.derived = FALSE;
            break;
          case 8:
          case 9:
            ttag->status.avail   = FALSE;
            ttag->status.suspect = FALSE;
            ttag->status.locked  = FALSE;
            ttag->status.derived = TRUE;
            break;
          default:
            ttag->status.avail   = TRUE;
            ttag->status.suspect = FALSE;
            ttag->status.locked  = FALSE;
            ttag->status.derived = FALSE;
            break;
        }
        EpochYearFactor = (UINT64) (ttag->ida9.year == IDA_EMBEDDED_YEAR ? 0 : year_secs(ttag->ida9.year));
        break;

      case IDA10_TIMER_OBS:
        ttag->sys = 
        ttag->ext =
        ttag->hz  =
            (UINT64) ((REAL64) NANOSEC_PER_SEC * ((REAL64) ttag->obs.sys / (REAL64) ttag->obs.ticrate)) + ttag->obs.ref;
        ttag->status.receiver = 0;
        ttag->status.init    = TRUE;
        ttag->status.avail   = TRUE;
        ttag->status.suspect = FALSE;
        ttag->status.locked  = TRUE;
        ttag->status.derived = FALSE;
        break;

      case IDA10_TIMER_OBS2:
        ttag->sys = ttag->ext = ttag->hz  =
            (UINT64) ((REAL64) NANOSEC_PER_SEC * ((REAL64) ttag->obs2.sys / (REAL64) ttag->obs2.ticrate)) + 
            (UINT64) ttag->obs2.ref * NANOSEC_PER_SEC -
            (INT64) ttag->obs2.delay * NANOSEC_PER_USEC;
        ttag->status.receiver = 0;
        ttag->status.init    = TRUE;
        ttag->status.avail   = TRUE;
        ttag->status.suspect = FALSE;
        ttag->status.locked  = TRUE;
        ttag->status.derived = FALSE;
        break;

      default:
        return;
    }

    ttag->offset = ttag->ext - ttag->hz;
    ttag->nepoch = (ttag->sys + ttag->offset) + (EpochYearFactor * NANOSEC_PER_SEC);
    ttag->epoch = (REAL64) ttag->nepoch / (REAL64) NANOSEC_PER_SEC;
}

/* Compute the end time for an IDA10 common header given a duration */

void ida10SetEndTime(IDA10_CMNHDR *cmnhdr, UINT64 duration)
{
REAL64 realdur;

    if (cmnhdr == NULL) return;
    if (cmnhdr->subformat == IDA10_SUBFORMAT_0) return; /* 10.0 includes end time */

    cmnhdr->ttag.end = cmnhdr->ttag.beg;
    cmnhdr->ttag.end.sys += duration;
    cmnhdr->ttag.end.epoch += (REAL64) duration;
    cmnhdr->end += (REAL64) duration / (REAL64) NANOSEC_PER_SEC;

    if (cmnhdr->ttag.end.type == IDA10_TIMER_OFIS) {
        cmnhdr->ttag.end.ofis.sys = cmnhdr->ttag.end.sys / NANOSEC_PER_OFISTIC;
    } else if (cmnhdr->ttag.end.type == IDA10_TIMER_SAN) {
        cmnhdr->ttag.end.san.sys.sec = cmnhdr->ttag.end.sys / NANOSEC_PER_SEC;
        cmnhdr->ttag.end.san.sys.msc = (cmnhdr->ttag.end.sys - (cmnhdr->ttag.end.san.sys.sec * NANOSEC_PER_SEC)) / NANOSEC_PER_SEC;
    }

    cmnhdr->ttag.end.derived = TRUE;
}

/* Time tag increment error in nanoseconds */

int ida10TtagIncrErr(IDA10_TSHDR *phdr, IDA10_TSHDR *chdr, int *sign, UINT64 *errptr)
{
UINT64 prev, crnt, sint, expected, actual, nsamp;
char tmp[1024];
BOOL debug = FALSE;
tmp[0] = 0;

    if (sign == NULL || errptr == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    if (chdr->nsint == 0) {
        errno = EINVAL;
        return IDA10_ESRATE;
    }

/* NULL phdr means check for intra-record errors, otherwise inter-record */

    if (phdr == NULL) {
        prev = chdr->cmn.ttag.beg.sys;
        crnt = chdr->cmn.ttag.end.sys;
        nsamp = (UINT64) chdr->nsamp;
        if (debug) {
            sprintf(tmp+strlen(tmp), "intra-record check, prev=%llu, crnt=%llu, nsamp=%llu\n", prev, crnt, nsamp);
        }
    } else {
        if (chdr->sint != phdr->sint) {
            errno = EINVAL;
            return IDA10_ESRATE;
        }
        prev = phdr->cmn.ttag.end.sys;
        crnt = chdr->cmn.ttag.beg.sys;
        nsamp = (UINT64) 2;
        if (debug) {
            sprintf(tmp+strlen(tmp), "inter-record check, prev=%llu, crnt=%llu, nsamp=%llu\n", prev, crnt, nsamp);
        }
    }

/* Expected time increment */

    sint = (UINT64) (chdr->sint * (REAL64) NANOSEC_PER_SEC);
    expected = ((nsamp - 1) * sint);
    if (debug) {
        sprintf(tmp+strlen(tmp), "nsamp = %llu, sint in nanoseconds = %llu, expected increment = %llu\n", nsamp, sint, expected);
    }

/* Actual time increment */

    if (crnt > prev) {
        actual = crnt - prev;
        if (debug) {
            sprintf(tmp+strlen(tmp), "crnt = %llu, prev = %llu, actual increment = %llu\n", crnt, prev, actual);
        }
        if (actual == expected) {
            *errptr = 0;
            *sign   = 0;
        } else if (actual > expected) {
            *errptr = actual - expected;
            *sign   = 1;
        } else {
            *errptr = expected - actual;
            *sign   = -1;
        }
    } else {
        actual = prev - crnt;
        if (debug) {
            sprintf(tmp+strlen(tmp), "crnt = %llu, prev = %llu, actual increment = %llu\n", crnt, prev, actual);
        }
        *errptr = actual + expected;
        *sign   = -1;
    }

/* save the results in the header */

    chdr->incerr.nsec = *errptr;
    chdr->incerr.samp =  chdr->incerr.nsec / chdr->nsint;
    if (chdr->incerr.nsec != 0) {
        chdr->incerr.ok = FALSE;
        chdr->incerr.nsec *= *sign;
    } else {
        chdr->incerr.ok = TRUE;
    }

    if (debug) {
        sprintf(tmp+strlen(tmp), "sign = %d, error = %llu\n", *sign, *errptr);
        if (*errptr != 0) printf("\nINCR ERR ****\n%s", tmp);
    }

/* check for fresh clock strings */

    if (chdr->cmn.ttag.end.status.init) {
        if (!chdr->cmn.ttag.beg.status.init || !phdr->cmn.ttag.end.status.init) {
            chdr->incerr.fresh = TRUE;
        } else {
            chdr->incerr.fresh = FALSE;
        }
    } else {
        chdr->incerr.fresh = FALSE;
    }

    return IDA10_OK;
}

/* Come up with a locked/unlocked value for a given time tag */

int ida10ClockQuality(IDA10_TTAG *ttag)
{
    if (ttag == NULL) return -2;
    if (ttag->status.init || ttag->status.suspect) return -2;
    if (!ttag->status.avail) return 8;

    return ttag->status.locked ? 1 : 6;

}

static void PrintHeader(FILE *fp, int type)
{
static char *SanHeader =
"#                                                            "
"10-15 9  67   5   4   3   2   1   0\n"
"#   Recno Stream  Ext Time --- System ----   PLL Phase Status"
"=RCVR ? TYPE RTC VLD LKD ETA PLL PPS | OffDiff  Comments";
static char *OfisHeader =
"#   Recno Stream  Ext Time ---- PPS Time ---- ---- Sys Time ----  "
"Time of First Sample    Host Tstamp       Seqno   Status="
"RCVR ? VLD LKD ETA LPY PPS | OffDiff  Comments";
static char *Q330Header =
"#   Recno Stream     Seqno    off.sec   off.usec  off.index      delay    loss  qual  "
"RCVR lock filt start hold track";
static char *GenericHeader =
"#   Recno Stream             Tstamp                         Vqual Gqual = ? LKD VLD";
static char *OBSHeader =
"#   Recno Stream       IMEI        MOMSN     OBS sync time      Timestamp (tics)  Timestamp (converted)    SBD tstamp       Receipt Time";
static char *OBS2Header =
"#   Recno Stream       IMEI        MOMSN     OBS sync time     Delay(sec) Timestamp (tics)  Timestamp (converted)    SBD tstamp       Receipt Time";

    switch (type) {
      case IDA10_TIMER_SAN:     fprintf(fp, "%s\n",  SanHeader); break;
      case IDA10_TIMER_Q330:    fprintf(fp, "%s\n", Q330Header); break;
      case IDA10_TIMER_OFIS:    fprintf(fp, "%s\n", OfisHeader); break;
      case IDA10_TIMER_OBS:     fprintf(fp, "%s\n", OBSHeader);  break;
      case IDA10_TIMER_OBS2:    fprintf(fp, "%s\n", OBS2Header);  break;
      case IDA10_TIMER_GENERIC: fprintf(fp, "%s\n", GenericHeader); break;
    default:
        fprintf(fp, " need to add support for ttag->type %d in PrintHeader, %s line %d", type, __FILE__, __LINE__);
    }
    return;
}

static void PrintTtag(FILE *fp, IDA10_TTAG *ttag, IDA10_TTAG_HISTORY *history, IDA_EXTRA *extra)
{
char tmpbuf[256];
INT64 offdiff;

#define ERROR_EXT_STUCK 0x01

    history->flags = 0;

    if (history->count == 1) {
        offdiff = 0;
    } else {
        if (history->ttag.ext == ttag->ext && history->ttag.hz != ttag->hz) history->flags |= ERROR_EXT_STUCK;
        offdiff = ttag->offset - history->ttag.offset;
    }

    switch (ttag->type) {
      case IDA10_TIMER_SAN:
        fprintf(fp, "%10u ", ttag->san.ext);
        fprintf(fp, "%11u.%03hd ", ttag->san.sys.sec, ttag->san.sys.msc);
        fprintf(fp, "%5hu ", ttag->san.pll);
        fprintf(fp, "%5hu ", ttag->san.phase);
        fprintf(fp, "0x%04x ", ttag->san.status.raw);
        fprintf(fp, "0x%02x ", ttag->san.status.receiver);
        fprintf(fp, "%s ", ttag->san.status.suspect ? "S" : "n");
        switch (ttag->san.status.type) {
          case SANIO_TT_TYPE_NMEA: fprintf(fp, "nmea "); break;
          case SANIO_TT_TYPE_TSIP: fprintf(fp, "tsip "); break;
          default: fprintf(fp, "0x%02x ", ttag->san.status.type); break;
        }
        fprintf(fp, "%s ", ttag->san.status.rtt_fail ? "ERR" : " ok");
        fprintf(fp, "%s ", ttag->san.status.init ? "yes" : " no");
        fprintf(fp, "%s ", ttag->san.status.locked ? "yes" : " no");
        fprintf(fp, "%s ", ttag->san.status.avail ? "yes" : " no");
        fprintf(fp, "%s ", ttag->san.status.pll ? "yes" : " no");
        fprintf(fp, "%s ", ttag->san.status.pps ? "yes" : " no");
        fprintf(fp, "%8lld", offdiff);
        break;

      case IDA10_TIMER_OFIS:
        offdiff /= NANOSEC_PER_OFISTIC;
        fprintf(fp, " %9u ", ttag->ofis.ext);
        fprintf(fp, "%18llu ", ttag->ofis.pps);
        fprintf(fp, "%18llu ", ttag->ofis.sys);
        fprintf(fp, "%s ", utilDttostr(ttag->epoch+SAN_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
        if (extra != NULL) {
            fprintf(fp, "%s ", utilLttostr(extra->tstamp+SAN_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
            fprintf(fp, "0x%08x ", extra->seqno);
        } else {
            fprintf(fp, "                             ");
        }
        fprintf(fp, "0x%04x ", ttag->ofis.status.raw);
        fprintf(fp, "0x%02x ", ttag->ofis.status.receiver);
        fprintf(fp, "%s ", ttag->ofis.status.suspect ? "S" : "n");
        fprintf(fp, "%s ", ttag->ofis.status.init ? "yes" : " no");
        fprintf(fp, "%s ", ttag->ofis.status.locked ? "yes" : " no");
        fprintf(fp, "%s ", ttag->ofis.status.avail ? "yes" : " no");
        fprintf(fp, "%s ", ttag->ofis.status.leapsec ? "yes" : " no");
        fprintf(fp, "%s ", ttag->ofis.status.pps ? "yes" : " no");
        fprintf(fp, "%9lld", offdiff);
        break;

      case IDA10_TIMER_Q330:
        fprintf(fp, " %9u ", ttag->q330.seqno);
        fprintf(fp, " %9d ", ttag->q330.offset.sec);
        fprintf(fp, " %9d ", ttag->q330.offset.usec);
        fprintf(fp, " %9d ", ttag->q330.offset.index);
        fprintf(fp, " %9d ", ttag->q330.delay);
        fprintf(fp, " %6hu ", ttag->q330.loss);
        fprintf(fp, " %3d%% ", ttag->q330.qual.percent);
        fprintf(fp, " 0x%02x ", ttag->q330.qual.bitmap);
        break;

      case IDA10_TIMER_OBS:
        fprintf(fp, " %16llu", ttag->obs.sbd.imei);
        fprintf(fp, " 0x%04x",   ttag->obs.sbd.momsn);
        fprintf(fp, " %s",   utilTimeString(ttag->obs.ref, 1000, tmpbuf, 256));
        fprintf(fp, " 0x%016llx", ttag->obs.sys);
        fprintf(fp, " %s",   utilTimeString(ttag->ext, 1000, tmpbuf, 256));
        fprintf(fp, " %s",   utilLttostr(ttag->obs.sbd.tstamp, 0, tmpbuf));
        if (extra != NULL) {
            fprintf(fp, " %s", utilLttostr(extra->tstamp+SAN_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
        } else {
            fprintf(fp, "                 ");
        }
        break;

      case IDA10_TIMER_OBS2:
        fprintf(fp, " %16llu", ttag->obs2.sbd.imei);
        fprintf(fp, " 0x%04x",   ttag->obs2.sbd.momsn);
        fprintf(fp, " %s",   utilTimeString((UINT64) ttag->obs2.ref * NANOSEC_PER_SEC, 1000, tmpbuf, 256));
        fprintf(fp, " %9.6f", (REAL32) ttag->obs2.delay / (REAL32) USEC_PER_SEC);
        fprintf(fp, " 0x%016llx", ttag->obs2.sys);
        fprintf(fp, " %s",   utilTimeString(ttag->ext, 1000, tmpbuf, 256));
        fprintf(fp, " %s",   utilLttostr(ttag->obs2.sbd.tstamp, 0, tmpbuf));
        if (extra != NULL) {
            fprintf(fp, " %s", utilLttostr(extra->tstamp+SAN_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
        } else {
            fprintf(fp, "                 ");
        }
        break;

      default:
        fprintf(fp, " need to add support for ttag->type %d in PrintTtag, %s line %d", ttag->type, __FILE__, __LINE__);
    }

/* comments */

    if (history->flags & ERROR_EXT_STUCK) fprintf(fp, ", GPS stuck");
    fprintf(fp, "\n");

/* save this ttag for comparison next time around */

    history->ttag = *ttag;
}

void ida10PrintTSttag(FILE *fp, IDA10_TS *ts, IDA10_TTAG_HISTORY *history)
{
IDA10_TTAG *ttag;
IDA_EXTRA *extra;

    if (fp == NULL || ts == NULL || history == NULL) return;

    ttag = &ts->hdr.cmn.ttag.beg;
    if (history->count++ % 25 == 0) PrintHeader(fp, ttag->type);
    fprintf(fp, "%9llu", history->count);
    fprintf(fp, "%7s", ts->hdr.cname);
    extra = ts->hdr.cmn.extra.valid ? &ts->hdr.cmn.extra : NULL;
    PrintTtag(fp, ttag, history, extra);
    fflush(fp);
}

void ida10InitTtagHistory(IDA10_TTAG_HISTORY *history)
{
    history->count = 0;
}

/* Revision History
 *
 * $Log: ttag.c,v $
 * Revision 1.33  2015/12/04 22:06:27  dechavez
 * casts and format fixes to calm OS X compiles
 *
 * Revision 1.32  2014/08/26 23:44:07  dechavez
 * ida10TtagIncrErr() reports error for all time tears (earlier Q330 tears
 * were tolerated if they were less than one sample)
 *
 * Revision 1.31  2014/08/26 17:09:52  dechavez
 * added nepoch support to ida10CompleteTimeTag
 *
 * Revision 1.30  2014/04/30 15:45:54  dechavez
 * for Q330s, set IDA10_CLOCK_STATUS percent to Q330 clock quality
 *
 * Revision 1.29  2014/04/14 18:33:02  dechavez
 * set locked and suspect generic clock status for Q330 based on q330.qual.percentage
 *
 * Revision 1.28  2013/09/20 16:12:44  dechavez
 * added IDA10_TIMER_OBS2 support to PrintHeader()
 *
 * Revision 1.27  2013/07/19 17:50:25  dechavez
 * IDA10_OBSTAG2 and IDA10.11 support
 *
 * Revision 1.26  2013/05/14 21:06:51  dechavez
 * fixed formatting typo when printing OBS sys time
 *
 * Revision 1.25  2013/05/11 22:51:43  dechavez
 * IDA10.10 support
 *
 * Revision 1.24  2012/01/11 21:19:46  dechavez
 * added human readable tofs, host tstamp, and seqno when printing OFIS ttag
 *
 * Revision 1.23  2011/07/01 18:48:51  dechavez
 * set IDA10_TTAG ext and hz fields from Q330 and GENERIC time tags
 * to be equal to sys field (instead of 0 which is equivalent, but which
 * caused some other programs to complain).
 *
 * Revision 1.22  2010/09/30 17:42:05  dechavez
 * checkpoint with initial support of IDA9 included, but ifdef'd out
 *
 * Revision 1.21  2010/05/10 16:19:46  dechavez
 * avoid division by zero error when a bad header results in nsint == 0
 *
 * Revision 1.20  2008/08/20 18:11:58  dechavez
 * set new "fresh" bit in generic ttag statu
 *
 * Revision 1.19  2008/04/03 17:19:32  dechavez
 * fixed core dumping typo when printing usupported time tags (IDA10_TIMER_GENERIC in this case,
 * which are still unsupported)
 *
 * Revision 1.18  2007/12/14 21:36:58  dechavez
 * IDA10.5 support
 *
 * Revision 1.17  2007/06/21 22:46:42  dechavez
 * added support for 'derived' time status field
 *
 * Revision 1.16  2007/05/11 16:17:47  dechavez
 * fixed Q330 filter delay compensation
 *
 * Revision 1.15  2007/03/06 18:36:31  dechavez
 * fixed bug printing offset in PrintTtag()
 *
 * Revision 1.14  2007/02/20 02:18:54  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.13  2007/01/04 17:55:24  dechavez
 * made ida10TtagIncrErr debug messages a compile time option,
 * added support for incerr struct in TS header
 *
 * Revision 1.12  2006/12/22 02:46:59  dechavez
 * initial release
 *
 * Revision 1.11  2006/12/08 17:27:31  dechavez
 * preliminary 10.4 support
 *
 * Revision 1.10  2006/11/10 06:10:59  dechavez
 * replaced ida10PhaseToUsec() with ida10SanPhaseToNanosec()
 * added ida10CompleteTimeTag(), ida10SetEndTime(), ida10ClockQuality(),
 * ida10CheckTimeStatus(), ida10InitTtagHistory(), ida10PrintTSttag()
 * eliminated ida10Offset(), ida101HzTime(), ida10SystemTime()
 * reworked ida10TtagIncrErr() to use generic time tag
 *
 * Revision 1.9  2006/08/15 00:53:27  dechavez
 * added OFIS ttag support
 *
 * Revision 1.8  2005/05/25 22:38:06  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.7  2004/12/10 17:43:38  dechavez
 * added ida10SystemTime(), ida10PhaseToUsec(), modified ida101HzTime() to use latter
 *
 * Revision 1.6  2004/06/24 18:18:40  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.5  2002/05/15 20:16:51  dec
 * replaced ida10GetOffset() with ida10Offset, added ida101HzTime()
 * and ida10CheckTimeStatus()
 *
 * Revision 1.4  2002/05/15 18:25:09  dec
 * added ida10GetOffset()
 *
 * Revision 1.3  2002/01/25 19:13:02  dec
 * return symbolic values instead of explicit numbers
 *
 * Revision 1.2  2000/02/08 23:26:54  dec
 * ifdef'd dummy SunOS version (lacks long long)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:26  dec
 * import existing IDA/NRTS sources
 *
 */
