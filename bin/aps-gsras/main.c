#pragma ident "$Id: main.c,v 1.1 2016/11/16 22:08:29 dechavez Exp $"
/*======================================================================
 *
 * Generate condensed APS output file, compute average azimuth
 *
 * GSRAS version
 *
 *====================================================================*/
#include "util.h"

#define NFIX 3
#define NSV  13

extern char *VersionIdentString;

typedef struct {
    REAL64 azimuth;
    int qual;
} READING;

static READING reading;
static BOOL PrintReadings = FALSE;
static BOOL Verbose = TRUE;

static void help(char *myname)
{
    fprintf(stderr,"usage: %s < input > output", myname);
    exit(1);
}

static void discard(char *input, int lineno, char *comment)
{
int i;

    if (lineno == 1) return; /* common case, don't bother to note it */

    if (Verbose) {
        fprintf(stderr, "IGNORE line %d (%s): ", lineno, comment);
        for (i = 0; i < strlen(input); i++) {
            if (isprint(input[i])) {
                printf("%c", input[i]);
            } else {
                printf("?");
            }
        }
        printf("\n");
    }
}

static BOOL ProcessTstring(char **token, int ntoken, char *input, int lineno)
{
int expected = 22;
static char *fid = "ProcessTstring";

    if (ntoken != expected) {
        if (Verbose) fprintf(stderr, "%s: expected %d tokens, got %d\n", fid, expected, ntoken);
        discard(input, lineno, "(wrong number of tokens)");
        return FALSE;
    }

    reading.azimuth = atof(token[2]);
    reading.qual = atoi(token[9]);

    return TRUE;
}

static void ClearReading()
{
    memset(&reading, 0, sizeof(READING));
}

static void SaveReading(LNKLST *list)
{
    if (!listAppend(list, &reading, sizeof(READING))) {
        perror("listAppend");
        exit(1);
    }
}

static BOOL Match(int have, int want)
{
    if (have == -1) return TRUE;
//printf("Match(%d, %d) = %s\n", want, have, (have >= want) ? "TRUE" : "FALSE");
    return (have >= want) ? TRUE : FALSE;
}

static void CleanCopy(char *copy, char *input)
{
int i, j;

    for (i=0,j=0; i < strlen(input); i++) if (isprint(input[i])) copy[j++] = input[i];
    copy[j] = 0;
}

static void ComputeMean(LNKLST *list, int qual)
{
LNKLST_NODE *crnt;
READING *entry;
int nsamp;
REAL64 sum, dev, sumsq, mean, stdev;

    nsamp = 0;
    sum = dev = sumsq = mean = stdev = 0.0;

/* average azimuth */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        entry = (READING *) crnt->payload;
        if (Match(entry->qual, qual)) {
            sum += entry->azimuth;
            ++nsamp;
        }
        crnt = listNextNode(crnt);
    }
    if (nsamp == 0) return;
    mean = sum / (REAL64) nsamp;

/* standard deviation */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        entry = (READING *) crnt->payload;
        if (Match(entry->qual, qual)) {
            dev = entry->azimuth - mean;
            sumsq += (dev * dev);
        }
        crnt = listNextNode(crnt);
    }
    stdev = sqrt(sumsq / (REAL64) (nsamp - 1));

    printf("for qual >= %2d ", qual);

    printf("azimuth = %5.1lf +/- %.1lf ",  mean, stdev);
    printf("(%d readings)\n", nsamp);
}

int main (int argc, char **argv)
{
#define MAXLINELEN 256
#define COMMENT    '#'
#define DELIMITERS ","
#define MAX_TOKEN   32
int i, nchar, lineno=0;
char input[MAXLINELEN+1];
char copy[MAXLINELEN+1];
char *token[MAX_TOKEN];
static char *Tstring = "$T";
char *expect;
int status, ntoken;
LNKLST *list;
int fix, nsv;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            PrintReadings = TRUE;
        } else if (strcmp(argv[i], "-q") == 0) {
            Verbose = FALSE;
        } else {
            fprintf(stderr, "unrecognized argument: %s\n", argv[i]);
            fprintf(stderr, "\nusage: [ -p ] < session.log > output\n");
            exit(1);
        }
    }

    if (Verbose) {
        nchar = printf("%s version %s\n", argv[0], VersionIdentString);
        for (i = 0; i < nchar-1; i++) printf("-"); printf("\n");
    }

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    ClearReading();
    expect = Tstring;
    while ((status = utilGetLine(stdin, input, MAXLINELEN, COMMENT, &lineno)) == 0) {
        CleanCopy(copy, input);
        ntoken = utilParse(copy, token, DELIMITERS, MAX_TOKEN, 0);
        if (strcmp(token[0], Tstring) == 0) {
            if (ProcessTstring(token, ntoken, input, lineno)) SaveReading(list);
        } else {
printf("token[0] = '%s', Tstring = '%s'\n", token[0], Tstring);
            discard(input, lineno, "(expecting $T)");
        }
    }

    if (status != 1) {
        perror("utilGetLine");
        exit(1);
    }

    ComputeMean(list,  00);
    ComputeMean(list,  10);
    ComputeMean(list,  20);
    ComputeMean(list,  30);
    ComputeMean(list,  40);
    ComputeMean(list,  50);
    ComputeMean(list,  60);
    ComputeMean(list,  70);
    ComputeMean(list,  80);
    ComputeMean(list,  90);
    ComputeMean(list, 100);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2016/11/16 22:08:29  dechavez
 * initial GSRAS release (modified from aps 1.4.1)
 *
 */
