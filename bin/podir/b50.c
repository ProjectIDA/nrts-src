#pragma ident "$Id: b50.c,v 1.4 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 50 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B50

void WriteBlockette50(FILE *fp, char *fpath, DCCDB_SITE *site, int net_code, char *net_id)
{
MSEED_B50 b50;
static char *fid = "WriteBlockette50";

    strncpy(b50.name, site->sta, MSEED_B50_STA_LEN+1);
    b50.code = net_code;
    b50.beg = utilConvertFrom1970SecsTo1999Nsec(site->begt);
    b50.end = (site->endt == DCCDB_NULL_ENDT) ? MSEED_NEVER : utilConvertFrom1970SecsTo1999Nsec(site->endt);
    b50.lat = site->lat;
    b50.lon = site->lon;
    b50.elev = site->elev * 1000.0; /* convert from km to m */
    strncpy(b50.desc, site->desc, MSEED_B50_DESC_LEN+1);
    strncpy(b50.net, net_id, MSEED_B50_NET_LEN+1);
    b50.update = MSEED_DEFAULT_B50_UPDATE_FLAG;

    if (!mseedWriteBlockette50(fp, &b50)) {
        LogErr("%s: mseedWriteBlockette50: %s: %s\n", fid, fpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
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
 * $Log: b50.c,v $
 * Revision 1.4  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
