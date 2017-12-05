#pragma ident "$Id: handle.c,v 1.2 2015/12/05 00:33:14 dechavez Exp $"
/*======================================================================
 *
 *  Handle utilities.  The list consists of handles that are associated
 *  with an individual wfdisc record.  We use the signature field "sig"
 *  in the MSEED_HDR to filter out unique streams (matching net, sta,
 *  chn and loc idents, plus matching sample interval and data format).
 *
 *====================================================================*/
#include "msdmx.h"

#define MY_MOD_ID MSDMX_MOD_HANDLE

static LNKLST *list = NULL;

void FlushHandle(MSDMX_HANDLE *handle)
{
FILE *fp;
static char *fid = "FlushHandle";

    if ((fp = OpenWfdiscFile()) != NULL) {
        fprintf(fp, "%s\n", cssioWfdiscString(&handle->wfdisc, NULL));
        fclose(fp);
    }
}

void CloseHandle(MSDMX_HANDLE *handle)
{
static char *fid = "CloseHandle";

    if (handle->fp != NULL) fclose(handle->fp);
    handle->fp = NULL;
}

void CloseAllHandles(void)
{
LNKLST_NODE *crnt;
MSDMX_HANDLE *handle;
static char *fid = "CloseAllHandles";

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        handle = (MSDMX_HANDLE *) crnt->payload;
        FlushHandle(handle);
        CloseHandle(handle);
        crnt = listNextNode(crnt);
    }
}

static void InitHandle(MSDMX_HANDLE *handle, MSEED_RECORD *record)
{
static char *fid = "InitHandle";

    handle->hdr = record->hdr;
    InitWfdisc(&handle->wfdisc, &record->hdr);
    snprintf(handle->path, MAXPATHLEN+1, "%s/%s/%s", BaseDir(), handle->wfdisc.dir, handle->wfdisc.dfile);
}

void ReInitHandle(MSDMX_HANDLE *handle, MSEED_RECORD *record)
{
INT32 foff;
static char *fid = "ReInitHandle";

    foff = handle->wfdisc.foff + (handle->wfdisc.nsamp * cssio_samplesize(handle->wfdisc.datatype));
    InitHandle(handle, record);
    handle->wfdisc.foff = foff;
}

static BOOL SignaturesMatch(MSEED_HDR *a, MSEED_HDR *b)
{
static char *fid = "SignaturesMatch";

    if (strcmp(a->sig, b->sig) == 0) {
        return TRUE;
    }

    return FALSE;
}

static MSDMX_HANDLE *LocateHandle(MSEED_RECORD *record)
{
MSDMX_HANDLE *handle;
LNKLST_NODE *crnt;
static char *fid = "LocateHandle";

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        handle = (MSDMX_HANDLE *) crnt->payload;
        if (SignaturesMatch(&record->hdr, &handle->hdr)) {
            return handle;
        }
        crnt = listNextNode(crnt);
    }

    return NULL;
}

MSDMX_HANDLE *GetHandle(MSEED_RECORD *record)
{
MSDMX_HANDLE new, *handle;
static char *fid = "GetHandle";

/* Initilize the list, first time through */

    if ((list == NULL) && ((list = listCreate()) == NULL)) {
        fprintf(stderr, "%s: listCreate: %s\n", fid, strerror(errno));
        exit(1);
    }

    if ((handle = LocateHandle(record)) != NULL) {
        if ((handle->fp = fopen(handle->path, "a")) == NULL) {
            LogErr("%s: fopen: %s: %s\n", fid, handle->path, strerror(errno));
            GracefulExit(MY_MOD_ID + 2);
         }
        return handle;
    }

    return NULL;
}

MSDMX_HANDLE *CreateHandle(MSEED_RECORD *record)
{
MSDMX_HANDLE new;
MSDMX_HANDLE *handle;
static char *fid = "CreateHandle";

    InitHandle(&new, record);

    if (!listAppend(list, (void *) &new, sizeof(MSDMX_HANDLE))) {
        LogErr("%s: listAppend: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    if ((handle = LocateHandle(record)) == NULL) {
        LogErr("%s: BUG: can't locate my own newly created handle!\n", fid);
        GracefulExit(MY_MOD_ID + 4);
    }

    if ((handle->fp = fopen(handle->path, "w")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, handle->path, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
     }
    handle->wfdisc.foff = 0;

    return handle;
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
 * $Log: handle.c,v $
 * Revision 1.2  2015/12/05 00:33:14  dechavez
 * changed CloseAllHandles() from BOOL to void
 *
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
