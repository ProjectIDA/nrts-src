#pragma ident "$Id: main.c,v 1.10 2010/04/02 18:36:43 dechavez Exp $"
/*======================================================================
 *
 *  Read data from one of several sources and write to an ISI disk loop
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_MAIN

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
ISIDL_PAR *par;
static INT32 status;

    par = init(cmdline->myname, cmdline->argc, cmdline->argv);
    while (1) {
        if ((status = ExitStatus()) == 0) {
            sleep(1);
        } else {
            LogMsg(LOG_DEBUG, "shutdown initiated: exit flag = %ld", status);
            Exit(status);
        }
    }
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
VOID main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.10  2010/04/02 18:36:43  dechavez
 * changed structure of poll loop to permit debug messages to get printed
 *
 * Revision 1.9  2010/04/01 21:49:18  dechavez
 * moved all input start ups to init, poll for exit conditions
 *
 * Revision 1.8  2007/02/20 02:35:14  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.7  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.6  2006/06/19 19:12:40  dechavez
 * conditional Q330 support
 *
 * Revision 1.5  2006/06/02 21:00:57  dechavez
 * ISI_TYPE_QDPLUS support
 *
 * Revision 1.4  2006/03/30 22:03:11  dechavez
 * Use ProcessLocalData thread to feed disk loop for station systems
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
