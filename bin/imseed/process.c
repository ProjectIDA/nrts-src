#pragma ident "$Id: process.c,v 1.4 2014/08/11 20:45:06 dechavez Exp $"
/*======================================================================
 *
 *  Process one newly read packet
 *
 *====================================================================*/
#include "imseed.h"

#define MY_MOD_ID MOD_PROCESS

void ProcessRecord(MSEED_HANDLE *handle, MSEED_RECORD *record)
{
static char *fid = "ProcessRecord";

    if (!mseedAddRecordToHandle(handle, record)) {
        LogErr("%s: mseedAddRecordToHandle: %s (FATAL ERROR)\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }
}

/* Revision History
 *
 * $Log: process.c,v $
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
