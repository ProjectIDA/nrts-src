#pragma ident "$Id: util.c,v 1.12 2017/11/21 18:06:12 dechavez Exp $"
/*======================================================================
 *
 *  miscellaneous utilities
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedSignaturesMatch(MSEED_HDR *a, MSEED_HDR *b)
{
    if (strcmp(a->sig, b->sig) == 0) return TRUE;
    return FALSE;
}

INT64 mseedTimeTearInSamples(MSEED_HDR *a, MSEED_HDR *b)
{
struct {
    INT64 nsec;
    INT64 nsmp;
} tear;

    tear.nsec = (b->tstamp - a->endtime) - a->sint;
    tear.nsmp = (INT64) ((REAL64) tear.nsec / (REAL64) a->sint);

    return tear.nsmp;
}

char *mseedCSSDatatypeString(MSEED_HDR *hdr)
{
static char *s2 = "s2"; static char *i2 = "i2";
static char *s4 = "s4"; static char *i4 = "i4";
static char *t4 = "t4"; static char *f4 = "f4";
static char *t8 = "t8"; static char *f8 = "f8";

    if (hdr->order == BIG_ENDIAN_BYTE_ORDER) {
        switch (hdr->format) {
          case MSEED_FORMAT_INT_16: return s2;
          case MSEED_FORMAT_INT_32: return s4;
          case MSEED_FORMAT_IEEE_F: return t4;
          case MSEED_FORMAT_IEEE_D: return t8;
        }
    } else {
        switch (hdr->format) {
          case MSEED_FORMAT_INT_16: return i2;
          case MSEED_FORMAT_INT_32: return i4;
          case MSEED_FORMAT_IEEE_F: return f4;
          case MSEED_FORMAT_IEEE_D: return f8;
        }
    }
    return NULL;
}

char *mseedChnloc(MSEED_HDR *hdr, char *buf)
{
int i;
#define UNDERSCORE '_'
static char mt_unsafe[MSEED_CNAMLEN + MSEED_LNAMLEN + 1];

    if (buf == NULL) buf = mt_unsafe;

    sprintf(buf, "%s%s", hdr->chnid, hdr->locid);
    utilTrimString(buf);
    for (i = 0; i < strlen(buf); i++) if (isspace(buf[i]) || !isprint(buf[i])) buf[i] = UNDERSCORE;

    return buf;
}

char mseedStageType(FILTER *filter)
{
    if (filter == NULL) {
        errno = EINVAL;
        return MSEED_STAGE_TYPE_NULL;
    }

    switch (filter->type) {
      case FILTER_TYPE_ANALOG:   return MSEED_STAGE_TYPE_HERTZ;
      case FILTER_TYPE_LAPLACE:  return MSEED_STAGE_TYPE_RADSEC;
    }

    return MSEED_STAGE_TYPE_DIGITAL;
}

BOOL mseedValidQC(char qc)
{
    switch (qc) {
      case MSEED_QC_UNDEFINED:
      case MSEED_QC_RAW:
      case MSEED_QC_VERIFIED:
      case MSEED_QC_MODIFIED:
        return TRUE;
    }

    return FALSE;
}

#ifndef NANOSEC_PER_SEC
#define NANOSEC_PER_SEC 1000000000000LL
#endif /* !NANOSEC_PER_SEC */

UINT64 mseedFactMultToNsint(int fact, int mult)
{
UINT64 result;

    if (fact == 0 || mult == 0) return 0;

    if (fact > 0 && mult > 0) {
        result = NANOSEC_PER_SEC / fact * mult;
    } else if (fact > 0 && mult < 0) {
        result = -mult * NANOSEC_PER_SEC / fact;
    } else if (fact < 0 && mult > 0) {
        result = -fact * NANOSEC_PER_SEC / mult;
    } else { // (fact < 0 && mult < 0)
        result = -fact * NANOSEC_PER_SEC / -mult;
    }

    return result;
}

void mseedNsintToFactMult(UINT64 nsint, int *fact, int *mult)
{
REAL64 rsint;

    *mult = 1;
    rsint = (REAL64) nsint / NANOSEC_PER_SEC;
    *fact = (nsint > NANOSEC_PER_SEC) ? (int) -rsint : (int) rint((1.0 / rsint));
}

/* Set the trace ident assuming that all required fields are defined */
/* DO NOT CHANGE THE FORMAT OF THE IDENT... it is used as-is as the record ident for feeding SeedLink */

char *mseedSetIdent(MSEED_HDR *hdr)
{
    if (hdr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    hdr->staid[MSEED_SNAMLEN] = 0;
    hdr->chnid[MSEED_CNAMLEN] = 0;
    hdr->locid[MSEED_LNAMLEN] = 0;
    hdr->netid[MSEED_NNAMLEN] = 0;

    sprintf(hdr->ident, "%s_%s_%s_%s/MSEED", hdr->netid, hdr->staid, hdr->locid, hdr->chnid);
    sprintf(hdr->sig, "%s_%02X_%016llX", hdr->ident, hdr->format, hdr->sint);

    return hdr->ident;
}

/* Figure out the time of last sample in a record */

void mseedSetEndtime(MSEED_HDR *hdr)
{
    if (hdr == NULL) {
        errno = EINVAL;
        return;
    }

    hdr->endtime = hdr->tstamp + ((hdr->nsamp - 1) * hdr->sint);
}

/* Compute and set actual sample interval given drift rate */

void mseedSetActualSint(MSEED_HDR *hdr, REAL64 drift)
{
REAL64 rsint;

    if (hdr == NULL) {
        errno = EINVAL;
        return;
    }

    if (drift == MSEED_UNDEFINED_DRIFT) {
        hdr->asint = 0;
        hdr->sint = hdr->nsint;
    } else {
        rsint = ((REAL64) (hdr->nsint) / ((REAL64) NANOSEC_PER_SEC)) * ((REAL64) 1.0 - drift);
        hdr->asint = hdr->sint = (UINT64) (rsint * NANOSEC_PER_SEC);
    }
}

/* How many bytes are needed for a packed header (FSDH, B1000 and possible B1001 and B100) */

int mseedPackedHeaderLength(MSEED_HDR *hdr)
{
int hlen;

/* We need one 64-byte frame for FSDH and blockettes 1000, 1001 */

    hlen = MSEED_STEIM_FRAME_LEN; /* need one 64-byte frame for FSDH and blockettes 1000, 1001 */

/* We need one additional 64-byte frame if we include sample rate blockette 100 */

    if (hdr->asint > 0) hlen += MSEED_STEIM_FRAME_LEN;

    return hlen;
}

/* Maximum number of samples of a given time that can fit into a packed record */

int mseedMaxSamp(MSEED_HANDLE *handle, MSEED_HDR *hdr, BOOL flush)
{
int hlen, dlen, minsamp;
static char *fid = "mseedMaxSamp";

    if (handle == NULL || hdr == NULL) {
        errno = EINVAL;
        return -1;
    }

/* First, figure out how much of the output record is needed for headers */

    hlen = mseedPackedHeaderLength(hdr);
    dlen = handle->reclen - hlen;

/* Now figure out how many samples will fit into the available space */

    switch (hdr->format) {
      case MSEED_FORMAT_INT_16:
      case MSEED_FORMAT_INT_32:
        minsamp = flush ? hdr->nsamp : mseedMaxSteim2SampleCount(dlen);
        break;
      case MSEED_FORMAT_IEEE_F:
        minsamp = flush ? hdr->nsamp : dlen / sizeof(REAL32);
        break;
      case MSEED_FORMAT_IEEE_D:
        minsamp = flush ? hdr->nsamp : dlen / sizeof(REAL64);
        break;
      default:
        errno = ENOTSUP;
        minsamp = -1;
        mseedLog(handle, -1, "%s: UNEXPECTED ERROR: unsupported data format '%d'\n", fid, hdr->format);
    }

    return minsamp;
}

/* Initialize an empty MSEED_HDR */

void mseedInitHeader(MSEED_HDR *hdr)
{
    if (hdr == NULL) return;
    memset(hdr, 0, sizeof(MSEED_HDR));
    hdr->tqual =-1;
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
 * $Log: util.c,v $
 * Revision 1.12  2017/11/21 18:06:12  dechavez
 * removed trailing spaces
 *
 * Revision 1.11  2016/11/10 17:03:41  dechavez
 * change mseedSetIdent() packet identifier string to be of the form NN_SSS_LL_CCC/MSEED
 * (drop the _Q field so that the Nanometrics Apollo client won't get confused)
 *
 * Revision 1.10  2015/12/04 23:29:25  dechavez
 * fixed bug in mseedTimeTearInSamples() (was returning time tear in nsec!)
 *
 * Revision 1.9  2015/11/13 20:47:14  dechavez
 * removed trailing % from sprintf format string
 *
 * Revision 1.8  2015/10/30 19:42:33  dechavez
 * fixed bug in mseedTimeTearInSamples()
 *
 * Revision 1.7  2015/10/02 15:39:51  dechavez
 * introduced mseedSignaturesMatch(), mseedTimeTearInSamples(), mseedCSSDatatypeString(),
 * added code to mseedSetIdent() to also set the new sig field
 *
 * Revision 1.6  2015/09/30 20:17:16  dechavez
 * introducing mseedChnloc()
 *
 * Revision 1.5  2015/09/04 00:46:09  dechavez
 * fixed bug assinging type in mseedStageType with FILTER_TYPE_IIR_PZ
 *
 * Revision 1.4  2015/07/10 17:50:41  dechavez
 * added mseedStageType()
 *
 * Revision 1.3  2014/08/26 17:17:06  dechavez
 * use hdr->sint in mseedSetEndtime(), fixed terrible bug computing hdr->asint in mseedSetActualSint()
 *
 * Revision 1.2  2014/08/19 17:59:38  dechavez
 * added mseedValidQC()
 *
 * Revision 1.1  2014/08/11 18:18:02  dechavez
 * initial release
 *
 */
