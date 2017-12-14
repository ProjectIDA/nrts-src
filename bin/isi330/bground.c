/*======================================================================
 *
 *  Go into the background, preserving disk loop locks
 *
 *====================================================================*/
#include "isi330.h"

#ifdef unix

BOOL BackGround(ISI330_CONFIG *cfg)
{
int devnull, child;

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

/* Parent updates all the disk loops with the new process id */

    if (child > 0) {
//        isidlUpdateParentID(cfg->dl, child);
        exit(0);
    }

/* Child stays behind */

    return TRUE;
}

#else


#endif /* unix */

