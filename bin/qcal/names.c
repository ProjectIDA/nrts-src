#pragma ident "$Id: names.c,v 1.5 2016/04/20 17:28:01 dechavez Exp $"
/*======================================================================
 *
 *  Generate output data and log file names
 *
 *====================================================================*/
#include "qcal.h"

void BuildFileNames(QCAL *qcal, char *digitizer, char *caltag)
{
time_t now;
struct tm result;
char *format;
static UINT16 SensorBmask = 0x38;
static char *SensorAFormat = "CAL-%s-%s-%s-%04d-%02d%02d-%02d%02d";
static char *SensorBFormat = "CAL-%s-%s-B-%s-%04d-%02d%02d-%02d%02d";

    if (strlen(qcal->name.prefix) == 0) {
        now = time(NULL);
        localtime_r(&now, &result);
        format = (qcal->c1_qcal.chans & SensorBmask) ? SensorBFormat : SensorAFormat;
        sprintf(qcal->name.prefix, format,
            digitizer,
            qcal->addr.input.a.sensor.name,
            caltag,
            result.tm_year+1900,
            result.tm_mon+1,
            result.tm_mday,
            result.tm_hour,
            result.tm_min
        );
    }
    sprintf(qcal->name.qdp,   "%s.qdp.gz", qcal->name.prefix);
    sprintf(qcal->name.ida,   "%s.ida.gz", qcal->name.prefix);
    sprintf(qcal->name.log,   "%s.log",    qcal->name.prefix);
    sprintf(qcal->name.mseed, "%s.ms",     qcal->name.prefix);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: names.c,v $
 * Revision 1.5  2016/04/20 17:28:01  dechavez
 * include a "B" in the file names for sensor B calibrations
 *
 * Revision 1.4  2015/12/07 19:00:19  dechavez
 * added MiniSEED support
 *
 * Revision 1.3  2010/12/17 19:41:45  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.2  2010/11/24 22:07:41  dechavez
 * swap positions of caltag and sensor name in output file name prefix
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
