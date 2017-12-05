#pragma ident "$Id: handle.c,v 1.3 2015/11/17 18:50:58 dechavez Exp $"
/*======================================================================
 * 
 * handle stuff
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedSetOptions(MSEED_HANDLE *handle, INT32 value)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    handle->options = value;

    return TRUE;
}

BOOL mseedSetDebug(MSEED_HANDLE *handle, int value)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    handle->debug = value;

    return TRUE;
}

BOOL mseedSetClockThreshold(MSEED_HANDLE *handle, int locked, int suspect)
{
    if (handle == NULL || locked < 0 || locked > 100 || suspect < 0 || suspect > 100 || locked < suspect) {
        errno = EINVAL;
        return FALSE;
    }

    handle->thresh.clock.locked = locked;
    handle->thresh.clock.suspect = suspect;

    return TRUE;
}

BOOL mseedSetStaid(MSEED_HANDLE *handle, char *staid)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (handle->staid != NULL) {
        free(handle->staid);
        handle->staid = NULL;
    }

    if (staid != NULL) {
        if ((handle->staid = strdup(staid)) == NULL) return FALSE;
        handle->staid[MSEED_SNAMLEN] = 0; /* ensure legal length */
        util_ucase(handle->staid); /* silently force upper case station names */
    }

    return TRUE;
}

BOOL mseedSetNetid(MSEED_HANDLE *handle, char *netid)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (handle->netid != NULL) {
        free(handle->netid);
        handle->netid = NULL;
    }

    if (netid != NULL) {
        if ((handle->netid = strdup(netid)) == NULL) return FALSE;
        handle->netid[MSEED_NNAMLEN] = 0; /* ensure legal length */
        util_ucase(handle->netid); /* silently force upper case network codes */
    }

    return TRUE;
}

BOOL mseedSetDriftRate(MSEED_HANDLE *handle, REAL64 drift)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    handle->drift = drift;

    return TRUE;
}

BOOL mseedSetIDA10Chanmap(MSEED_HANDLE *handle, LNKLST *chanmap)
{
    if (handle == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    handle->rename.ida10 = chanmap;

    return TRUE;
}

static INT32 IncrementSeqno(INT32 seqno)
{
    if (seqno == MSEED_MAX_FSDH_SEQNO) seqno = 0;
    return ++seqno;
}

INT32 mseedIncrementSeqno(MSEED_HANDLE *handle, char *ident)
{
INT32 seqno;
MSEED_DATUM *datum;
static char *fid = "mseedIncrementSeqno";

    if (handle == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (handle->seqno != -1) {
        seqno = handle->seqno = IncrementSeqno(handle->seqno);
    } else {
        if ((datum = mseedGetHistory(handle, NULL, ident)) == NULL) {
            mseedLog(handle, -1, "%s: mseedGetHistory(%s): %s", fid, ident, strerror(errno));
            return -1;
        }
        seqno = datum->value = IncrementSeqno(datum->value);
    }

    return seqno;
}

static void DestroyRenameMaps(MSEED_NAME_MAP *rename)
{
    if (rename->ida10 != NULL) listDestroy(rename->ida10);
}

MSEED_HANDLE *mseedDestroyHandle(MSEED_HANDLE *handle)
{
LNKLST_NODE *crnt;

    if (handle == NULL) return NULL;
    if (handle->staid != NULL) free(handle->staid);
    if (handle->netid != NULL) free(handle->netid);
    if (handle->records != NULL) listDestroy(handle->records);
    if (handle->history.data != NULL) listDestroy(handle->history.data);
    if (handle->history.seqno != NULL) listDestroy(handle->history.seqno);
    DestroyRenameMaps(&handle->rename);
    free(handle);
    return (MSEED_HANDLE *) NULL;
}

static void InitRenameMaps(MSEED_NAME_MAP *rename)
{
    rename->ida10 = NULL;
}

static MSEED_HANDLE *InitHandle(MSEED_HANDLE *handle, LOGIO *lp, int reclen, char qc, int seqno, MSEED_SAVE_FUNC func, void *argptr)
{
    if (handle == NULL) return NULL;

    handle->lp = lp;

    /* Set all the pointers to NULL so that mseedDestroyHandle will behave OK in case of failures */

    handle->staid = handle->netid = NULL;
    InitRenameMaps(&handle->rename);

    if ((handle->records = listCreate()) == NULL) return mseedDestroyHandle(handle);
    if ((handle->history.data = listCreate()) == NULL) return mseedDestroyHandle(handle);
    if ((handle->history.seqno = listCreate()) == NULL) return mseedDestroyHandle(handle);

    handle->reclen = reclen;
    handle->qc = qc;
    handle->seqno = seqno;
    handle->func = func;
    handle->format = MSEED_FORMAT_UNDEF;
    handle->drift = MSEED_UNDEFINED_DRIFT;
    handle->argptr = argptr;
    handle->debug = 0;
    handle->options = MSEED_DEFAULT_HANDLE_OPTIONS;

    mseedSetClockThreshold(handle, MSEED_DEFAULT_CLOCK_LOCKED_PERCENT, MSEED_DEFAULT_CLOCK_SUSPECT_PERCENT);

    return handle;
}

MSEED_HANDLE *mseedCreateHandle(LOGIO *lp, int reclen, char qc, int seqno, MSEED_SAVE_FUNC func, void *argptr)
{
MSEED_HANDLE *handle;
static char *fid = "mseedCreateHandle";

/* Use 0 to invoke the default sizes */

    if (reclen == 0) reclen = MSEED_DEFAULT_RECLEN;

/* Ensure record length is within the limits */

    if (reclen < MSEED_MIN_RECLEN || reclen > MSEED_MAX_RECLEN) {
        errno = EINVAL;
        return NULL;
    }

/* And that it is a power of 2 */

    if (util_powerof(reclen, 2) == -1) {
        errno = EINVAL;
        return NULL;
    }

/* sequence number sanity check (negative seqno implies per-channel numbering) */

    if (seqno > MSEED_MAX_FSDH_SEQNO) {
        errno = EINVAL;
        return NULL;
    }

/* FSDH quality code sanity check */

    if (!mseedValidQC(qc)) {
        errno = EINVAL;
        return NULL;
    }

/* Create and initialize */

    if ((handle = (MSEED_HANDLE *) malloc(sizeof(MSEED_HANDLE))) == NULL) return NULL;

    return InitHandle(handle, lp, reclen, qc, seqno, func, argptr);
}

/* Flush all data in a record */

BOOL mseedFlushRecord(MSEED_HANDLE *handle, MSEED_RECORD *record)
{
char tmpbuf[1024];
static char *fid = "mseedFlushRecord";

    if (handle == NULL || record == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    while (record->hdr.nsamp > 0) {
        mseedLog(handle, 3, "%s: flush %s", fid, mseedHdrString(&record->hdr, tmpbuf));
        if (!mseedPackRecord(handle, record, TRUE)) {
            mseedLog(handle, -1, "%s: mseedPackRecord failed\n", fid);
            return FALSE;
        }
    }

    return TRUE;
}

/* Flush all the data in a handle */

BOOL mseedFlushHandle(MSEED_HANDLE *handle)
{
LNKLST_NODE *crnt;
MSEED_RECORD *record;
static char *fid = "mseedFlushHandle";

    crnt = listFirstNode(handle->records);
    while (crnt != NULL) {
        record = (MSEED_RECORD *) crnt->payload;
        if (!mseedFlushRecord(handle, record)) {
            mseedLog(handle, -1, "%s: mseedFlushRecord failed\n", fid);
            return FALSE;
        }
        crnt = listDestroyNode(crnt);
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
 * $Log: handle.c,v $
 * Revision 1.3  2015/11/17 18:50:58  dechavez
 * added mseedSetOptions(), initialze (new) options field in InitHandle()
 *
 * Revision 1.2  2014/08/19 18:09:06  dechavez
 * added QC sanity check, removed log messages where handle was still unitialized
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
