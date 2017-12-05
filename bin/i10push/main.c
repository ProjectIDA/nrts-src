#pragma ident "$Id: main.c,v 1.1 2011/08/04 22:07:22 dechavez Exp $"
/*======================================================================
 *
 *  Take IDA10 from a FILE and push to an ISI disk loop server
 *
 *====================================================================*/
#include "push.h"

#define MY_MOD_ID PUSH_MOD_MAIN

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
int type, status, len;
ISI_PUSH *ph;
UINT8 *buf;
gzFile *gz;

    if ((buf = (UINT8 *) malloc(IDA10_MAXRECLEN)) == NULL) {
        perror("malloc");
        exit(1);
    }

    ph = init(cmdline->myname, cmdline->argc, cmdline->argv);

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    while (1) {
        status = len = ida10ReadGz(gz, buf, IDA10_MAXRECLEN, &type, FALSE);
        if (status > 0) {
            isiPushRawPacket(ph, buf, len, ISI_TYPE_IDA10);
        } else {
            if (status == IDA10_EOF) {
                LogMsg(LOG_INFO, "input EOF");
                status = 0;
            } else {
                LogMsg(LOG_INFO, "ida10ReadGz: %s", strerror(errno));
                status = MY_MOD_ID;
            }
            LogMsg(LOG_INFO, "%llu packets sent", isiPushCount(ph));
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

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2011/08/04 22:07:22  dechavez
 * initial release
 *
 */
