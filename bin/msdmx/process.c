#pragma ident "$Id: process.c,v 1.1 2015/10/02 15:46:39 dechavez Exp $"
/*======================================================================
 *
 *  Process each MSEED_RECORD as it is read
 *
 *====================================================================*/
#include "msdmx.h"

static BOOL HaveTimeTear(MSDMX_HANDLE *handle, MSEED_RECORD *record)
{
INT64 errsmp;
static char *fid = "HaveTimeTear";

    if ((errsmp = mseedTimeTearInSamples(&handle->hdr, &record->hdr)) == 0) {
        return FALSE;
    }

    LogMsg("%lld sample time tear detected for %s:%s\n", errsmp, record->hdr.staid, mseedChnloc(&record->hdr, NULL));
    LogMsg("    prev: %s\n", mseedHdrString(&handle->hdr, NULL));
    LogMsg("    crnt: %s\n", mseedHdrString(&record->hdr, NULL));
    
    return TRUE;
}

void ProcessRecord(MSEED_RECORD *record)
{
MSDMX_HANDLE *handle;
static char *fid = "ProcessRecord";

    IncrementCounter();

    if ((handle = GetHandle(record)) == NULL) {
        handle = CreateHandle(record);
    } else if (HaveTimeTear(handle, record)) {
        FlushHandle(handle);
        ReInitHandle(handle, record);
    }

    WriteData(handle, record);
    CloseHandle(handle);
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
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
