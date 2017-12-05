#pragma ident "$Id: e300.c,v 1.1 2014/10/21 21:54:11 dechavez Exp $"
/*======================================================================
 *
 *  E300 control
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_E300

static THREAD_FUNC E300PingThread(void *argptr)
{
Q330_CALIB *calib;

    calib = (Q330_CALIB *) argptr;

    while (1) {
        e300Ping(calib->input.e300.name);
        sleep(60);
    }
}

BOOL E300Control(Q330_CALIB *calib)
{
THREAD tid;
static char *fid = "E300Control";

    if (!calib->input.e300.present) return FALSE;

    if (!e300Extcal(calib->input.e300.name)) {
        fprintf(stderr, "%s: %s\n", calib->input.e300.name, strerror(errno));
        fprintf(stderr, "ERROR: can't configure E300 for external calibration\n");
        exit(MY_MOD_ID + 1);
    }

    if (!THREAD_CREATE(&tid, E300PingThread, (void *) calib)) {
        fprintf(stderr, "%s: THREAD_CREATE: %s\n", fid, strerror(errno));
        fprintf(stderr, "ERROR: can't launch E300PingThead\n");
        exit(MY_MOD_ID + 2);
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: e300.c,v $
 * Revision 1.1  2014/10/21 21:54:11  dechavez
 * created
 *
 */
