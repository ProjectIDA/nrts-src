#pragma ident "$Id: main.c,v 1.1 2014/05/01 21:28:19 dechavez Exp $"
/*======================================================================
 *
 *  Trim the first 'n' characters off the front of each line.
 *  If 'n' is not specified (only command line argument) then n=7 is
 *  assumed.
 *
 *  Useful for cutting tags off of idalst output.
 *  For example: idalst [...] +tag < input | sort | trim > output
 *
 *====================================================================*/
#include "platform.h"

int main(int argc, char **argv)
{
char line[256];
int  num_trim;

    if (argc != 2) {
        num_trim = 7;
    } else {
        num_trim = atoi(argv[1]);
    }

    while (fgets(line, 255, stdin) != NULL) {
        printf("%s",line+num_trim);
    }

    if (ferror(stdin) || ferror(stdout)) exit(1);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2014/05/01 21:28:19  dechavez
 * original version imported from src/bin/misc
 *
 */
