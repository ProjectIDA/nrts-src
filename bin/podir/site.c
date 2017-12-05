#pragma ident "$Id: site.c,v 1.4 2015/09/30 20:25:46 dechavez Exp $"
/*======================================================================
 *
 *  Station Loop
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_SITE

#define SiteNameMatches(site, chan) (strcmp(site->sta, chan->sta) == 0 ? TRUE : FALSE)

static BOOL EpochsIntersect(DCCDB_SITE *site, DCCDB_CHAN *chan)
{
    if (chan->endt < site->begt) return FALSE; /* chan epoch is fully before the site epoch */
    if (chan->begt > site->endt) return FALSE; /*   "    "    "    "  after   "    "    "   */

    return TRUE;
}

void ProcessSite(DCCDB *db, DCCDB_SITE *site, char *net_id, char *root)
{
int i;
FILE *fp;
int net_code;
MSEED_B50 b50;
DCCDB_CHAN *chan;
char dirpath[MAXPATHLEN+1], b50path[MAXPATHLEN+1];
static char *fid = "ProcessSite";

/* Create the site directory and blockette 50 file */

    sprintf(dirpath, "%s/%s.%s", root, site->sta, net_id);
    if (util_mkpath(dirpath, 0775) != 0) {
        LogErr("%s: util_mkpath: ERROR: %s: %s\n", fid, dirpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
    LogMsg(2, "site directory '%s' created OK\n", dirpath);

    sprintf(b50path, "%s/B050", dirpath);
    if ((fp = fopen(b50path, "a+")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, b50path, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

/* Generate blockette 50 from site table */

    if ((net_code = dccdbLookupAbbrevCode(db, net_id)) < 0) {
        LogErr("ERROR: missing '%s' table entry for network code '%s'\n", DCCDB_ABBREV_TABLE_NAME, net_id);
        GracefulExit(MY_MOD_ID + 3);
    }

    WriteBlockette50(fp, b50path, site, net_code, net_id);
    LogMsg(2, "station descriptor '%s' created OK\n", b50path);
    fclose(fp);

/* Loop over all channels for this station */


    for (i = 0; i < db->nchan; i++) {
        chan = &db->chan[i]; /* to make code easier to type and read */
        if (SiteNameMatches(site, chan) && EpochsIntersect(site, chan)) {
            LogMsg(2, "ProcessChan(db, '%s', ,'%s_%s_%s', '%s')\n", site, db->chan[i].sta, db->chan[i].chn, db->chan[i].loc, dirpath);
            ProcessChan(db, site, &db->chan[i], dirpath);
        }
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
 * $Log: site.c,v $
 * Revision 1.4  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
