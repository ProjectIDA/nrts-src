#pragma ident "$Id: main.c,v 1.1 2015/09/23 19:32:58 dechavez Exp $"
/*======================================================================
 *
 *  List MiniSeed packet headers
 *
 *====================================================================*/
#include "mseed.h"

int main(int argc, char **argv)
{
int i;
FILE *fp = stdin;
MSEED_RECORD record;

    if (argc == 1) {
        while (mseedReadRecord(stdin, &record)) mseedPrintHeader(stdout, &record.hdr);
    } else {
        for (i = 1; i < argc; i++) {
            if ((fp = fopen(argv[i], "r")) == NULL) {
                perror(argv[i]);
                exit(1);
            }
            while (mseedReadRecord(fp, &record)) mseedPrintHeader(stdout, &record.hdr);
            fclose(fp);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2015/09/23 19:32:58  dechavez
 * initial release
 *
 */
