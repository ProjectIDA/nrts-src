#pragma ident "$Id: process.c,v 1.13 2010/03/22 21:54:50 dechavez Exp $"
/*======================================================================
 *
 *  Call back routine for all high level packets.
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_PROCESS

static QHLP_DL_FUNC WriteToDiskLoop;      /* function to reformat HLP and write it to disk loop */

/* Initialize the packet processor */

BOOL InitPacketProcessor(Q330_CFG *cfg, ISI_DL *dl, char *metadir, QHLP_DL_FUNC saveFunc, UINT32 trigflags)
{
static char *fid = "InitPacketProcessor";

    if (dl == NULL || metadir == NULL || saveFunc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    WriteToDiskLoop = saveFunc;

    if (!InitTrigger(cfg, dl, metadir, saveFunc, trigflags)) return FALSE;

    return TRUE;
}

/* Process a newly created high level packet */

void ProcessHLP(void *arg, QDP_HLP *hlp)
{
DISK_LOOP_DATA *output;
static char *fid = "ProcessHLP";

    logioUpdateWatchdog(Watch, fid);

    if (arg == NULL || hlp == NULL) {
        LogMsg("%s: NULL input(s)!\n", fid);
        Exit(MY_MOD_ID + 8);
    }

    output = (DISK_LOOP_DATA *) arg;

    ProcessTrigChan(output, hlp);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: process.c,v $
 * Revision 1.13  2010/03/22 21:54:50  dechavez
 * Removed all special handling of calibration monitor data
 *
 * Revision 1.12  2010/03/12 00:50:45  dechavez
 * moved all the trigger stuff to trigger.c (new) and added code to handle calibration monitor "trigger"
 *
 * Revision 1.11  2009/07/27 17:39:32  dechavez
 * ing q330GetX instead of q330LookupX (libq3301.1.0)
 *
 * Revision 1.10  2009/07/09 18:38:05  dechavez
 * Changed trigger for calibration events to use lcq cal_in_progress, thus eliminating
 * the reliance on c1_stat packets in the data stream.
 * Cleaned up the logic flow of the trigger state switch.
 *
 * Revision 1.9  2009/07/08 16:03:03  dechavez
 * Fixed multiple Q330 automatic calibration detrigger bug
 *
 * Revision 1.8  2009/07/02 23:09:08  dechavez
 * added automatic calibration trigger support
 *
 * Revision 1.7  2009/07/01 18:57:24  dechavez
 * Graceful handling of misconfigured detector channel list
 *
 * Revision 1.6  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.5  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.4  2008/01/07 22:09:53  dechavez
 * 1.3.4
 *
 * Revision 1.3  2007/09/26 23:15:40  dechavez
 * always update pre-event memory (even during detections), look for retriggers in post-event phase
 *
 * Revision 1.2  2007/09/25 22:28:57  dechavez
 * Debugged event trigger flows, works OK with libdetect 1.0.0 STA/LTA
 *
 * Revision 1.1  2007/09/22 00:27:55  dechavez
 * Initial release.  Not tested with real detector as calls to libdetect
 * only find stubs in this build.
 *
 */
