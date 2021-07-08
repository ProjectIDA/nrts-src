#pragma ident "$Id: read.c,v 1.4 2015/12/08 22:39:36 dechavez Exp $"
/*======================================================================
 *
 * Read and demultiplex the input stream into a linked list of
 * TIME_SERIES.  Time tears for any given ident will result in a new
 * list entry.
 *
 *====================================================================*/
#include "vhfilt.h"

#define MY_MOD_ID VHFILT_MOD_READ

static BOOL SignaturesMatch(MSEED_HDR *a, MSEED_HDR *b)
{
    if (strcmp(a->sig, b->sig) != 0) return FALSE;
    return TRUE;
}

static void StartNewTimeseries(LNKLST *list, MSEED_RECORD *record)
{
int i;
TIME_SERIES new;
static char *fid = "ReadInput:StartNewTimeseries";

    new.hdr = record->hdr;
    if ((new.dat = (INT32 *) malloc(record->hdr.nsamp * sizeof(INT32))) == NULL) {
        LogErr("ERROR: %s: malloc: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    for (i = 0; i < record->hdr.nsamp; i++) new.dat[i] = record->dat.int32[i];

    if (!listAppend(list, &new, sizeof(TIME_SERIES))) {
        LogErr("ERROR: %s: listAppend: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }
}

static void AppendTimeseries(TIME_SERIES *ts, MSEED_RECORD *record)
{
int i, j;
static char *fid = "ReadInput:AppendTimeseries";

    if ((ts->dat = (INT32 *) realloc(ts->dat, (ts->hdr.nsamp + record->hdr.nsamp) * sizeof(INT32))) == NULL) {
        LogErr("ERROR: %s: realloc: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }

    for (i = ts->hdr.nsamp, j = 0; j < record->hdr.nsamp; i++, j++) ts->dat[i] = record->dat.int32[j];
    ts->hdr.nsamp += record->hdr.nsamp;
    ts->hdr.endtime = record->hdr.endtime;
}

static TIME_SERIES *LocateContinuousTimeseries(LNKLST *list, MSEED_RECORD *record)
{
INT64 tear;
LNKLST_NODE *crnt;
TIME_SERIES *ts;
static char *fid = "LocateContinuousTimeseries";

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        ts = (TIME_SERIES *) crnt->payload;
        if (SignaturesMatch(&ts->hdr, &record->hdr)) {
            tear = mseedTimeTearInSamples(&ts->hdr, &record->hdr);
            if (tear == 0) return ts;
        }
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static BOOL IsOneSpsLHchannel(MSEED_RECORD *record)
{
	if (record->hdr.nsint != NANOSEC_PER_SEC) return FALSE;
    if (strncasecmp(record->hdr.chnid, "LH", strlen("LH")) == 0) return TRUE;
    return FALSE;
}

static void PrintTimeSeries(char *fid, LNKLST *list)
{
int i;
LNKLST_NODE *crnt;
TIME_SERIES *ts;
char path[MAXPATHLEN+1];
FILE *fp;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        ts = (TIME_SERIES *) crnt->payload;
        sprintf(path, "%s.%s", fid, ts->hdr.ident);
        for (i = 0; i < strlen(path); i++) if (path[i] == '/') path[i] = 0;
        if ((fp = fopen(path, "w")) == NULL) {
            fprintf(stderr, "fopen: ");
            perror(path);
            exit(1);
        }
        for (i = 0; i < ts->hdr.nsamp; i++) fprintf(fp, "%05d: %d\n", i, ts->dat[i]);
        fclose(fp);
        crnt = listNextNode(crnt);
    }
}

LNKLST *ReadInput(void)
{
LNKLST *list;
TIME_SERIES *ts;
MSEED_RECORD record;
static char *fid = "ReadInput";

FILE *fp;
int i, count=0;

    if ((list = listCreate()) == NULL) {
        LogErr("%s: listCreate: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 6);
    }

    while (mseedReadRecord(stdin, &record)) {
        if (!IsOneSpsLHchannel(&record)) continue;       /* Only process 1Hz LH* channels */
        if (record.hdr.format == MSEED_FORMAT_INT_32) {  /* Only accept INT32 data */
            if ((ts = LocateContinuousTimeseries(list, &record)) == NULL) {
                StartNewTimeseries(list, &record);
            } else {
                AppendTimeseries(ts, &record);
            }
        } else {
            RejectUnsupportedFormat(&record);
        }
    }

    if (!feof(stdin)) {
        LogErr("%s: mseedReadRecord: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 7);
    }

    return list;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * Revision 1.5  2021/06/21 22:37:00  dauerbach
 * Check tqual when comparing MSEED_HDR sig fields.
 *
 * Revision 1.4  2015/12/08 22:39:36  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.3  2015/11/03 15:18:15  dechavez
 * initial production release
 *
 */
