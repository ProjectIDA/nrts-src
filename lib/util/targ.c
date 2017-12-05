#pragma ident "$Id: targ.c,v 1.1 2013/09/20 15:59:30 dechavez Exp $"
/*======================================================================
 *
 * Finally, a function to deal with the command line argument checks of
 * the form "if (strncasecmp(argv[i], "arg=", strlen("arg="))"!
 *
 * Returns a pointer to the stuff after the = sign, or NULL.
 *
 *====================================================================*/
#include "util.h"

BOOL utilTarg(char *arg, char *target, BOOL CaseSensitive, char **result)
{
    if (arg == NULL || target == NULL || result == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (CaseSensitive) {
        if (strncmp(arg, target, strlen(target)) == 0) {
            *result = arg + strlen(target); return TRUE; }
        else {
            return FALSE;
        }
    } else if (strncasecmp(arg, target, strlen(target)) == 0) {
        *result = arg + strlen(target);
        return TRUE;
    }

    return FALSE;
}

/* Revision History
 *
 * $Log: targ.c,v $
 * Revision 1.1  2013/09/20 15:59:30  dechavez
 * created
 *
 */
