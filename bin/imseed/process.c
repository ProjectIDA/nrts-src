#pragma ident "$Id: process.c,v 1.5 2017/11/01 21:38:39 dechavez Exp $"
/*======================================================================
 *
 *  Process one newly read packet
 *
 *====================================================================*/
#include "imseed.h"

#define MY_MOD_ID MOD_PROCESS

/* IDA10.12 packets contain native Miniseed, just write them out directly using
 * the same callback as mseedAddRecordToHandle().  That callback is required to
 * free the data it is passed, so we allocate a fresh MSEED_PACKED each time.
 * */

void ProcessIDA1012(MSEED_HANDLE *handle, UINT8 *src)
{
MSEED_PACKED *packed;
static char *fid = "ProcessIDA1012";

/* Callback expects a MSEED_PACKED structure, allocate and populate it here */

    if ((packed = (MSEED_PACKED *) malloc(sizeof(MSEED_PACKED))) == NULL) {
        LogErr("%s: FATAL ERROR: malloc: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    if (!mseedUnpackHeader(&packed->hdr, src)) {
        LogErr("%s: mseedUnpackHeader: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

    if ((packed->len = packed->hdr.reclen) != handle->reclen) {
        LogErr("%s: mseedUnpackHeader: hdr.reclen (%d) != handle->reclen (%d)\n", fid, packed->hdr.reclen, handle->reclen);
        GracefulExit(MY_MOD_ID + 3);
    }

    memcpy(packed->data, src, packed->len);

/* write to disk using the callback */

    (handle->func)(handle->argptr, packed);
}

/* "True" IDA10 data get demutiplexed and feed into the handle for generating Miniseed  */

void ProcessRecord(MSEED_HANDLE *handle, MSEED_RECORD *record)
{
static char *fid = "ProcessRecord";

    if (!mseedAddRecordToHandle(handle, record)) {
        LogErr("%s: FATAL ERROR: mseedAddRecordToHandle: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }
}

/* Revision History
 *
 * $Log: process.c,v $
 * Revision 1.5  2017/11/01 21:38:39  dechavez
 * introduced ProcessIDA1012()
 *
 * Revision 1.4  2014/08/11 20:45:06  dechavez
 * Rework to support libmseed 2.0.0
 *
 * Revision 1.3  2014/03/06 18:51:49  dechavez
 * rework to accomodate the move of template.c over to libmseed
 *
 * Revision 1.2  2014/03/04 21:10:43  dechavez
 * Major rework to make use of the new MSFlags field in the modified libmseed
 * which allows propagating input conditions to the MiniSEED FSDH flags and
 * blockette 1001
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
