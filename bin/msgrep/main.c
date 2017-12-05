#pragma ident "$Id"
/*======================================================================
 *
 *  Simple minded filter to extract data packets based on sta/chan/loc
 *
 *====================================================================*/
#include "mseed.h"
#include "util.h"

extern char *VersionIdentString;

#define DEFAULT_IFP stdin
#define DEFAULT_OFP stdout

typedef struct {
    char staid[MSEED_SNAMLEN+1];
    char chnid[MSEED_CNAMLEN+1];
    char locid[MSEED_LNAMLEN+1];
} SCL;

#define WILDCARD "*"
static LNKLST *list;

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [if=input_file of=output_file -v] { keep=sta.chn.loc,[sta.chn.loc,...] | reject=sta.chn.loc,[sta.chn.loc,...] }\n", myname);
    fprintf(stderr, "defaults: if=stdin of=stdout\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"You must specify EITHER keep= OR reject=\n");
    fprintf(stderr,"Be sure to quote wild cards, eg keep=\"pfo.*.*\" or keep=\"*.lhz.00\",\"pfo.bhz.10\"\n");
    fprintf(stderr,"\n");
    exit(1);
}

static BOOL ParseMatchStringEntry(char *entry)
{
int ntoken;
#define MAX_TOKEN 3
char *token[MAX_TOKEN];
char *copy;
SCL new;

    if ((copy = strdup(entry)) == NULL) {
        perror("strdup");
        exit(1);
    }

    ntoken = utilParse(copy, token, ".:-_", MAX_TOKEN, 0);
    switch (ntoken) {
      case 3:
        strncpy(new.staid, token[0], MSEED_SNAMLEN+1);
        strncpy(new.chnid, token[1], MSEED_CNAMLEN+1);
        strncpy(new.locid, token[2], MSEED_LNAMLEN+1);
        break;
      case 2:
        strncpy(new.staid, WILDCARD, MSEED_SNAMLEN+1);
        strncpy(new.chnid, token[0], MSEED_CNAMLEN+1);
        strncpy(new.locid, token[1], MSEED_LNAMLEN+1);
        break;
      case 1:
        strncpy(new.staid, WILDCARD, MSEED_SNAMLEN+1);
        strncpy(new.chnid, WILDCARD, MSEED_CNAMLEN+1);
        strncpy(new.locid, token[0], MSEED_LNAMLEN+1);
      default:
        fprintf(stderr, "Illegal SCL string '%s'\n", entry);
        return FALSE;
    }
    free(copy);

    if (!listAppend(list, (void *) &new, sizeof(SCL))) {
        perror("listAppend");
        return FALSE;
    }

    return TRUE;
}

static BOOL ParseMatchString(char *string)
{
int i, ntoken;
#define MAX_SCL 1000 /* REAL BIG */
char *token[MAX_SCL];
char *copy;
SCL new;

    if ((copy = strdup(string)) == NULL) {
        perror("strdup");
        exit(1);
    }

    ntoken = utilParse(copy, token, ",", MAX_SCL, 0);
    for (i = 0; i < ntoken; i++) if (!ParseMatchStringEntry(token[i])) return FALSE;
    free(copy);

    return TRUE;
}

static BOOL IsMatch(char *want, char *have)
{
    if (strcasecmp(want, WILDCARD) == 0) return TRUE;
    if (strcasecmp(want, have) == 0) return TRUE;

    return FALSE;
}

static BOOL SCLmatch(SCL *want, MSEED_HDR *have)
{
    if (!IsMatch(want->staid, have->staid)) return FALSE;
    if (!IsMatch(want->chnid, have->chnid)) return FALSE;
    if (!IsMatch(want->locid, have->locid)) return FALSE;

    return TRUE;
}

static BOOL WantRecord(MSEED_HDR *hdr, BOOL MatchAction)
{
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        if (SCLmatch((SCL *) crnt->payload, hdr)) return MatchAction;
        crnt = listNextNode(crnt);
    }

    return !MatchAction;
}

int main(int argc, char **argv)
{
int i;
FILE *ifp = DEFAULT_IFP;
FILE *ofp = DEFAULT_OFP;
MSEED_PACKED packed;
BOOL MatchAction;
char *MatchString = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            char *path;
            path = argv[i] + strlen("if=");
            if ((ifp = fopen(path, "rb")) == NULL) {
                fprintf(stderr, "%s: fopen: ", argv[0]);
                perror(path);
                exit(1);
            }
        } else if (strncmp(argv[i], "of=", strlen("of=")) == 0) {
            char *path;
            path = argv[i] + strlen("of=");
            if ((ofp = fopen(path, "wb")) == NULL) {
                fprintf(stderr, "%s: fopen: ", argv[0]);
                perror(path);
                exit(1);
            }
        } else if (strncmp(argv[i], "cf=", strlen("cf=")) == 0) {
            MatchString = argv[i] + strlen("cf=");
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            if (MatchString != NULL) help(argv[0]);
            MatchString = argv[i] + strlen("keep=");
            MatchAction = TRUE;
        } else if (strncmp(argv[i], "reject=", strlen("reject=")) == 0) {
            if (MatchString != NULL) help(argv[0]);
            MatchString = argv[i] + strlen("reject=");
            MatchAction = FALSE;
        } else if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-v") == 0) {
            MatchAction = FALSE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if ((list = listCreate()) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("listCreate");
        exit(1);
    }

    if (MatchString == NULL) help(argv[0]);

    if (!ParseMatchString(MatchString)) {
        fprintf(stderr, "%s: ParseMatchString failed", argv[0]);
        exit(1);
    }

    while (mseedReadPackedRecord(ifp, &packed)) {
        if (WantRecord(&packed.hdr, MatchAction)) {
            if (fwrite(packed.data, 1, packed.len, ofp) != packed.len) {
                fprintf(stderr, "%s: ", argv[0]);
                perror("fwrite");
                exit(1);
            }
        }
    }

    if (!feof(ifp)) fprintf(stderr, "%s: fread: %s\n", argv[0], strerror(errno));

    if (ifp != stdin) fclose(ifp);
    if (ofp != stdout) fclose(ofp);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2015/12/07 19:15:22  dechavez
 * fixed typo causing the wrong max length to be passed to strncpy() when setting locid
 *
 * Revision 1.1  2015/09/23 19:33:55  dechavez
 * initial release
 *
 */
