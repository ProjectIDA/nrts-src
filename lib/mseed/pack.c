#pragma ident "$Id: pack.c,v 1.8 2015/11/13 20:51:08 dechavez Exp $"
/*======================================================================
 * 
 * Pack a MSEED_RECORD into a MiniSEED packet, as per the handle.
 *
 *====================================================================*/
#include "mseed.h"

static BOOL FreeAndFail(MSEED_PACKED *packed)
{
    if (packed == NULL) return FALSE;
    free(packed);
    return FALSE;
}

static int ShiftData(MSEED_HANDLE *handle, MSEED_RECORD *record, int nskip, int ncopy)
{
int i, j;
static char *fid = "mseedPackRecord:ShiftData";

    switch (record->hdr.format) {
      case MSEED_FORMAT_INT_16: mseedCopyINT16(&record->dat.int16[0], &record->dat.int16[nskip], ncopy); break;
      case MSEED_FORMAT_INT_32: mseedCopyINT32(&record->dat.int32[0], &record->dat.int32[nskip], ncopy); break;
      case MSEED_FORMAT_IEEE_F: mseedCopyREAL32(&record->dat.real32[0], &record->dat.real32[nskip], ncopy); break;
      case MSEED_FORMAT_IEEE_D: mseedCopyREAL64(&record->dat.real64[0], &record->dat.real64[nskip], ncopy); break;
    }

    mseedLog(handle, 3, "%s: ident=%s nskip=%d ncopy=%d format=%s\n", fid, record->hdr.ident, nskip, ncopy, mseedDataFormatString(record->hdr.format));
    return ncopy;
}

BOOL mseedPackRecord(MSEED_HANDLE *handle, MSEED_RECORD *record, BOOL flush)
{
UINT8 nblockettes;
INT64 sint;
int minsamp, hlen, npacked, remain, nframe, format;
char tmpbuf[1024];
MSEED_PACKED *packed = NULL;
static char *fid = "mseedPackRecord";

    if (handle == NULL || record == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Nothing to do if we haven't accumulated enough samples */

    mseedLog(handle, 3, "%s: begin %s (flush=%s)\n", fid, mseedHdrString(&record->hdr, tmpbuf), flush ? "TRUE" : "FALSE");
    if (record->hdr.nsamp == 0) return TRUE;

    minsamp = mseedMaxSamp(handle, &record->hdr, flush);
    mseedLog(handle, 3, "%s: minsamp = %d\n", fid, minsamp);
    if (record->hdr.nsamp < minsamp && !flush) return TRUE;

/* We'll be packing data, so allocate space for the output. It is up to the application to free it. */

    if ((packed = (MSEED_PACKED *) malloc(sizeof(MSEED_PACKED))) == NULL) {
        mseedLog(handle, -1, "%s: malloc: %s", fid, strerror(errno));
        return FALSE;
    }
    memset(packed->data, 0, packed->len = handle->reclen);

/* Figure out how long the header will need to be */

    record->hdr.bod = mseedPackedHeaderLength(&record->hdr); /* always returns a multiple of 64 bytes */

/* If output encoding format not explicity set, derive the default format */

    switch (handle->format) {
      case MSEED_FORMAT_UNDEF:
      case MSEED_FORMAT_COMPRESSED:
        switch (record->hdr.format) {
          case MSEED_FORMAT_INT_16: format = MSEED_FORMAT_STEIM2; break;
          case MSEED_FORMAT_INT_32: format = MSEED_FORMAT_STEIM2; break;
          default:
            format = record->hdr.format;
        }
        break;

      case MSEED_FORMAT_UNCOMPRESSED:
        format = record->hdr.format;
        break;

      default:
        format = handle->format;
    }

/* Pack in the data */

    mseedLog(handle, 3, "%s: packing %d samples of %s into %s, bod=%d\n", fid, record->hdr.nsamp, 
        mseedDataFormatString(record->hdr.format), mseedDataFormatString(format), record->hdr.bod
    );
    if (!mseedPackData(handle, packed, record, &npacked, &nframe, &format)) {
        mseedLog(handle, -1, "%s: mseedPackData: %s", fid, strerror(errno));
        return FreeAndFail(packed);
    }

/* Pack the FSDH and any blockettes */

    if (!mseedPackHeader(handle, packed, record, npacked, nframe, format)) {
        mseedLog(handle, -1, "%s: mseedPackHeader: %s", fid, strerror(errno));
        return FreeAndFail(packed);
    }

/* Send this off to the application */

    remain = record->hdr.nsamp - npacked;
    record->hdr.nsamp = npacked;
    memcpy(&packed->hdr, &record->hdr, sizeof(MSEED_HDR));
    mseedLog(handle, 3, "%s: send %d byte MiniSEED packet containing %d %s samples to application\n", fid, packed->len, packed->hdr.nsamp, packed->hdr.ident);
    (handle->func)(handle->argptr, packed); /* APPLICATION MUST FREE packed */

/* If there are any left over samples, move them to the front of the data array and adjust header accordingly */

    if (remain) {
        record->hdr.nsamp = ShiftData(handle, record, npacked, remain);
        sint = (record->hdr.asint != 0) ? record->hdr.asint : record->hdr.nsint;
        record->hdr.tstamp += (npacked * sint);
    } else {
        record->hdr.nsamp = 0;
    }
    mseedLog(handle, 3, "%s: on exit: %s\n", fid, mseedHdrString(&record->hdr, tmpbuf));

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
 * $Log: pack.c,v $
 * Revision 1.8  2015/11/13 20:51:08  dechavez
 * corrected mess-up comment delimiters around Revision History
 *
 * Revision 1.7  2014/10/29 21:23:33  dechavez
 * mseedPackData() "format" argument is now a pointer
 *
 * Revision 1.6  2014/08/19 18:11:34  dechavez
 * rework to use MSEED_PACKED
 *
 * Revision 1.5  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
