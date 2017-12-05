#pragma ident "$Id: output.c,v 1.5 2015/11/17 18:55:18 dechavez Exp $"
/*======================================================================
 *
 * Convert TIME_SERIES into a sequence of MiniSeed packets written to
 * the provided FILE
 *
 *====================================================================*/
#include "vhfilt.h"

#define MY_MOD_ID VHFILT_MOD_OUTPUT

static int RecordLength = 0;

static void WriteMiniSeed(void *argptr, MSEED_PACKED *packed)
{
FILE *fp;
static char *fid = "OuputData:WriteMiniSeed";

int i;
MSEED_RECORD record;
if (!mseedUnpackRecord(&record, packed->data)) fprintf(stderr, "%s: mseedUnpackRecord: %s\n", fid, strerror(errno));

    fp = (FILE *) argptr;
    if (fwrite(packed->data, sizeof(UINT8), packed->len, fp) != packed->len) {
        LogErr("%s: fwrite: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
    free(packed);
}

void OutputData(FILE *fp, TIME_SERIES *ts)
{
int i;
LOGIO *lp;
INT32 *pdata;
INT32 maxcopy, ncopy, nbyte, remain;
MSEED_HANDLE *handle;
MSEED_RECORD record;
static char *fid = "OuputData";

    lp = LogHandle();
    if ((handle = mseedCreateHandle(LogHandle(), ts->hdr.reclen, ts->hdr.flags.qc, -1, WriteMiniSeed, (void *) fp)) == NULL) {
        LogErr("%s: mseedCreateHandle: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    if (!mseedSetNetid(handle, ts->hdr.netid)) {
        LogErr("%s: mseedSetNetid: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    if (!mseedSetStaid(handle, ts->hdr.staid)) {
        LogErr("%s: mseedSetStaid: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }

    if (!mseedSetOptions(handle, MSEED_HANDLE_OPTION_NONE)) { /* to avoid segmenting output */
        LogErr("%s: mseedSetStaid: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
	}

    record.hdr = ts->hdr;
    remain = ts->hdr.nsamp;
    pdata = ts->dat;
    maxcopy = MSEED_MAX_RECLEN / sizeof(INT32);

    while (remain) {
        ncopy = (remain > maxcopy) ? maxcopy : remain;
        for (i = 0; i < ncopy; i++) record.dat.int32[i] = *pdata++;
        record.hdr.nsamp = ncopy;
        mseedSetEndtime(&record.hdr);

        if (!mseedAddRecordToHandle(handle, &record)) {
            LogErr("%s: mseedAddRecordToHandle: %s\n", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 6);
        }

        remain -= ncopy;
        record.hdr.tstamp += ((INT64) ncopy * record.hdr.sint);
    }

    if (!mseedFlushHandle(handle)) {
        LogErr("%s: mseedFlushHandle: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 7);
    }
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
 * $Log: output.c,v $
 * Revision 1.5  2015/11/17 18:55:18  dechavez
 * removed dangling debug output
 *
 * Revision 1.4  2015/11/17 18:54:34  dechavez
 * set handle options to supress splitting on tqual or flag transitions
 *
 * Revision 1.3  2015/10/30 22:29:34  dechavez
 * initial production release
 *
 */
