#pragma ident "$Id: process.c,v 1.2 2015/10/30 22:28:08 dechavez Exp $"
/*======================================================================
 *
 * Process a single TIME_SERIES
 *
 *====================================================================*/
#include "vhfilt.h"

#define MY_MOD_ID VHFILT_MOD_PROCESS

static void AdjustStartTime(TIME_SERIES *ts, VHFILT *vhfilt)
{
REAL64 tstamp, t1, t2, dt, delay;

    tstamp = utilConvertFrom1999NsecTo1970Secs(ts->hdr.tstamp);
    delay = vhfilt->delay / NANOSEC_PER_SEC;

    t1 = floor(tstamp + delay + 0.5);
    t2 = 10 * floor(t1 / 10);
    dt = 10 - floor(t1 - t2);

    if (dt < 9.5) {
        ts->skip = (INT32) dt;
        ts->nice = utilConvertFrom1970SecsTo1999Nsec(tstamp + dt);
    } else {
        ts->skip = 0;
        ts->nice = ts->hdr.tstamp;
    }

    ts->nice += vhfilt->delay;
}

static REAL64 *AllocateBuffer(TIME_SERIES *ts)
{
REAL64 *result;
static char *fid = "Process:AllocateBuffer";

    if ((result = (REAL64 *) malloc(ts->hdr.nsamp * sizeof(REAL64))) == NULL) {
        LogErr("ERROR: %s: malloc: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
    return result;
}

static int ApplyFilter(PASS *pass, REAL64 *input, int ninput, REAL64 *output)
{
#define GAIN 2.0 /* don't know why this is here */
FILTER_COEFF *cf;
REAL64 sum;
int i, j, n, count = 0;

    cf = &pass->filter.data.cf;
    n = ninput - cf->ncoef - 1;

    for (i = 0; i < n; i += pass->decimation) {
        for (sum = 0.0, j = 0; j < cf->ncoef; j++) sum += input[i+j] * cf->coef[j];
        output[count++] = sum * GAIN;
    }

    return count;
}

void Process(TIME_SERIES *ts, VHFILT *vhfilt, FILE *fp)
{
int i, j, nsamp, decimation;
REAL64 *buf1, *buf2;

    if (ts->hdr.nsamp < vhfilt->minsamp) {
        LogMsg(0, "ignore %d sample %s stream (minsamp = %d)\n", ts->hdr.sig, ts->hdr.nsamp, vhfilt->minsamp);
        return;
    }

/* Allocate work space */

    buf1 = AllocateBuffer(ts);
    buf2 = AllocateBuffer(ts);

/* Copy input data to first workspace */

    AdjustStartTime(ts, vhfilt);
    for (i = ts->skip, j = 0; i < ts->hdr.nsamp; i++, j++) buf1[j] = (REAL64) ts->dat[i];

/* Apply first filter, leaving results in buf2 */

    nsamp = ApplyFilter(&vhfilt->pass[0], buf1, ts->hdr.nsamp - ts->skip, buf2);

/* Apply second filter, leaving results in buf1*/

    nsamp = ApplyFilter(&vhfilt->pass[1], buf2, nsamp, buf1);

/* Overwrite input TIME_SERIES with new results */

    decimation = vhfilt->pass[0].decimation * vhfilt->pass[1].decimation;
    ts->hdr.nsamp = nsamp;
    ts->hdr.nsint *= decimation;
    ts->hdr.asint *= decimation;
    ts->hdr.sint  *= decimation;
    ts->hdr.chnid[0] = 'V';
    ts->hdr.tstamp = ts->nice;
    mseedSetEndtime(&ts->hdr);

    for (i = 0; i < nsamp; i++) ts->dat[i] = (INT32) buf1[i];

/* Free work space */

    free(buf1);
    free(buf2);

/* Convert TIME_SERIES into a set of MiniSEED packets */

    OutputData(fp, ts);
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
 * $Log: process.c,v $
 * Revision 1.2  2015/10/30 22:28:08  dechavez
 * initial production release
 *
 */
