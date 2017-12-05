#pragma ident "$Id: timer.c,v 1.1 2017/09/28 18:20:44 dauerbach Exp $"
/*======================================================================
 *
 *  Wait until it's time to grab a scan
 *
 *====================================================================*/
#include "siomet.h"

#define MY_MOD_ID SIOMET_MOD_TIMER

/* Current time stamp (to the closest second) from host clock
 * in nanoseconds since 1999 (SAN Epoch)
 */

INT64 CurrentSecond()
{
#define EpochConversionFactor (SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC)
INT64 tstamp;

    tstamp = utilCurrentSecond() - EpochConversionFactor;
    return tstamp;
}

INT64 WaitForSampleTime(SIOMET_INFO *info, INT64 target)
{
INT64 now, PollInterval, NanoPollInterval;

    PollInterval = (info->sint / 4);
    NanoPollInterval = PollInterval * NANOSEC_PER_MSEC;

    now = CurrentSecond();
    while ((target - now) > NanoPollInterval) {
        utilDelayMsec(PollInterval);
        now = CurrentSecond();
    }

    return now;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: timer.c,v $
 * Revision 1.1  2017/09/28 18:20:44  dauerbach
 * initial release
 *
 */
