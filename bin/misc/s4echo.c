#pragma ident "$Id: s4echo.c,v 1.2 2015/12/08 21:25:48 dechavez Exp $"
/*======================================================================
 *
 *  Echo s4 data to stdout as ascii
 *
 *====================================================================*/
#include "util.h"

int main(int argc, char **argv)
{
INT32 value;

    while (fread(&value, sizeof(INT32), 1, stdin) == 1) {
#ifdef LTL_ENDIAN_HOST
        utilSwapUINT32((UINT32 *) &value, 1);
#endif
        printf("%d\n", value);
    }

    if (feof(stdin)) exit(0);
    perror("fread");
    exit(1);
}

/* Revision History
 *
 * $Log: s4echo.c,v $
 * Revision 1.2  2015/12/08 21:25:48  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.1  2015/08/24 19:16:26  dechavez
 * initial release
 *
 */
