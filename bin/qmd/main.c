#pragma ident "$Id: main.c,v 1.2 2016/01/27 00:25:55 dechavez Exp $"
/*======================================================================
 *
 *  Read/decode a Q330 meta-data file
 *
 *====================================================================*/
#include "qdp.h"

int main(int argc, char **argv)
{
int nitems, nsets = 0;
QDP_META meta;

    while ((nitems = qdpReadMeta(stdin, &meta)) > 0) {
        printf("%d QDP metadata items read\n", nitems);
        qdpPrintTokens(stdout, &meta.token);
        qdpPrint_C1_COMBO(stdout, &meta.combo);
        qdpPrint_C2_EPD(stdout, &meta.epd);
        ++nsets;
    }

    if (nitems < 0) {
        printf("read/content error after %d sets\n", nsets);
    } else {
        printf("%d meta-data sets read\n", nsets);
    }

}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2016/01/27 00:25:55  dechavez
 * include C2_EPD filter delays in report
 *
 * Revision 1.1  2006/12/12 23:08:37  dechavez
 * initial release
 *
 */
