#pragma ident "$Id: help.c,v 1.9 2015/12/07 19:04:34 dechavez Exp $"
/*======================================================================
 *
 *  Help message
 *
 *====================================================================*/
#include "qcal.h"

void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "%s - %s\n", myname, VersionIdentString);
    fprintf(stderr, "usage: %s digitizer[:port:tokens:sta:net] caltag [ options ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments\n");
    fprintf(stderr, "digitizer  -> digitizer name (eg, xxx00)\n");
    fprintf(stderr, "caltag     -> calibration tag (eg, rbhf)\n");
    fprintf(stderr,"\n");
    fprintf(stderr, "Optional arguments\n");
    fprintf(stderr, "port          -> data port for calibration output (default=%s)\n", qdpDataPortString(DEFAULT_PORT));
    fprintf(stderr, "tokens        -> lcq tag for data port config\n");
    fprintf(stderr, "sta           -> station name\n");
    fprintf(stderr, "net           -> network name\n");
    fprintf(stderr, "cal=chans     -> calibrate channels, (default cal=123)\n");
    fprintf(stderr, "mon=chans     -> monitor channels, (default mon=4)\n");
    fprintf(stderr, "cfg=name      -> explicit path to Q330 cfg file\n");
    fprintf(stderr, "wdsec=secs    -> set watchdog exit interval\n");
    fprintf(stderr, "debug=0|1|2   -> debug level (default=0)\n");
    fprintf(stderr, "prefix=string -> output prefix (default=CAL-digitizer-sensor-caltag-date)\n");
    fprintf(stderr, "-q            -> suppress commentary\n");
    fprintf(stderr, "-lax          -> unpack all channels, even those with no naming tokens\n");
    fprintf(stderr, "-mseed        -> save MiniSEED data (default)\n");
    fprintf(stderr, "-ida10        -> save IDA10 data\n");
    fprintf(stderr, "-qdp          -> save QDP data\n");
    fprintf(stderr, "\n");
    exit(1);
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
 * $Log: help.c,v $
 * Revision 1.9  2015/12/07 19:04:34  dechavez
 * include version number in help message, note default mon and cal options, added -mseed option
 *
 * Revision 1.8  2014/06/12 20:20:16  dechavez
 * added -lax and -qdp to help, moved -q down to the bottom with these
 *
 * Revision 1.7  2012/01/11 19:22:36  dechavez
 * added wdsec to help message, fixed the way default port is printed
 *
 * Revision 1.6  2011/12/20 16:39:28  dechavez
 * removed -V from help (shouldn't even be there, really)
 *
 * Revision 1.5  2011/12/15 21:51:35  dechavez
 * added -V option
 *
 * Revision 1.4  2011/04/16 04:52:56  dechavez
 * added cal and mon options to override default calibration and monitor channels
 *
 * Revision 1.3  2011/04/14 19:22:09  dechavez
 * added QCAL.token support
 *
 * Revision 1.2  2010/11/24 22:09:03  dechavez
 * change example caltag to rbhf
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
