#pragma ident "$Id: write.c,v 1.1 2015/10/02 15:46:39 dechavez Exp $"
/*======================================================================
 *
 *  Write the data to disk
 *
 *====================================================================*/
#include "msdmx.h"

#define MY_MOD_ID MSDMX_MOD_WRITE

void WriteData(MSDMX_HANDLE *handle, MSEED_RECORD *record)
{
int nbytes;
static char *fid = "WriteData";

    switch (record->hdr.format) {
      case MSEED_FORMAT_INT_16: nbytes = record->hdr.nsamp * sizeof(INT16); break;
      case MSEED_FORMAT_INT_32: nbytes = record->hdr.nsamp * sizeof(INT32); break;
      case MSEED_FORMAT_IEEE_F: nbytes = record->hdr.nsamp * sizeof(REAL32); break;
      case MSEED_FORMAT_IEEE_D: nbytes = record->hdr.nsamp * sizeof(REAL64); break;
      default:
        LogMsg("%s: ERROR: unsupported data format '%d'\n", fid, record->hdr.format);
        GracefulExit(MY_MOD_ID + 5);
    }

    if (fwrite(record->dat.uint8, sizeof(UINT8), nbytes, handle->fp) != nbytes) {
        LogErr("%s: ERROR: fwrite: %s: %s\n", fid, handle->path, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    handle->wfdisc.nsamp += record->hdr.nsamp;
    handle->wfdisc.endtime = handle->wfdisc.time + (double) (handle->wfdisc.nsamp-1)/(double) handle->wfdisc.smprate;
    handle->hdr = record->hdr;
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
 * $Log: write.c,v $
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
