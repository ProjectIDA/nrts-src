#pragma ident "$Id: mseed.c,v 1.1 2016/02/05 20:13:52 dechavez Exp $"
/*======================================================================
 *
 * Save MiniSEED packets
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_MSEED

#define DEFAULT_MSEED_RECLEN 512
#define DEFAULT_MSEED_QCODE  'R'
#define DEFAULT_MSEED_SEQNO  1

static FILE *msfp = NULL;
static MSEED_HANDLE *handle = NULL;

static void MseedCallback(void *unused, MSEED_PACKED *packed)
{
    if (fwrite(packed->data, packed->len, 1, msfp) != 1) {
        LogMsg("MseedCallback: %s\n", strerror(errno));
        SetExitStatus(MY_MOD_ID + 1);
        return;
    }

    free(packed); /* don't forget to free! */
}

void CloseMSEED(QCAL *qcal)
{
    if (handle != NULL) mseedFlushHandle(handle);
    if (msfp != NULL) fclose(msfp);
}

void GenerateMSEED(UINT8 *ida10)
{
MSEED_RECORD record;

    if (handle == NULL) return;

    if (!mseedConvertIDA10(handle, &record, ida10)) { 
        LogMsg("ERROR: mseedConvertIDA10: %s\n", strerror(errno));
        SetExitStatus(MY_MOD_ID + 1);
    }
    if (!mseedAddRecordToHandle(handle, &record)) {
        LogMsg("ERROR: mseedAddRecordToHandle: %s\n", strerror(errno));
        SetExitStatus(MY_MOD_ID + 2);
    }
}

BOOL OpenMSEED(QCAL *qcal)
{
    if ((handle = mseedCreateHandle(qcal->lp, DEFAULT_MSEED_RECLEN, DEFAULT_MSEED_QCODE, DEFAULT_MSEED_SEQNO, MseedCallback, NULL)) == NULL) {
        perror("ERROR: mseedCreateHandle");
        return FALSE;
    }

    if (qcal->dport.sta != NULL) {
        util_ucase(qcal->dport.sta);
        if (!mseedSetStaid(handle, qcal->dport.sta)) {
            perror("mseedSetStaid");
            return FALSE;
        }
    }

    if (qcal->dport.net != NULL) {
        util_ucase(qcal->dport.net);
        if (!mseedSetNetid(handle, qcal->dport.net)) {
            perror("mseedSetNetid");
            return FALSE;
        }
    }

    if ((msfp = fopen(qcal->name.mseed, "w")) == NULL) {
        fprintf(stderr, "ERROR: fopen: ");
        perror(qcal->name.mseed);
        return FALSE;
    }

    return TRUE;
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
 * $Log: mseed.c,v $
 * Revision 1.1  2016/02/05 20:13:52  dechavez
 * created
 *
 */
