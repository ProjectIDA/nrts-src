#pragma ident "$Id: history.c,v 1.2 2015/11/13 20:52:05 dechavez Exp $"
/*======================================================================
 *
 * history related functions
 *
 *====================================================================*/
#include "mseed.h"

static INT32 firstsample(MSEED_RECORD *record)
{
    switch (record->hdr.format) {
      case MSEED_FORMAT_INT_16: return (INT32) record->dat.int16[0];
      case MSEED_FORMAT_INT_32: return (INT32) record->dat.int32[0];
      case MSEED_FORMAT_IEEE_F: return (INT32) record->dat.real32[0];
      case MSEED_FORMAT_IEEE_D: return (INT32) record->dat.real64[0];
    }

    return 0;
}

static MSEED_DATUM *CreateDatum(MSEED_HANDLE *handle, char *ident, INT32 value)
{
MSEED_DATUM *datum;
static char *fid = "mseedGetHistory:CreateDatum";

    if (handle == NULL || ident == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((datum = (MSEED_DATUM *) malloc(sizeof(MSEED_DATUM))) == NULL) {
        mseedLog(handle, -1, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    strcpy(datum->ident, ident);
    datum->value = value;

    return datum;
}

static MSEED_DATUM *LocateDatum(LNKLST *list, char *target)
{
LNKLST_NODE *crnt;
MSEED_DATUM *datum;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        datum = (MSEED_DATUM *) crnt->payload;
        if (strcmp(datum->ident, target) == 0) return datum;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

MSEED_DATUM *mseedGetHistory(MSEED_HANDLE *handle, MSEED_RECORD *record, char *ident)
{
INT32 InitialValue;
LNKLST *list;
MSEED_DATUM *datum;
static char *fid = "mseedGetHistory";

    if (handle == NULL) {
        errno = EINVAL;
        return NULL;
    }

/* Only one of record and ident should be defined */

    if (record == NULL && ident == NULL) {
        errno = EINVAL;
        mseedLog(handle, -1, "ERROR: %s: NEITHER 'record' nor 'ident' args are defined\n", fid);
        return NULL;
    }

    if (record != NULL && ident != NULL) {
        errno = EINVAL;
        mseedLog(handle, -1, "ERROR: %s: BOTH 'record' and 'ident' args are defined\n", fid);
        return NULL;
    }

/* Pass a MSEED_RECORD when tracking last samples, pass an ident when tracking sequence numbers */

    if (record != NULL) {
        ident = record->hdr.ident;
        list  = handle->history.data;
        InitialValue = firstsample(record); /* initial value if we have to initialize the entry for this ident */
    } else {
        list  = handle->history.seqno;
        InitialValue = 0;                   /* ditto */
    }

/* Search the appropriate list, keying off the stream ident */

    if ((datum = LocateDatum(list, ident)) != NULL) return datum;

/* Didn't find one, so create a new entry */

    if ((datum = CreateDatum(handle, ident, InitialValue)) == NULL) return NULL;
    if (!listAppend(list, datum, sizeof(MSEED_DATUM))) {
        mseedLog(handle, -1, "%s: listAppend: %s\n", fid, strerror(errno));
        return NULL;
    }

/* We better find an entry now, since we just added it */

    if ((datum = LocateDatum(list, ident)) != NULL) return datum;

    mseedLog(handle, -1, "%s: Cannot locate my own datum! THIS SHOULD NEVER OCCUR!\n", fid);
    return NULL;
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
 * $Log: history.c,v $
 * Revision 1.2  2015/11/13 20:52:05  dechavez
 * replaced some incorrect return FALSE statements with return NULL
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
