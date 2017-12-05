#pragma ident "$Id: b58.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 58 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B58

void WriteBlockette58(FILE *fp, char *fpath, int stageid, REAL64 gain, REAL64 freq)
{
MSEED_B58 b58;
static char *fid = "WriteBlockette58";

    b58.stageid = stageid;
    b58.gain = gain;
    b58.freq = freq;

    if (!mseedWriteBlockette58(fp, &b58)) {
        LogErr("%s: mseedWriteBlockette58: %s: %s\n", fid, fpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: b58.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */

