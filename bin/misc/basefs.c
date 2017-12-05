#pragma ident "$Id: basefs.c,v 1.3 2010/10/04 18:35:14 dechavez Exp $"
#include "platform.h"
/*
 * Given a string of the from PrefixSuffix and the string Suffix
 * return Prefix.
 *
 * eg: basefs dpool/home/mydir@0 @0
 *
 *     would return dpool/home/mydir
 */

int main(int argc, char **argv)
{
char *prefix, *suffix, *result;

    if (argc != 3) {
        fprintf(stderr, "usage: %s prefix suffix\n", argv[0]);
        exit(1);
    }

    prefix = argv[1];
    suffix = argv[2];

    if ((result = strstr(prefix, suffix)) != NULL) result[0] = 0;
    printf("%s\n", prefix);

    exit(0);
}

/* Revision History
 *
 * $Log: basefs.c,v $
 * Revision 1.3  2010/10/04 18:35:14  dechavez
 * use strstr instead of strtok (duh)
 *
 * Revision 1.2  2010/08/25 16:29:49  dechavez
 * include "platform.h" instead of specifics
 *
 * Revision 1.1  2010/06/14 19:27:36  dechavez
 * created
 *
 */
