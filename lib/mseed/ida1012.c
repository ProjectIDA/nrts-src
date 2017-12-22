#pragma ident "$Id: ida1012.c,v 1.2 2017/11/21 21:09:11 dechavez Exp $"
/*======================================================================
 *
 *  Convert a 512-byte MiniSEED record to 1024-byte IDA10.12
 *  It is ASSUMED the destination array can hold at least 1024 bytes!
 *
 *  In order to avoid having to include ida10.h (which includes
 *  mseed.h) we (re)declare the known contants here.  They are
 *  guaranteed to never change (if so it wouldn't be IDA10.12)
 *
 *====================================================================*/
#include "mseed.h"
#include "util.h"

#define IDA1012_SNAME_LEN    4
#define IDA1012_NNAME_LEN    2
#define IDA1012_CNAME_LEN    6
#define IDA10_FIXED_NBYTES  974
#define IDA10_FIXED_RECLEN 1024
#define IDA10_COMP_NONE       0
#define IDA10_EPOCH_TO_1970_EPOCH 915148800

UINT8 *mseed512ToIDA1012(UINT8 *mseed512, UINT8 *dest)
{
char *ptr;
int srfact, srmult;
char chnloc[MSEED_CHNLOCLEN+1];
char staid[MSEED_SNAMLEN+1];
char netid[MSEED_NNAMLEN+1];
UINT8 clock_stat;
UINT32 timestamp;
MSEED_RECORD mseed;

    if (mseed512 == NULL || dest == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!mseedUnpackRecord(&mseed, mseed512)) return NULL;
    if (mseed.hdr.reclen != MSEED_DEFAULT_RECLEN) return NULL;
    memset(dest, 0x00, IDA10_FIXED_RECLEN);

    mseedChnLocToChnloc(mseed.hdr.chnid, mseed.hdr.locid, chnloc); util_lcase(chnloc);
    strcpy(staid, mseed.hdr.staid); util_lcase(staid);
    strcpy(netid, mseed.hdr.staid); util_lcase(netid);
    mseedNsintToFactMult(mseed.hdr.nsint, &srfact, &srmult);
    clock_stat = (mseed.hdr.flags.ioc & (1 << 5)) ? 1 : 0;
    timestamp = (UINT32) time(NULL) - IDA10_EPOCH_TO_1970_EPOCH;

    ptr = dest;
    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10;
    *ptr++ = 12;
    ptr += utilPackBytes(ptr, (UINT8 *) staid, IDA1012_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) netid, IDA1012_NNAME_LEN);
    ptr += utilPackUINT64(ptr, mseed.hdr.tstamp);
    ptr++; /* skipping device clock specific status */
    *ptr++ = clock_stat;
    ptr += utilPackUINT32(ptr, mseed.hdr.seqno);
    ptr += utilPackUINT32(ptr, timestamp);
    ptr += 20;  /* skipping reserved portion of header */
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    ptr += utilPackBytes(ptr, chnloc, IDA1012_CNAME_LEN);
    *ptr++ = IDA10_COMP_NONE; /* data format not used for IDA10.12 MSEED payloads */
    *ptr++ = 1; /* gain */
    ptr += utilPackUINT16(ptr, mseed.hdr.nsamp);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    memcpy(ptr, mseed512, MSEED_DEFAULT_RECLEN);

    return dest;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: ida1012.c,v $
 * Revision 1.2  2017/11/21 21:09:11  dechavez
 * changed mseed512ToIDA1012() to return pointer to IDA10.12 packet instead of BOOL
 *
 * Revision 1.1  2017/11/21 18:05:31  dechavez
 * initial release
 *
 */
