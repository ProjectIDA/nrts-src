#pragma ident "$Id: stats.c,v 1.1 2011/01/25 18:35:40 dechavez Exp $"
/*======================================================================
 * 
 * Manage the I/O statistics
 *
 *====================================================================*/
#include "qio.h"

void qioInitStats(QIO_STATS *stats)
{
    MUTEX_INIT(&stats->mutex);
    stats->total.bytes = stats->total.pkts = (UINT64) 0;
    stats->len.min = stats->len.max = 0;
    stats->tstamp = utilTimeStamp();
    stats->error.count = stats->error.tstamp = 0;
}
     
void qioUpdateStats(QIO_STATS *stats, int nbytes, UINT64 tstamp)
{
    MUTEX_LOCK(&stats->mutex);
        if (nbytes > 0) {
            stats->total.bytes += nbytes;
            ++stats->total.pkts;
            if (stats->len.min == 0 || nbytes < stats->len.min) stats->len.min = nbytes;
            if (nbytes > stats->len.max) stats->len.max = nbytes;
            stats->tstamp = tstamp;
        } else {
            ++stats->error.count;
            stats->error.tstamp = tstamp;
        }
    MUTEX_UNLOCK(&stats->mutex);
}

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
 * $Log: stats.c,v $
 * Revision 1.1  2011/01/25 18:35:40  dechavez
 * initial release
 *
 */
