#pragma ident "$Id: record.c,v 1.10 2015/11/17 18:52:33 dechavez Exp $"
/*======================================================================
 *
 * MSEED_RECORD stuff
 *
 *====================================================================*/
#include "mseed.h"

/* Check for continuity between successive records of the same channel */

static BOOL ContinuousRecord(MSEED_HANDLE *handle, MSEED_RECORD *prev, MSEED_RECORD *crnt)
{
char tbuf[1024];
UINT64 expected;
struct { INT64 nsec; REAL64 secs; REAL64 samp; char *type;} tear; static char *gap = "gap", *overlap = "overlap";

/* it is possible for the previous record to be empty, in which case there is no comparison */

    if (prev->hdr.nsamp == 0) return FALSE;

/* transitions in time quality or any of the FSDH flags will cause a break */

    if (handle->options & MSEED_HANDLE_OPTION_FSDH_QUAL_CHECK) {
        if (crnt->hdr.tqual != prev->hdr.tqual) {
            mseedLog(handle, 2, "flushing %s: time quality change %d%% -> %d%%\n", prev->hdr.ident, prev->hdr.tqual, crnt->hdr.tqual);
            return FALSE;
        }
    }

    if (handle->options & MSEED_HANDLE_OPTION_FSDH_FLAG_CHECK) {
        if (crnt->hdr.flags.act != prev->hdr.flags.act) {
            mseedLog(handle, 2, "flushing %s: activity flag change 0x%02x -> 0x%02x\n", prev->hdr.ident, prev->hdr.flags.act, crnt->hdr.flags.act);
            return FALSE;
        }
        if (crnt->hdr.flags.ioc != prev->hdr.flags.ioc) {
            mseedLog(handle, 2, "flushing %s: I/O and clock flag change 0x%02x -> 0x%02x\n", prev->hdr.ident, prev->hdr.flags.ioc, crnt->hdr.flags.ioc);
            return FALSE;
        }
        if (crnt->hdr.flags.dat != prev->hdr.flags.dat) {
            mseedLog(handle, 2, "flushing %s: data quality flag change 0x%02x -> 0x%02x\n", prev->hdr.ident, prev->hdr.flags.dat, crnt->hdr.flags.dat);
            return FALSE;
        }
    }

/* time-tears which exceed one half sample will cause a break */

    expected = prev->hdr.endtime + prev->hdr.sint;
    if (crnt->hdr.tstamp != expected) {

        if (crnt->hdr.tstamp > expected) {
            tear.nsec = crnt->hdr.tstamp - expected;
            tear.type = gap;
        } else {
            tear.nsec = expected - crnt->hdr.tstamp;
            tear.type = overlap;
        }

        tear.samp = (REAL64) tear.nsec / (REAL64) prev->hdr.sint;
        tear.secs = (REAL64) tear.nsec / (REAL64) NANOSEC_PER_SEC;

        if (tear.samp > 0.5) {
            mseedLog(handle, 1, "flushing %s: %.3lf sec (%.3lf sample) %s\n", prev->hdr.ident, tear.secs, tear.samp, tear.type);
            mseedLog(handle, 1, "previous record: %s\n", mseedHdrString(&prev->hdr, tbuf));
            mseedLog(handle, 1, "current  record: %s\n", mseedHdrString(&crnt->hdr, tbuf));
            return FALSE;
        }
    }

    return TRUE;

}

static BOOL AppendData(MSEED_HANDLE *handle, MSEED_RECORD *old, MSEED_RECORD *new)
{
size_t SampleSize;
int used, need;
char tmpbuf[1024];
static char *fid = "mseedAddRecordToHandle:AppendData";

/* Make sure we have enough space to store the new samples */
/* Data can backup in the handle if a channel doesn't compress well.  If we
 * end up running out of space we just flush that record (will result in one
 * "short" record, but that is legal MiniSEED.
 */

    mseedLog(handle, 3, "%s: on entry old = %s\n", fid, mseedHdrString(&old->hdr, tmpbuf));
    mseedLog(handle, 3, "%s: on entry new = %s\n", fid, mseedHdrString(&new->hdr, tmpbuf));

    switch (old->hdr.format) {
      case MSEED_FORMAT_INT_16: SampleSize = sizeof(INT16); break;
      case MSEED_FORMAT_INT_32: SampleSize = sizeof(INT32); break;
      case MSEED_FORMAT_IEEE_F: SampleSize = sizeof(REAL32); break;
      case MSEED_FORMAT_IEEE_D: SampleSize = sizeof(REAL64); break;
      default:
        mseedLog(handle, -1, "%s: UNEXPECTED ERROR in file %s, line %d\n", fid,  __FILE__, __LINE__);
        mseedLog(handle, -1, "%s: unsupported data format code=%d\n", fid, old->hdr.format);
        errno = ENOTSUP;
        return FALSE;
    }
    need = SampleSize * new->hdr.nsamp;
    used = SampleSize * old->hdr.nsamp;

    if (MSEED_MAX_BUFLEN - used < need) {
        mseedLog(handle, 1, "flushing %s: handle is full", old->hdr.ident);
        if (!mseedFlushRecord(handle, old)) {
            mseedLog(handle, -1, "%s: mseedFlushRecord failed", fid);
            return FALSE;
        }
        used = SampleSize * old->hdr.nsamp;
    }

/* enough space is available, so copy over the data */

    memcpy(&old->dat.uint8[used], new->dat.uint8, need);
    mseedLog(handle, 3, "%s: copied %d bytes from new to old\n", fid, need);

/* and update the affected header fields */

    if (old->hdr.nsamp == 0) {
        mseedLog(handle, 3, "%s: RESET old with new\n", fid);
        old->hdr.nsamp = new->hdr.nsamp;
        old->hdr.tstamp = new->hdr.tstamp;
        old->hdr.flags.act = new->hdr.flags.act;
        old->hdr.flags.ioc = new->hdr.flags.ioc;
        old->hdr.flags.dat = new->hdr.flags.dat;
    } else {
        mseedLog(handle, 3, "%s: UPDATE old with new\n", fid);
        old->hdr.nsamp += new->hdr.nsamp;
        old->hdr.flags.act |= new->hdr.flags.act;
        old->hdr.flags.ioc |= new->hdr.flags.ioc;
        old->hdr.flags.dat |= new->hdr.flags.dat;
    }
    mseedSetEndtime(&old->hdr);
    mseedLog(handle, 3, "%s: on exit  old = %s\n", fid, mseedHdrString(&old->hdr, tmpbuf));

    return TRUE;
}

static MSEED_RECORD *LocateRecord(MSEED_HANDLE *handle, MSEED_RECORD *target)
{
MSEED_RECORD *entry;
LNKLST_NODE *crnt;
static char *fid = "mseedAddRecordToHandle:LocateRecord";

    crnt = listFirstNode(handle->records);
    while (crnt != NULL) {
        entry = (MSEED_RECORD *) crnt->payload;
        if (
            strcmp(entry->hdr.ident, target->hdr.ident) == 0 &&
            entry->hdr.nsint == target->hdr.nsint &&
            entry->hdr.format == target->hdr.format
        ) {
            return entry;
        }
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static MSEED_RECORD *CreateRecord(MSEED_HANDLE *handle, MSEED_RECORD *target)
{
MSEED_RECORD *record;
static char *fid = "mseedAddRecordToHandle:CreateRecord";

    if (!listAppend(handle->records, (void *) target, sizeof(MSEED_RECORD))) {
        mseedLog(handle, -1, "%s: listAppend: %s", fid, strerror(errno));
        return NULL;
    }
    if ((record = LocateRecord(handle, target)) == NULL) {
        mseedLog(handle, -1, "%s: CAN'T LOCATE MY OWN RECORD!!!", fid);
        return NULL;
    }
    mseedLog(handle, 3, "%s: create new %s record\n", fid, target->hdr.ident);
    return record;
}

BOOL mseedAddRecordToHandle(MSEED_HANDLE *handle, MSEED_RECORD *new)
{
int minsamp;
MSEED_RECORD *existing;
LNKLST_NODE *node;
static char *fid = "mseedAddRecordToHandle";

    if (handle == NULL || new == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* If there isn't already a record for this stream in the handle, add it now.  If we
 * have a record in the handle, check for continuity between it and the existing record.
 * Time tears will cause the existing record to get flushed and the new record to take
 * its place.  If the record is contiguous, then the data get appended to the existing
 * record and its header updated accordingly.
 *
 * When all is said and done, there should be a single record which may or may not have
 * sufficient data to generate one fully packed MiniSEED record.  We check and pack if
 * it is.
 */

    if ((existing = LocateRecord(handle, new)) == NULL) {
        if ((existing = CreateRecord(handle, new)) == NULL) return FALSE;
    } else {
        if (!ContinuousRecord(handle, existing, new)) {
            if (!mseedFlushRecord(handle, existing)) return FALSE; /* force flush the existing record */
            memcpy(existing, (void *) new, sizeof(MSEED_RECORD)); /* replace the existing record with the new one */
        } else {
            if (!AppendData(handle, existing, new)) return FALSE;
        }
    }

    minsamp = mseedMaxSamp(handle, &existing->hdr, FALSE);
    while (existing->hdr.nsamp >= minsamp) {
        if (!mseedPackRecord(handle, existing, FALSE)) {
            mseedLog(handle, -1, "%s: mseedPackRecord failed\n", fid);
            return FALSE;
        }
    }
    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: record.c,v $
 * Revision 1.10  2015/11/17 18:52:33  dechavez
 * modified ContinuousRecord() to check for breaks on tqual or flag transitions depending on options
 *
 * Revision 1.9  2015/09/24 22:16:15  dechavez
 * fixed serious bug in ContinuousRecord() where it could return without setting value
 *
 * Revision 1.8  2014/10/29 21:31:48  dechavez
 * removed tabs
 *
 * Revision 1.7  2014/08/26 22:56:01  dechavez
 * split record if time tears exceed 1/2 sample interval
 *
 * Revision 1.6  2014/08/26 17:37:01  dechavez
 * changed mseedLog() values for debug statements
 *
 * Revision 1.5  2014/08/26 17:28:09  dechavez
 * rework time tear logic
 *
 * Revision 1.4  2014/08/19 20:40:18  dechavez
 * removed some fflush(stdout)'s left over from a debug session
 *
 * Revision 1.3  2014/08/19 18:10:41  dechavez
 * keep packing header as long as available data exceed minsamp (major bug fix)
 *
 * Revision 1.2  2014/08/12 22:35:06  dechavez
 * removed incorrect (and misleading) test on gross difference between time stamps
 * (don't worry about it, it never got invoked... that's why its confusing)
 *
 * Revision 1.1  2014/08/11 18:18:02  dechavez
 * initial release
 *
 */
