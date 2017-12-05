#pragma ident "$Id: bground.c,v 1.1 2010/09/17 19:37:14 dechavez Exp $"
/*======================================================================
 *
 *  Go into the background, preserving disk loop locks
 *
 *====================================================================*/
#include "isimerge.h"

#ifdef unix

BOOL BackGround()
{
int i, devnull, child;

/* Ignore the terminal stop signals */

#ifdef SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif

/* Disassociate from controlling terminal */

    if ((devnull = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(devnull, fileno(stdin));
        dup2(devnull, fileno(stdout));
        dup2(devnull, fileno(stderr));
        close(devnull);
    }

/* Fork, parent gets the child's process id */

    if ((child = fork()) < 0) return FALSE;

/* Parent updates the disk loop with the new process id */

    if (child > 0) {
        UpdateParentID();
        exit(0);
    }

/* Child stays behind */

    return TRUE;
}

#else

BOOL BackGround()
{
    return TRUE;
}

#endif /* unix */

/* Revision History
 *
 * $Log: bground.c,v $
 * Revision 1.1  2010/09/17 19:37:14  dechavez
 * initial release
 *
 */
