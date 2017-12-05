#pragma ident "$Id: rev01.c,v 1.3 2011/10/19 21:29:27 dechavez Exp $"
/*======================================================================
 *
 *  Decode rev 1 data
 *
 *====================================================================*/
#include "txtoida10.h"

#define MIN_TOKEN 11
#define MAX_TOKEN 33
#define DELIMITERS "-: =\t\n"

void DecodeRev01(LNKLST *list, char *buf, UINT32 options)
{
#define FLOAT_TO_INT_FACTOR 100
int i, ntoken, remain;
char *token[MAX_TOKEN], *copy, *chn, *string;
LINE_SAMPLE sample;
static char *fid = "DecodeRev01";

    if ((copy = strdup(buf)) == NULL) {
        LogMsg(LOG_INFO, "%s: strdup: %s\n", fid, strerror(errno));
        exit(1);
    }

    ntoken = utilParse(copy, token, DELIMITERS, MAX_TOKEN, 0);

    if (ntoken <= MIN_TOKEN) {
        //LogMsg(LOG_INFO, "%s: ntoken %d < %d: IGNORE: %s\n", fid, ntoken, MIN_TOKEN, buf);
        free(copy);
        return;
    }

    remain = ntoken - MIN_TOKEN; /* better be just chan, value pairs left */
    if (remain % 2) {
        LogMsg(LOG_INFO, "%s: %d is not even: IGNORE: %s\n", fid, remain, buf);
        LogMsg(LOG_INFO, "%s: IGNORE: %s (unexpected format)\n", fid, buf);
        free(copy);
        return;
    }

    if (strcasecmp(token[6], "UT") != 0) {
        LogMsg(LOG_INFO, "%s: missing UT: IGNORE: %s\n", fid, buf);
        free(copy);
        return;
    }

    sample.tstamp.year   = atoi(token[0]);
    sample.tstamp.month  = atoi(token[1]);
    sample.tstamp.day    = atoi(token[2]);
    sample.tstamp.hour   = atoi(token[3]);
    sample.tstamp.minute = atoi(token[4]);
    sample.tstamp.second = atoi(token[5]);
    sample.tstamp.status = IDA10_GENTAG_LOCKED;
    BuildGentag(&sample.tstamp.gentag, &sample.tstamp);

    strncpy(sample.sname, token[7], SNAME_LEN+1);
    strncpy(sample.nname, token[8], NNAME_LEN+1);
    strncpy(sample.loc,   token[9], LNAME_LEN+1);

    sample.sint = (UINT64) (atof(token[10]) * NANOSEC_PER_SEC);

    for (i = 0; i <= remain / 2; i += 2) {
        chn = token[11+i];
        if (strlen(chn) > 3) {
            LogMsg(LOG_INFO, "%s: illegal chan name '%s': IGNORE: %s\n", fid, chn, buf);
            free(copy);
            return;
        }
        strcpy(sample.cname, chn);
        if (options & TXTOIDA10_OPTION_LCASE) {
            util_lcase(sample.cname);
            util_lcase(sample.loc);
        }
        sprintf(sample.chnloc, "%s%s", sample.cname, sample.loc);
        BuildIdentString(sample.ident, sample.nname, sample.sname, sample.chnloc);
        string = token[12+i];
        if (strchr(string, '.') != NULL) { /* has a decimal point */
            sample.gain = FLOAT_TO_INT_FACTOR;
            sample.value = (INT32) (atof(string) * sample.gain);
        } else {
            sample.gain = 1;
            sample.value = (INT32) (atoi(string));
        }
        AppendSample(list, &sample);
    }

    free(copy);
}

/* Revision History
 *
 * $Log: rev01.c,v $
 * Revision 1.3  2011/10/19 21:29:27  dechavez
 * fixed bug where x.00 floating values were not multiplied by 100 before int conversion
 *
 * Revision 1.2  2011/10/12 17:50:52  dechavez
 * changed stderr output to logio (via LogMsg)
 *
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
