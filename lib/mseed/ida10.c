#pragma ident "$Id: ida10.c,v 1.3 2014/08/26 17:29:08 dechavez Exp $"
/*======================================================================
 *
 *  Conversion from IDA10 to MSEED_RECORD
 *
 *====================================================================*/
#include "mseed.h"
#include "ida10.h"
#include "isi.h"
#include "dmc.h"

BOOL mseedConvertIDA10(MSEED_HANDLE *handle, MSEED_RECORD *dest, UINT8 *src)
{
INT8 format;
IDA10_TS ts;
ISI_STREAM_NAME name;
char *staid, *netid, chnid[MSEED_CNAMLEN+1], locid[MSEED_LNAMLEN+1];
static char *DefaultNetworkID = MSEED_DEFAULT_NETID;
static char *fid = "mseedConvertIDA10";

    if (handle == NULL || dest == NULL || src == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!ida10UnpackTS(src, &ts)) {
        mseedLog(handle, -1, "%s: ida10UnpackTS: %s\n", fid, strerror(errno));
        return FALSE;
    }

/* station name is either as-is from the data or user specified via the handle */

    staid = (handle->staid != NULL) ? handle->staid : ts.hdr.sname;

/* network name is either user specified or assumed to be "II" */

    netid = (handle->netid != NULL) ? handle->netid : DefaultNetworkID;

/* some (or all) IDA10 channel names can be replaced using the optional channel map in the handle */

    if (handle->rename.ida10 != NULL) ida10Cname(ts.hdr.cname, handle->rename.ida10);

/* and the IDA10 channel name is split from chnlc into chn, loc pairs */

    strncpy(name.chnloc, ts.hdr.cname, ISI_CHNLOCLEN);
    isiChnlocToChnLoc(&name);
    strncpy(chnid, name.chn, 4);
    strncpy(locid, name.loc, 3);

/* now that the input names are organized, define the FSDH header fields */

    strncpy(dest->hdr.staid, staid, MSEED_SNAMLEN);
    strncpy(dest->hdr.netid, netid, MSEED_NNAMLEN);
    strncpy(dest->hdr.chnid, chnid, MSEED_CNAMLEN);
    strncpy(dest->hdr.locid, locid, MSEED_LNAMLEN);

    util_ucase(dest->hdr.staid);
    util_ucase(dest->hdr.chnid);
    util_ucase(dest->hdr.locid);
    util_ucase(dest->hdr.netid);

/* that's it for names, now the rest of the header */

    dest->hdr.flags.qc = handle->qc;
    mseedSetIdent(&dest->hdr);
    dest->hdr.nsamp = ts.hdr.nsamp;
    dest->hdr.nsint = mseedFactMultToNsint(ts.hdr.dl.srate.factor, ts.hdr.dl.srate.multiplier);
    mseedSetActualSint(&dest->hdr, handle->drift);
    dest->hdr.sint = (dest->hdr.asint > 0) ? dest->hdr.asint : dest->hdr.nsint;

    dest->hdr.flags.act = MSEED_DEFAULT_ACT_FLAG;
    dest->hdr.flags.ioc = (ts.hdr.cmn.ttag.beg.status.locked)  ? MSEED_IOC_CLOCK_LOCKED : MSEED_DEFAULT_IOC_FLAG;
    dest->hdr.flags.dat = (ts.hdr.cmn.ttag.beg.status.suspect) ? MSEED_DAT_TIME_SUSPECT : MSEED_DEFAULT_DAT_FLAG;
    if (ts.hdr.cmn.ttag.beg.type == IDA10_TIMER_Q330) {
        dest->hdr.tqual = ts.hdr.cmn.ttag.beg.q330.qual.percent;
        if (dest->hdr.tqual >= handle->thresh.clock.locked)  dest->hdr.flags.ioc |= MSEED_IOC_CLOCK_LOCKED;
        if (dest->hdr.tqual <= handle->thresh.clock.suspect) dest->hdr.flags.dat |= MSEED_DAT_TIME_SUSPECT;
    } else {
        dest->hdr.tqual = -1; /* clock quality percentage is undefined for non-Q330 sources */
    }
    dest->hdr.tstamp  = ts.hdr.cmn.ttag.beg.nepoch;
    mseedSetEndtime(&dest->hdr);

/* Copy over the data */

    switch (ts.hdr.datatype) {
      case IDA10_DATA_INT32:  dest->hdr.format = mseedCopyINT32(dest->dat.int32, ts.data.int32, dest->hdr.nsamp); break;
      case IDA10_DATA_INT16:  dest->hdr.format = mseedCopyINT16(dest->dat.int16, ts.data.int16, dest->hdr.nsamp); break;
      case IDA10_DATA_REAL32: dest->hdr.format = mseedCopyREAL32(dest->dat.real32, ts.data.real32, dest->hdr.nsamp); break;
      case IDA10_DATA_REAL64: dest->hdr.format = mseedCopyREAL64(dest->dat.real64, ts.data.real64, dest->hdr.nsamp); break;
      default:
        mseedLog(handle, -1, "%s: UNEXPECTED ERROR: unsupported IDA10 data type '%d'\n", fid, ts.hdr.datatype);
        errno = ENOTSUP;
        return FALSE;
    }

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
 * $Log: ida10.c,v $
 * Revision 1.3  2014/08/26 17:29:08  dechavez
 * use new IDA10_TTAG nepoch field to set hdr.tstamp
 *
 * Revision 1.2  2014/08/19 18:05:19  dechavez
 * set new sint field, fixed missing QC field in ident string bug
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
