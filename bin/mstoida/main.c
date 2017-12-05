#pragma ident "$Id: main.c,v 1.1 2017/10/20 01:24:12 dauerbach Exp $"
/*======================================================================
 *
 * MiniSEED to IDA10 Conversion Application
 *
 *====================================================================*/
#include "mstoida.h"

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s < miniseed > ida10\n", myname);
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i;
MSEED_RECORD record;
MSEED_PACKED packed_record;
/* char chnloc[MSEED_CHNLOCLEN + 1]; */
char *chnloc;
char buf[IDA10_FIXEDHDRLEN];
char *ptr;
int srfact, srmult;
UINT8 clock_stat;

    if (argc != 1) help(argv[0]);

    chnloc = (char *)malloc(MSEED_CHNLOCLEN);

    while (mseedReadPackedRecord(stdin, &packed_record)) {
        if (!mseedUnpackRecord(&record, packed_record.data)) {
            fprintf(stderr, "ERROR unpacking MSEED packed record\n");
            exit(1);
        }

        chnloc[0] = 0;
        memset(buf, 0, IDA10_FIXEDHDRLEN);

        chnloc = strncat(chnloc, record.hdr.chnid, MSEED_CNAMLEN);
        chnloc = strncat(chnloc, record.hdr.locid, MSEED_LNAMLEN);

        clock_stat = (record.hdr.flags.ioc & (1 << 5)) ? 1 : 0;

        if (record.hdr.reclen != MSEED_DEFAULT_RECLEN) {
            fprintf(stderr, "ERROR: record.reclen (%d) != MSEED_DEFAULT_RECLEN (%d)\n",
                    record.hdr.reclen,
                    MSEED_DEFAULT_RECLEN);
            exit(1);
        }

        ptr = buf;
        ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
        *ptr++ = 10;
        *ptr++ = 12;
        ptr += utilPackBytes(ptr, (UINT8 *) record.hdr.staid, IDA1012_SNAME_LEN);
        ptr += utilPackBytes(ptr, (UINT8 *) record.hdr.netid, IDA1012_NNAME_LEN);
        ptr += utilPackUINT64(ptr, record.hdr.tstamp);
        ptr++; /* skipping device clock specific status */
        *ptr++ = clock_stat;
        ptr += utilPackUINT32(ptr, record.hdr.seqno);
        ptr += 4;   /* utilPackUINT32(ptr, 0); host timestamp */
        ptr += 20;  /* skipping reserved portion of header */
        ptr += utilPackUINT16(ptr, IDA10_FIXEDRECLEN - (ptr - buf + sizeof(UINT16)));

        ptr += utilPackBytes(ptr, chnloc, MSEED_CHNLOCLEN + 1);
        *ptr++ = IDA10_COMP_NONE; /* data format not used for IDA10.12 MSEED payloads */
        *ptr++ = 1; /* gain */
        ptr += utilPackUINT16(ptr, record.hdr.nsamp);
        mseedNsintToFactMult(record.hdr.nsint, &srfact, &srmult);
        ptr += utilPackINT16(ptr, srfact);
        ptr += utilPackINT16(ptr, srmult);

        memcpy(ptr, packed_record.data, MSEED_DEFAULT_RECLEN);

        if (fwrite(buf, IDA10_FIXEDRECLEN, 1, stdout) != 1) {
            perror("fwrite");
            exit(1);
        }

    }

    if (feof(stdin)) {
        exit(0);
    } else {
        perror("fread");
        exit(1);
    }
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
 *
 * $Log: main.c,v $
 * Revision 1.1  2017/10/20 01:24:12  dauerbach
 * Initial 1.0.0 commit
 *
 */
