#pragma ident "$Id: b57.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 57 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B57

void WriteBlockette57(FILE *fp, char *fpath, DCCDB_STAGE *stage, REAL64 delay)
{
MSEED_B57 b57;
static char *fid = "WriteBlockette57";

    b57.stageid  = stage->stageid;
    b57.factor   = stage->decifac;
    b57.offset   = 0;
    b57.irate    = stage->srate;
    b57.estdelay = b57.correction = delay;

    if (!mseedWriteBlockette57(fp, &b57)) {
        LogErr("%s: mseedWriteBlockette57: %s: %s\n", fid, fpath, strerror(errno));
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
 * $Log: b57.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
