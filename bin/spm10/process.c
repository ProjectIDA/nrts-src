#pragma ident "$Id: process.c,v 1.2 2016/04/30 19:09:10 dechavez Exp $"
/*======================================================================
 *
 *  Decode SPM strings and build packets
 *
 *====================================================================*/
#include "spm10.h"

#define MY_MOD_ID SPM10_MOD_PROCESS

static MUTEX mutex;
static BOOL LogRawOption = DEFAULT_LOGRAW;

#define MIN_TOKEN 11
#define MAX_TOKEN (MIN_TOKEN * 2)
#define DELIMITERS "-: =\t\n"

/* hard-coded sample rate */

#define SPM01_SINT (5 * NANOSEC_PER_SEC)

/* hard-coded channel name */

#define SPM01_CHN  "SPM"

/* hard-coded location codes */

#define SPM01_TEMPF   "KF"
#define SPM01_VDC     "DC"
#define SPM01_VAC1    "A1"
#define SPM01_VAC2    "A2"
#define SPM01_CURRENT "CR"

static BOOL LogRawEnabled()
{
BOOL retval;

    MUTEX_LOCK(&mutex);
        retval = LogRawOption;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void ToggleRawLogOption()
{
    MUTEX_LOCK(&mutex);
        LogRawOption = !LogRawOption;
    MUTEX_UNLOCK(&mutex);
    LogMsg(LOG_INFO, "input string logging %s\n", LogRawOption ? "enabled" : "disabled");
}

void ProcessString(char *input, char *sta, char *net, UINT32 options)
{
int i, ntoken, remain;
char *token[MAX_TOKEN], *copy, *chn;
LINE_SAMPLE sample;
static char *fid = "ProcessString";

    if (LogRawEnabled()) LogMsg(LOG_INFO, input);

    if ((copy = strdup(input)) == NULL) {
        LogMsg(LOG_INFO, "%s: strdup: %s\n", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    ntoken = utilParse(copy, token, DELIMITERS, MAX_TOKEN, 0);

    if (ntoken != MIN_TOKEN) {
        LogMsg(LOG_DEBUG, "%s: ntoken %d != %d: IGNORE: %s\n", fid, ntoken, MIN_TOKEN, input);
        free(copy);
        return;
    }

    sample.sint = (UINT64) SPM01_SINT;
    sample.gain = 1;

    sample.tstamp.month  = -1;
    sample.tstamp.day    = -1;
    sample.tstamp.year   = atoi(token[1]);
    sample.tstamp.jday   = atoi(token[2]);
    sample.tstamp.hour   = atoi(token[3]);
    sample.tstamp.minute = atoi(token[4]);
    sample.tstamp.second = atoi(token[5]);
    sample.tstamp.status = IDA10_GENTAG_LOCKED;
    BuildGentag(&sample.tstamp.gentag, &sample.tstamp);

    strncpy(sample.sname, sta,       SNAME_LEN+1); sample.sname[SNAME_LEN] = 0;
    strncpy(sample.nname, net,       NNAME_LEN+1); sample.nname[NNAME_LEN] = 0;
    strncpy(sample.cname, SPM01_CHN, CNAME_LEN+1); sample.cname[CNAME_LEN] = 0;
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.cname);

/* Token 6 is TempF */

    sample.value = atoi(token[6]);
    strncpy(sample.lname, SPM01_TEMPF, LNAME_LEN+1);
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.lname);
    sprintf(sample.chnloc, "%s%s", sample.cname, sample.lname);
    BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
    AppendSample(&sample);

/* Token 7 is VDC */

    sample.value = atoi(token[7]);
    strncpy(sample.lname, SPM01_VDC, LNAME_LEN+1);
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.lname);
    sprintf(sample.chnloc, "%s%s", sample.cname, sample.lname);
    BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
    AppendSample(&sample);

/* Token 8 is VAC1 */

    sample.value = atoi(token[8]);
    strncpy(sample.lname, SPM01_VAC1, LNAME_LEN+1);
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.lname);
    sprintf(sample.chnloc, "%s%s", sample.cname, sample.lname);
    BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
    AppendSample(&sample);

/* Token 9 is VAC2 */

    sample.value = atoi(token[9]);
    strncpy(sample.lname, SPM01_VAC2, LNAME_LEN+1);
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.lname);
    sprintf(sample.chnloc, "%s%s", sample.cname, sample.lname);
    BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
    AppendSample(&sample);

/* Token 10 is Current */

    sample.value = atoi(token[10]);
    strncpy(sample.lname, SPM01_CURRENT, LNAME_LEN+1);
    if (options & SPM10_OPTION_LCASE) util_lcase(sample.lname);
    sprintf(sample.chnloc, "%s%s", sample.cname, sample.lname);
    BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
    AppendSample(&sample);

    free(copy);
}

void InitStringProcessor(BOOL initialLogRawOption)
{
    MUTEX_INIT(&mutex);
    LogRawOption = initialLogRawOption;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: process.c,v $
 * Revision 1.2  2016/04/30 19:09:10  dechavez
 * fixed typo causing channel names to lack trailing null
 *
 * Revision 1.1  2016/04/28 23:01:50  dechavez
 * initial release
 *
 */
