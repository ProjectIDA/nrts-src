#pragma ident "$Id: generic.c,v 1.2 2014/08/19 18:05:39 dechavez Exp $"
/*======================================================================
 *
 * Convert a ISI_GENERIC_TS packet to MSEED_RECORD
 *
 * This is included in the library because it was easy enough to write,
 * but it has never been tested. And should not be used without careful
 * study. Compare with ida10.c, which is actually in use routinely.
 *
 *====================================================================*/
#include "isi.h"
#include "mseed.h"

BOOL mseedConvertGenericTS(MSEED_HANDLE *handle, MSEED_RECORD *dest, ISI_GENERIC_TS *generic)
{
INT8 format;
char *staid, *netid;
static char *DefaultNetworkID = MSEED_DEFAULT_NETID;

    if (handle == NULL || dest == NULL || generic == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    netid = (handle->netid != NULL) ? handle->netid : DefaultNetworkID;
    strncpy(dest->hdr.netid, netid, MSEED_NNAMLEN);
    dest->hdr.netid[MSEED_NNAMLEN] = 0;
    util_ucase(dest->hdr.netid);

    staid = (handle->staid != NULL) ? handle->staid : generic->hdr.name.sta;
    strncpy(dest->hdr.staid, staid, MSEED_SNAMLEN);
    dest->hdr.staid[MSEED_SNAMLEN] = 0;
    util_ucase(dest->hdr.staid);

    strncpy(dest->hdr.chnid, generic->hdr.name.chn, MSEED_CNAMLEN);
    dest->hdr.chnid[MSEED_CNAMLEN] = 0;
    util_ucase(dest->hdr.chnid);

    strncpy(dest->hdr.locid, generic->hdr.name.loc, MSEED_LNAMLEN);
    dest->hdr.locid[MSEED_LNAMLEN] = 0;
    util_ucase(dest->hdr.locid);

    mseedSetIdent(&dest->hdr);

    dest->hdr.nsamp = generic->hdr.nsamp;
    dest->hdr.nsint = mseedFactMultToNsint(generic->hdr.srate.factor, generic->hdr.srate.multiplier);
    dest->hdr.asint = 0; /* actual sample interval not specified */
    dest->hdr.sint  = dest->hdr.nsint;

    dest->hdr.flags.qc = handle->qc;
    dest->hdr.flags.act = MSEED_DEFAULT_ACT_FLAG;
    dest->hdr.flags.ioc = (generic->hdr.tofs.status & ISI_TSTAMP_STATUS_LOCKED)  ? MSEED_IOC_CLOCK_LOCKED : MSEED_DEFAULT_IOC_FLAG;
    dest->hdr.flags.dat = (generic->hdr.tofs.status & ISI_TSTAMP_STATUS_SUSPECT) ? MSEED_DAT_TIME_SUSPECT : MSEED_DEFAULT_DAT_FLAG;
    if (generic->hdr.tofs.status & ISI_TSTAMP_STATUS_HAVE_QUAL) {
        dest->hdr.tqual = (generic->hdr.tofs.status & ISI_TSTAMP_QUAL_MASK) >> 8;
        if (dest->hdr.tqual >= handle->thresh.clock.locked)  dest->hdr.flags.ioc |= MSEED_IOC_CLOCK_LOCKED;
        if (dest->hdr.tqual <= handle->thresh.clock.suspect) dest->hdr.flags.dat |= MSEED_DAT_TIME_SUSPECT;
    } else {
        dest->hdr.tqual = -1; /* clock quality percentage is undefined for non-Q330 sources */
    }
    dest->hdr.tstamp = generic->hdr.tofs.value;

    switch (generic->hdr.desc.type) {
      case ISI_TYPE_INT16:  dest->hdr.format = mseedCopyINT16(dest->dat.int16,    (INT16 *) generic->data, dest->hdr.nsamp); break;
      case ISI_TYPE_INT32:  dest->hdr.format = mseedCopyINT32(dest->dat.int32,    (INT32 *) generic->data, dest->hdr.nsamp); break;
      case ISI_TYPE_REAL32: dest->hdr.format = mseedCopyREAL32(dest->dat.real32, (REAL32 *) generic->data, dest->hdr.nsamp); break;
      case ISI_TYPE_REAL64: dest->hdr.format = mseedCopyREAL64(dest->dat.real64, (REAL64 *) generic->data, dest->hdr.nsamp); break;
      default:
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
 * $Log: generic.c,v $
 * Revision 1.2  2014/08/19 18:05:39  dechavez
 * set new sint field
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
