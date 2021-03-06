#pragma ident "$Id: main.c,v 1.1 2014/05/01 21:19:16 dechavez Exp $"
/*======================================================================
 *
 *  Stdin/stdout filter which strips out blank lines and commented
 *  text.  A comment is the single character given as the only command
 *  line argument.  If no argument is given then '#' is assumed.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

#define BUFLEN 1024
#define COMMENT '#'

void help(char *myname)
{
    fprintf(stderr, "usage: %s [comment=char] [input file]\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i;
FILE *fp = stdin;
char *fname = NULL;
int lineno = 0;
int status = 0;
char comment = COMMENT;
char buffer[BUFLEN];

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "comment=", strlen("comment=")) == 0) {
            comment = *(argv[i] + strlen("comment="));
        } else if (fname != NULL) {
            help(argv[0]);
        } else {
            fname = argv[i];
        }
    }

    if (fname != NULL && (fp = fopen(fname, "r")) == NULL) {
        perror(fname);
        exit(1);
    }

    while ((status = util_getline(fp, buffer, BUFLEN, comment, &lineno)) == 0) {
        printf("%s\n", buffer);
    }

    if (status != 1) {
        perror(argv[0]);
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2014/05/01 21:19:16  dechavez
 * original version imported from src/bin/misc
 *
 */
