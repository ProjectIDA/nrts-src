#pragma ident "$Id: select.c,v 1.2 2015/09/30 20:25:46 dechavez Exp $"
/*======================================================================
 *
 *  code to manage the station selection list
 *
 *====================================================================*/
#include "podir.h"

static LNKLST *list = NULL;
#define PODIR_ACTION_EXCLUDE 1
#define PODIR_ACTION_INCLUDE 2
static int action = PODIR_ACTION_EXCLUDE;
static BOOL IgnoreCase = TRUE;

static BOOL UcaseAllNames()
{
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        util_ucase((char *) crnt->payload);
        crnt = listNextNode(crnt);
    }
}

static BOOL SiteNameMatch(char *target)
{
char *sname;
LNKLST_NODE *crnt;

    if (list == NULL || target == NULL) return FALSE;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        sname = (char *) crnt->payload;
        if (IgnoreCase) {
            if (strcasecmp(sname, target) == 0) return TRUE;
        } else {
            if (strcmp(sname, target) == 0) return TRUE;
        }
        crnt = listNextNode(crnt);
    }

    return FALSE;
}

BOOL ProcessThisSite(DCCDB_SITE *site)
{
    if (action == PODIR_ACTION_EXCLUDE) {
        return SiteNameMatch(site->sta) ? FALSE : TRUE;
    } else {
        return SiteNameMatch(site->sta) ? TRUE : FALSE;
    }
}

char *StationSelectionString(char *buf, int buflen)
{
char *sname;
LNKLST_NODE *crnt;

    if (list == NULL) {
        sprintf(buf, "[none]");
        return buf;
    }

    buf[0] = 0;
    if (action == PODIR_ACTION_INCLUDE) sprintf(buf, "all but");
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        sname = (char *) crnt->payload;
        if (buf[0] != 0) {
            if (buflen - strlen(buf) > strlen(sname)+2) {
                strcat(buf, " ");
                strcat(buf, sname);
            }
        } else {
            strncpy(buf, sname, buflen);
        }
        crnt = listNextNode(crnt);
    }
    return buf;
}

BOOL BuildStationSelectionList(char *input, int ActionFlag, BOOL CaseFlag)
{
static char *delimiters = ",";
static char comment = 0;
static char *fid = "BuildStationSelectionList";

    if (input == NULL) return;
    if (list != NULL) {
        fprintf(stderr, "keep/KEEP/exclude/EXCLUDE options can only be specified once\n");
        return FALSE;
    }

    if ((list = utilStringTokenList(input, delimiters, comment)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList('%s'): %s\n", fid, input, strerror(errno));
        return FALSE;
    }

    action = ActionFlag;
    if (IgnoreCase = CaseFlag) UcaseAllNames();

    return TRUE;
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
 * $Log: select.c,v $
 * Revision 1.2  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
