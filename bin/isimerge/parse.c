#pragma ident "$Id: parse.c,v 1.2 2010/09/17 20:02:46 dechavez Exp $"
/*======================================================================
 *
 *  Parse the source string and load the contants into the
 *  user provided SITE_PAR.
 *
 *  Syntax is site@server[:arg=value]
 *
 *  Where the optional colon(:) delmited arg=value strings may be
 *  repeated an arbitrary number of times.
 *
 *  This is called during initialization and before going into the
 *  background.  Therefore all error messages can go straight to stderr.
 *
 *====================================================================*/
#include "isimerge.h"

#define MY_MOD_ID ISI_MERGE_MOD_PARSE

#define MAX_COMMAND_LINE_LENGTH 255 /* arbitrary, but definitely "big enough" */
#define MAX_TOKEN MAX_COMMAND_LINE_LENGTH /* also "big enough" */

static BOOL GetSiteAndServer(char *string, SITE_PAR *site)
{
int ntoken;
char *token[MAX_TOKEN];
char copy[MAX_COMMAND_LINE_LENGTH+1];
static char *fid = "ParseSourceString:GetSiteAndServer";

    strncpy(copy, string, MAX_COMMAND_LINE_LENGTH); copy[MAX_COMMAND_LINE_LENGTH] = 0;
    ntoken = utilParse(copy, token, "@", MAX_TOKEN, 0);

    if (ntoken != 2 || strlen(token[0]) > ISI_SITELEN) return FALSE;

    strncpy(site->name, token[0], ISI_SITELEN);
    strncpy(site->server, token[1], MAXPATHLEN);

    return TRUE;
}

static BOOL DecodeOption(char *string, SITE_PAR *site)
{
int ntoken;
int sndbuf, rcvbuf;
char *token[MAX_TOKEN];
char copy[MAX_COMMAND_LINE_LENGTH+1];
static char *fid = "ParseSourceString:DecodeOption";

    isiGetTcpBuflen(&site->isi_param, &sndbuf, &rcvbuf); /* in case they change */

    strncpy(copy, string, MAX_COMMAND_LINE_LENGTH); copy[MAX_COMMAND_LINE_LENGTH] = 0;
    ntoken = utilParse(copy, token, "=", MAX_TOKEN, 0);
    if (ntoken != 2) {
        fprintf(stderr, "%s: bad argument sub-string '%s'\n", fid, copy);
        return FALSE;
    }

    if (strcmp(token[0], "port") == 0) {
        isiSetServerPort(&site->isi_param, atoi(token[1]));
    } else if (strcmp(token[0], "to") == 0) {
        isiSetTimeout(&site->isi_param, atoi(token[1]));
    } else if (strcmp(token[0], "sndbuf") == 0) {
        sndbuf = atoi(token[1]);
    } else if (strcmp(token[0], "rcvbuf") == 0) {
        rcvbuf = atoi(token[1]);
    } else if (strcmp(token[0], "beg") == 0) {
        if (!isiStringToSeqno(token[1], &site->beg)) {
            fprintf(stderr, "%s: bad '%s' argument\n", fid, token[1]);
            return FALSE;
        }
    } else if (strcmp(token[0], "end") == 0) {
        if (!isiStringToSeqno(token[1], &site->end)) {
            fprintf(stderr, "%s: bad '%s' argument\n", fid, token[1]);
            return FALSE;
        }
    }

    isiGetTcpBuflen(&site->isi_param, &sndbuf, &rcvbuf); /* in case they changed */

    return TRUE;
}

BOOL ParseSourceString(char *string, SITE_PAR *site)
{
int i, ntoken;
char *token[MAX_TOKEN];
char copy[MAX_COMMAND_LINE_LENGTH+1];
static char *fid = "ParseSourceString";

/* will parse a copy of the user supplied string */

    strncpy(copy, string, MAX_COMMAND_LINE_LENGTH); copy[MAX_COMMAND_LINE_LENGTH] = 0;

/* parse source string into site@server arg=val arg=val ... */

    ntoken = utilParse(copy, token, ":", MAX_TOKEN, 0);
    if (ntoken < 1) {
        fprintf(stderr, "%s: bad source string '%s'\n", fid, string);
        return FALSE;
    }

/* the first token better be site@server */

    if (!GetSiteAndServer(token[0], site)) return FALSE;

/* decode any of the optional arguments */

    for (i = 1; i < ntoken; i++) if (!DecodeOption(token[i], site)) return FALSE;
    site->next = site->beg;

    return TRUE;

}

/* Revision History
 *
 * $Log: parse.c,v $
 * Revision 1.2  2010/09/17 20:02:46  dechavez
 * fail without complaint if expected site@server string doesn't parse
 *
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
