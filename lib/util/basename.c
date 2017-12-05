#pragma ident "$Id"
/*======================================================================
 *
 *  Returns the filename component from a full path.  Allocates memory
 *  with strdup().
 *
 *====================================================================*/
#include "util.h"

#define MAXCOMPONENTS 256

char *utilBasename(char *path)
{
int ntoken;
char *copy, *token[MAXCOMPONENTS], *result;
#define DELIMITERS "./\\"

    if ((copy = strdup(path)) == NULL) return NULL;

    ntoken = utilParse(copy, token, DELIMITERS, MAXCOMPONENTS, 0);

    result = strdup(token[ntoken-1]);
    free(copy);
    return result;
}

/* Revision History
 *
 * $Log: basename.c,v $
 * Revision 1.3  2011/02/07 19:58:17  dechavez
 * return a malloc'd pointer to just the token, not the orginal copy
 *
 * Revision 1.2  2009/06/19 18:26:02  dechavez
 * use 0 for utilParse quote string instead of NULL, to calm a compiler
 *
 * Revision 1.1  2003/06/10 00:04:46  dechavez
 * initial release
 *
 */
