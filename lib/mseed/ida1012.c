#pragma ident "$Id: ida1012.c,v 1.4 2018/01/18 23:37:37 dechavez Exp $"
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

#define tIDA1012_SNAME_LEN     5
#define tIDA1012_NNAME_LEN     2
#define tIDA1012_CNAME_LEN     6
#define tIDA10_FIXEDHDRLEN    64
#define tIDA10_FIXED_NBYTES  974
#define tIDA10_FIXED_RECLEN 1024
#define tIDA10_COMP_NONE       0
#define tIDA10_EPOCH_TO_1970_EPOCH 915148800
#define tIDA10_TT_SEED_LOCKED     0x01
#define tIDA10_TT_SEED_SUSPICIOUS 0x02
#define tIDA1012_RESERVED_BYTES 11

#define FSDH_SNAME_OFFSET  8
#define FSDH_NNAME_OFFSET 18

UINT8 *mseed512ToIDA1012(UINT8 *mseed512, UINT8 *dest, char *sname, char *nname, UINT64 serialno)
{
char *ptr;
int srfact, srmult;
char chnloc[MSEED_CHNLOCLEN+1];
char staid[MSEED_SNAMLEN+1];
char netid[MSEED_NNAMLEN+1];
UINT8 clockbitmap = 0;
MSEED_RECORD mseed;

    if (mseed512 == NULL || dest == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (!mseedUnpackRecord(&mseed, mseed512)) return NULL;
    if (mseed.hdr.reclen != MSEED_DEFAULT_RECLEN) return NULL;

    /* Will use the lower case versions of FSDH station code in the IDA10 header, unless overrides supplied */

    if (sname == NULL) {
        strcpy(staid, mseed.hdr.staid);
    } else {
        strncpy(staid, sname, MSEED_SNAMLEN+1);
    }
    staid[MSEED_SNAMLEN] = 0; /* in case supplied name was too long */
    util_lcase(staid);

    /* Ditto for network code */

    if (nname == NULL) {
        strcpy(netid, mseed.hdr.netid);
    } else {
        strncpy(netid, nname, MSEED_NNAMLEN+1);
    }
    netid[MSEED_NNAMLEN] = 0; /* ditto */
    util_lcase(netid);

    mseedChnLocToChnloc(mseed.hdr.chnid, mseed.hdr.locid, chnloc); util_lcase(chnloc);
    mseedNsintToFactMult(mseed.hdr.nsint, &srfact, &srmult);
    if (mseed.hdr.flags.ioc & MSEED_IOC_CLOCK_LOCKED) clockbitmap |= tIDA10_TT_SEED_LOCKED;
    if (mseed.hdr.flags.dat & MSEED_DAT_TIME_SUSPECT) clockbitmap |= tIDA10_TT_SEED_SUSPICIOUS;

    memset(dest, 0x00, tIDA10_FIXED_RECLEN);
    ptr = dest;
    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10;
    *ptr++ = 12;
    ptr += utilPackUINT64(ptr, serialno);
    ptr += utilPackBytes(ptr, (UINT8 *) staid, tIDA1012_SNAME_LEN);
    ptr += utilPackBytes(ptr, (UINT8 *) netid, tIDA1012_NNAME_LEN);
    ptr += utilPackUINT64(ptr, mseed.hdr.tstamp);
    *ptr++ = mseed.hdr.tqual;
    *ptr++ = clockbitmap;
    ptr += utilPackUINT32(ptr, mseed.hdr.seqno);
    ptr += 4; /* skip host time stamp (DL writer will insert that) */
    ptr += tIDA1012_RESERVED_BYTES;  /* skipping reserved portion of header */
    ptr += utilPackUINT16(ptr, tIDA10_FIXED_NBYTES);

    ptr += utilPackBytes(ptr, chnloc, tIDA1012_CNAME_LEN);
    *ptr++ = 0; /* data format not used for IDA10.12 MSEED payloads */
    *ptr++ = 1; /* gain */
    ptr += utilPackUINT16(ptr, mseed.hdr.nsamp);
    ptr += utilPackINT16(ptr, srfact);
    ptr += utilPackINT16(ptr, srmult);

    memcpy(ptr, mseed512, MSEED_DEFAULT_RECLEN);

    /* ensure MiniSeed payload uses the same staion and network codes as the IDA10.12 header */

    if (sname != NULL) {
        utilPadString(staid, MSEED_SNAMLEN, ' ');
        util_ucase(staid);
        memcpy(ptr + FSDH_SNAME_OFFSET, staid, MSEED_SNAMLEN);
    }

    if (nname != NULL) {
        utilPadString(netid, MSEED_NNAMLEN, ' ');
        util_ucase(netid);
        memcpy(ptr + FSDH_NNAME_OFFSET, netid, MSEED_NNAMLEN);
    }

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
 * Revision 1.4  2018/01/18 23:37:37  dechavez
 * reworked mseed512ToIDA1012() to support updated IDA10.12 definition
 *
 * Revision 1.3  2018/01/13 00:58:39  dechavez
 * added optional replacement of station and network code in the MiniSeed and
 * IDA10.12 headers with app supplied values
 *
 * Revision 1.2  2017/11/21 21:09:11  dechavez
 * changed mseed512ToIDA1012() to return pointer to IDA10.12 packet instead of BOOL
 *
 * Revision 1.1  2017/11/21 18:05:31  dechavez
 * initial release
 *
 */
