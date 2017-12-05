#pragma ident "$Id: req.c,v 1.4 2015/09/30 20:25:46 dechavez Exp $"
/*======================================================================
 *
 *  POD.req utilities
 *
 *====================================================================*/
#include "podir.h"

static FILE *reqfp = NULL;
static char net[MSEED_NNAMLEN+1];

BOOL CreateReq(DCCDB *db, char *outdir, char *net_id)
{
int i;
static char path[MAXPATHLEN+1];
static char *fid = "CreateReq";

    sprintf(path, "%s/%s", outdir, PODIR_REQ_FILE_NAME);
    if ((reqfp = fopen(path, "w")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, path, strerror(errno));
        return FALSE;
    }
    strcpy(net, net_id);

    return TRUE;
}

void CloseReq(void)
{
    if (reqfp == NULL) return;
    fprintf(reqfp, "\n");
    fclose(reqfp);
}

void UpdateReq(DCCDB_CASCADE *cascade)
{
char CHN[DCCDB_CHAN_CHN_LEN+1]; /* forced upper case channel code */ 
char LOC[DCCDB_CHAN_LOC_LEN+1]; /* forced upper case location code */ 
char begstr[MSEED_TIMELEN+1], endstr[MSEED_TIMELEN+1];
static char *dummy = "dummy";
static char  *hdir = PODIR_HEADER_DIR_NAME;

    strcpy(CHN, cascade->chn); util_ucase(CHN);
    strcpy(LOC, cascade->loc); util_ucase(LOC);

    mseedSetTIMEstring(begstr, utilConvertFrom1970SecsTo1999Nsec(cascade->begt), TRUE);
    mseedSetTIMEstring(endstr, utilConvertFrom1970SecsTo1999Nsec(cascade->endt), TRUE);

    fprintf(reqfp, "%s\t", cascade->sta);          /* station                          */
    fprintf(reqfp, "%s\t", net);                   /* network                          */
    fprintf(reqfp, "%s\t", CHN);                   /* channel                          */
    fprintf(reqfp, "%s\t", LOC);                   /* location                         */
    fprintf(reqfp, "%s\t", begstr);                /* beg time in SEED format          */
    fprintf(reqfp, "%s\t", endstr);                /* end time in SEED format          */
    fprintf(reqfp, "%s\t", dummy);                 /* channel (data records) file name */
    fprintf(reqfp, "%s\t", hdir);                  /* header directory                 */
    fprintf(reqfp, "%s\t", begstr);                /* request beg time that user wants */
    fprintf(reqfp, "%s\n", endstr);                /* request end time that user wants */
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
 * $Log: req.c,v $
 * Revision 1.4  2015/09/30 20:25:46  dechavez
 * initial production release
 *
 */
