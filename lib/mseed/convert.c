#pragma ident "$Id: convert.c,v 1.1 2014/08/11 18:18:01 dechavez Exp $"
/*======================================================================
 *
 *  Conversion from other packet formats to MSEED_RECORD
 *
 *====================================================================*/
#include "mseed.h"

static BOOL ConvertMSEED(MSEED_HANDLE *handle, MSEED_RECORD *dest, UINT8 *src)
{
INT8 format;
char *staid, *netid;
static char *DefaultNetworkID = MSEED_DEFAULT_NETID;

    if (handle == NULL || dest == NULL || src == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    return mseedUnpackRecord(dest, src);
}

BOOL mseedConvertIsiRawPacket(MSEED_HANDLE *handle, MSEED_RECORD *dest, ISI_RAW_PACKET *raw)
{
static char *fid = "mseedConvertIsiRawPacket";

    if (handle == NULL || dest == NULL || raw == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA5:
      case ISI_TYPE_IDA6:
      case ISI_TYPE_IDA7:
      case ISI_TYPE_IDA8:
      case ISI_TYPE_IDA9:
        return mseedConvertIDA9(handle, dest, raw->payload);
        break;

      case ISI_TYPE_IDA10:
        return mseedConvertIDA10(handle, dest, raw->payload);
        break;

      case ISI_TYPE_MSEED:
        return ConvertMSEED(handle, dest, raw->payload);
        break;

      case ISI_TYPE_REFTEK:
        break;

      case ISI_TYPE_SBD1:
        break;
    }

    mseedLog(handle, -1, "%s: UNEXPECTED ERROR in file %s, line %d\n", fid,  __FILE__, __LINE__);
    mseedLog(handle, -1, "%s: unsupported input datatype code=%d\n", fid, raw->hdr.desc.type);
    errno = ENOTSUP;
    return FALSE;
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
 * $Log: convert.c,v $
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
