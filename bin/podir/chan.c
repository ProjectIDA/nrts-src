#pragma ident "$Id: chan.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Channel Loop
 *
 *====================================================================*/
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_CHAN

void ProcessChan(DCCDB *db, DCCDB_SITE *site, DCCDB_CHAN *chan, char *root)
{
int i;
FILE *fp;
LNKLST *list;
LNKLST_NODE *crnt;
char dirpath[MAXPATHLEN+1];
char path[MAXPATHLEN+1];
char LOC[DCCDB_CHAN_LOC_LEN+1]; /* forced upper case location code */
char CHN[DCCDB_CHAN_CHN_LEN+1]; /* forced upper case channel code */
static char *fid = "ProcessChan";

/* Create the chn.loc directory and the b052 file therein */

    strcpy(LOC, chan->loc); util_ucase(LOC);
    strcpy(CHN, chan->chn); util_ucase(CHN);
    if (LOC[0] != ' ' && LOC[1] != ' ') {
        sprintf(dirpath, "%s/%s.%s", root, CHN, LOC);
    } else {
        sprintf(dirpath, "%s/%s.", root, CHN);
    }
    if (util_mkpath(dirpath, 0775) != 0) {
        LogErr("%s: util_mkpath: ERROR: %s: %s\n", fid, dirpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    sprintf(path, "%s/B052", dirpath);
    if ((fp = fopen(path, "a+")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, path, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

/* Locate the corresponding cascades (as a link list... will need to free when done) */

    if ((list = dccdbLookupCascadeFromChan(db, chan)) == NULL) {
        LogErr("%s: dccdbLookupCascadeFromChan(sta=%s, chn=%s, loc=%s, begt=%17.5f, endt=%17.5f): %s\n",
            fid, chan->sta, chan->chn, chan->loc, chan->begt, chan->endt, strerror(errno)
        );
        GracefulExit(MY_MOD_ID + 3);
    }

/* Every chan should have a least one cascade (log cascaded is added by library) */

    if (list->count > 0) {
        crnt = listFirstNode(list);
        while (crnt != NULL) {
            ProcessCascade(fp, path, db, site, chan, (DCCDB_CASCADE *) crnt->payload);
            crnt = listNextNode(crnt);
        }
        listDestroy(list);
    } else {
        LogMsg(0, "%s: WARNING: no cascades associated with chan entry %s_%s_%s\n", fid, chan->sta, chan->chn, chan->loc);
    }

    fclose(fp);
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
 * $Log: chan.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
