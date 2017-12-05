#pragma ident "$Id: b52.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Blockette 52 stuff
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_B52

void WriteBlockette52(FILE *fp, char *fpath, DCCDB *db, DCCDB_SITE *site, DCCDB_CHAN *chan, DCCDB_STAGE *stage, REAL64 srate)
{
REAL64 endt;
MSEED_B52 b52;
char comment[MSEED_B52_COMMENT_LEN+1];
static char *fid = "WriteBlockette52";

    comment[0] = 0;

    strncpy(b52.loc, chan->loc, MSEED_B52_LOC_LEN+1); util_ucase(b52.loc);
    strncpy(b52.chn, chan->chn, MSEED_B52_CHN_LEN+1); util_ucase(b52.chn);
    if (stage != NULL) {
        if (strlen(stage->ssident) > 0) snprintf(comment, MSEED_B52_COMMENT_LEN+1, "S/N %s", stage->ssident);
        b52.runitid = dccdbLookupUnitCode(db, stage->iunits);
        b52.cunitid = dccdbLookupUnitCode(db, stage->ounits);
        endt = (chan->endt > stage->endt) ? stage->endt : chan->endt;
    } else {
        b52.runitid = 0;
        b52.cunitid = 0;
        endt = chan->endt;
    }
    snprintf(b52.flag, MSEED_B52_FLAG_LEN+1, "%s", chan->flag);
    snprintf(b52.comment, MSEED_B52_COMMENT_LEN+1, "%s", comment);

    b52.subchn    = MSEED_B52_DEFAULT_SUBCHN;
    b52.instid    = dccdbLookupAbbrevCode(db, chan->instype);
    b52.lat       = site->lat;
    b52.lon       = site->lon;
    b52.elev      = site->elev * METERS_PER_KILOMETER;
    b52.depth     = chan->edepth * METERS_PER_KILOMETER;
    b52.azimuth   = chan->hang;
    b52.dip       = chan->vang - 90.0;
    b52.dfcode    = MSEED_B52_DEFAULT_DFCODE;
    b52.drlen     = MSEED_B52_DEFAULT_DRLEN;
    b52.srate     = srate;
    b52.maxdrift  = MSEED_B52_CLOCK_TOLERANCE(b52.srate); /* SHOULD REALLY BE DIGITIZER + CLOCK DEPENDENT! */
    b52.ncomments = MSEED_B52_DEFAULT_NCOMMENTS;
    b52.beg       = ConvertDccdbBegtTo1999Nsec(chan->begt);
    b52.end       = ConvertDccdbEndtTo1999Nsec(endt);
    b52.update    = MSEED_B52_DEFAULT_UPDATE;

    if (!mseedWriteBlockette52(fp, &b52)) {
        LogErr("%s: mseedWriteBlockette52: %s: %s\n", fid, fpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
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
 * $Log: b52.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
