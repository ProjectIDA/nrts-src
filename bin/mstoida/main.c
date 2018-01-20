#pragma ident "$Id: main.c,v 1.5 2018/01/18 23:52:31 dechavez Exp $"
/*======================================================================
 *
 * MiniSEED to IDA10 Conversion Application
 *
 *====================================================================*/
#include "mstoida.h"

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [sta=sname net=name] < miniseed > ida10\n", myname);
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, errors = 0;
MSEED_PACKED packed_record;
char ida10[IDA10_FIXEDRECLEN];
char *sname = NULL;
char *nname = NULL;
char *myname;

    myname = argv[0];

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sname = argv[i] + strlen("sta=");
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            nname = argv[i] + strlen("net=");
        } else if (strcmp(argv[i], "-h") == 0) {
            help(myname);
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname);
            help(myname);
        }
    }

    if (sname != NULL && strlen(sname) > MSEED_SNAMLEN) {
        fprintf(stderr, "ERROR: supplied station name '%s' cannot be longer than %d characters\n", MSEED_SNAMLEN);
        ++errors;
    }

    if (nname != NULL && strlen(nname) > MSEED_NNAMLEN) {
        fprintf(stderr, "ERROR: supplied network name '%s' cannot be longer than %d characters\n", MSEED_NNAMLEN);
        ++errors;
    }

    if (errors) exit(1);

    while (mseedReadPackedRecord(stdin, &packed_record)) {

        if (mseed512ToIDA1012(packed_record.data, ida10, sname, nname, 0) == NULL) {
            fprintf(stderr, "%s: mseed512ToIDA1012: %s\n", argv[0], strerror(errno));
            exit(1);
        }

       if (fwrite(ida10, IDA10_FIXEDRECLEN, 1, stdout) != 1) {
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
 * Revision 1.5  2018/01/18 23:52:31  dechavez
 * mseed512ToIDA1012() needs serialno with new IDA10.12 definition, set to 0
 *
 * Revision 1.4  2018/01/13 01:01:15  dechavez
 * added sta=sname and net=nname overrides of packet header options
 *
 * Revision 1.3  2017/11/21 21:07:06  dechavez
 * changed mseed512ToIDA1012() success test to reflect new return type (UINT8 *) in libmseed 2.7.1
 *
 * Revision 1.2  2017/11/21 18:09:07  dechavez
 * moved conversion code over to libmseed as mseed512ToIDA1012(), reworked main to use same
 *
 * Revision 1.1  2017/10/20 01:24:12  dauerbach
 * Initial 1.0.0 commit
 *
 */
