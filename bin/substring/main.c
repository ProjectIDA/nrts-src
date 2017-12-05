#pragma ident "$Id: main.c,v 1.1 2014/05/01 21:26:06 dechavez Exp $"
/*======================================================================
 *
 *  does the same as "expr substing arg beg count".  It turns out that
 *  not all implementations of expr support the substr command,
 *  hence this program.
 *
 *====================================================================*/
#include "platform.h"

int main(int argc, char **argv)
{
int start, count, extra;
char *string, *substr;

    if (argc != 4) {
        fprintf(stderr, "usage: %s string start_index count\n", argv[0]);
        exit(1);
    }
    string = argv[1];
    start = atoi(argv[2]);
    count = atoi(argv[3]);

    if (start < 1 || count == 0) {
        printf("");
        exit(2);
    }

    if (strlen(string) < start) {
        printf("");
        exit(3);
    }

    extra = count - (strlen(string) - (start-1));
    if (extra > 0) count -= extra;

    substr = &string[start-1];
    substr[count] = 0;

    printf("%s\n", substr);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2014/05/01 21:26:06  dechavez
 * original version imported from src/bin/misc
 *
 */
