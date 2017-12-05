#pragma ident "$Id: read.c,v 1.4 2015/11/13 20:50:55 dechavez Exp $"
/*======================================================================
 * 
 * Read a single MiniSEED record into a MSEED_RECORD.  It is slightly
 * inefficient because after reading and decoding enough to figure out
 * the block size, the header is decoded a second time when the entire
 * record is passed to mseedUnpackRecord().  I don't think we'll notice.
 *
 *====================================================================*/
#include "mseed.h"

BOOL mseedReadPackedRecord(FILE *fp, MSEED_PACKED *dest)
{
UINT8 *ptr;
BOOL swap, HaveB1000;
int NextBlockette, type, remain, hlen, dlen;
static char *fid = "mseedReadPackedRecord";

    if (dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    if (fp == NULL) fp = stdin;

/* Read and decode the 48 byte FSDH */

    ptr = dest->data;
    if (fread(ptr, sizeof(UINT8), MSEED_FSDH_LEN, fp) != MSEED_FSDH_LEN) return FALSE;
    swap = (mseedHeaderByteOrder(ptr) == NATIVE_BYTE_ORDER) ? FALSE : TRUE;

    if ((NextBlockette = mseedUnpackFSDH(&dest->hdr, ptr)) < 0) return FALSE;
    ptr += MSEED_FSDH_LEN;

/* Read the rest of the header */

    remain = dest->hdr.bod - MSEED_FSDH_LEN; /* how many bytes remain in header */
    if (fread(ptr, sizeof(UINT8), remain, fp) != remain) return FALSE;

    HaveB1000 = FALSE; /* better not */

    while (NextBlockette != 0) {
        ptr = dest->data + NextBlockette;
        type = mseedUnpackINT16(ptr, swap); ptr += sizeof(INT16);
        NextBlockette = mseedUnpackINT16(ptr, swap); ptr += sizeof(INT16);
        switch (type) {
          case 100:  mseedUnpackB100 (&dest->hdr, ptr, swap); break;
          case 1000: mseedUnpackB1000(&dest->hdr, ptr, swap); HaveB1000 = TRUE; break;
          case 1001: mseedUnpackB1001(&dest->hdr, ptr, swap); break;
        }
    }
    hlen = ptr - dest->data;

/* It isn't MiniSEED without a blockette 1000 */

    if (!HaveB1000) {
        errno = ENOSYS;
        return FALSE;
    }

/* Read in the rest of the record */

    dlen = dest->hdr.reclen - dest->hdr.bod;
    if (fread(dest->data + dest->hdr.bod, sizeof(UINT8), dlen, fp) != dlen) return FALSE;

    dest->len = dest->hdr.reclen;

    return TRUE;
}

BOOL mseedReadRecord(FILE *fp, MSEED_RECORD *dest)
{
MSEED_PACKED packed;

    if (dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    if (fp == NULL) fp = stdin;

    if (!mseedReadPackedRecord(fp, &packed)) return FALSE;
    if (!mseedUnpackRecord(dest, packed.data)) return FALSE;

    return TRUE;
}

#ifdef DEBUG_TEST

/* compile: cc -DDEBUG_TEST -o tst read.c -lmseed -llogio -lutil -lmseed -lm -ldmc */
/*     run: tst < data                                                             */

int main(int argc, char **argv)
{
BOOL result;
MSEED_RECORD record;

    while (mseedReadRecord(stdin, &record)) mseedPrintHeader(stdout, &record.hdr);
    exit(0);
}
#endif /* DEBUG_TEST */

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
 * $Log: read.c,v $
 * Revision 1.4  2015/11/13 20:50:55  dechavez
 * corrected mess-up comment delimiters around Revision History
 *
 * Revision 1.3  2015/09/24 22:13:51  dechavez
 * introduced mseedReadPackedRecord() and modifed mseedReadRecord() to use it
 *
 * Revision 1.2  2015/09/15 23:38:06  dechavez
 * fixed compile instructions for test wrapper
 *
 * Revision 1.1  2015/09/15 23:24:19  dechavez
 * created, introducing mseedReadRecord()
 *
 */
