#pragma ident "$Id: main.c,v 1.10 2014/09/11 21:53:55 dechavez Exp $"
/*======================================================================
 *
 * Generate condensed APS output file, compute average azimuth
 *
 *====================================================================*/
#include "util.h"

#define NFIX 3
#define NSV  13

extern char *VersionIdentString;

typedef struct {
    int nsv;
    int fix;
    char *tstamp;
    char *lat;
    char *dlat;
    char *lon;
    char *dlon;
    REAL64 azimuth;
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

static void ProcessPLTIT(char **token, int ntoken, char *input, int lineno)
{
int expected = 7;
static char *fid = "ProcessPLTIT";

    if (ntoken != expected) {
        if (Verbose) fprintf(stderr, "%s: expected %d tokens, got %d\n", fid, expected, ntoken);
        discard(input, lineno, "(wrong number of tokens)");
        return;
    }

    reading.azimuth = atof(token[2]);
}

static BOOL ProcessGPGGA(char **token, int ntoken, char *input, int lineno)
{
int expected = 15;
static char *fid = "ProcessGPGGA";

    if (ntoken != expected && ntoken > 10 && strcasecmp(token[10], "M") != 0) {
        if (Verbose) fprintf(stderr, "%s: expected %d tokens, got %d, token[10]='%s'\n", fid, expected, ntoken, token[10]);
        discard(input, lineno, "(wrong number of tokens)");
        return FALSE;
    }

    reading.tstamp = token[1];
    reading.lat = token[2];
    reading.dlat = token[3];
    reading.lon = token[4];
    reading.dlon = token[5];
    reading.fix = atoi(token[6]);
    reading.nsv = atoi(token[7]);

    if (reading.fix >= NFIX) {
        discard(input, lineno, "(bad 'fix' value)");
        return FALSE;
    }
    if (reading.nsv >= NSV) {
        discard(input, lineno, "(bad 'nsv' value)");
        return FALSE;
    }

    return TRUE;
}

static void ClearReading()
{
    memset(&reading, 0, sizeof(READING));
}

static void SaveReading(LNKLST *list)
{
    if (PrintReadings) {
        printf("%s ", reading.tstamp);
        printf("%s%s ", reading.lat, reading.dlat);
        printf("%s%s ", reading.lon, reading.dlon);
        printf("%d ", reading.fix);
        printf("%2d ", reading.nsv);
        printf("%5.1lf\n", reading.azimuth);
    }

    if (!listAppend(list, &reading, sizeof(READING))) {
        perror("listAppend");
        exit(1);
    }
}

static BOOL Match(int want, int have)
{
    if (have == -1) return TRUE;
    return (have == want) ? TRUE : FALSE;
}

static void CleanCopy(char *copy, char *input)
{
int i, j;

    for (i=0,j=0; i < strlen(input); i++) if (isprint(input[i])) copy[j++] = input[i];
    copy[j] = 0;
}

static void ComputeMean(LNKLST *list, int fix, int nsv)
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
        if (Match(entry->fix, fix) && Match(entry->nsv, nsv)) {
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
        if (Match(entry->fix, fix) && Match(entry->nsv, nsv)) {
            dev = entry->azimuth - mean;
            sumsq += (dev * dev);
        }
        crnt = listNextNode(crnt);
    }
    stdev = sqrt(sumsq / (REAL64) (nsamp - 1));

    if (fix > 0 && nsv > 0) {
        printf("for fix=%d, satellites=%2d, ", fix, nsv);
    } else {
        printf("-----------------------------------\n");
        printf("for all readings          ");
    }

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
static char *PLTIT = "$PLTIT";
static char *GPGGA = "$GPGGA";
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
        for (i = 0; i < nchar; i++) printf("-"); printf("\n");
    }

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    ClearReading();
    expect = PLTIT;
    while ((status = utilGetLine(stdin, input, MAXLINELEN, COMMENT, &lineno)) == 0) {
        CleanCopy(copy, input);
        ntoken = utilParse(copy, token, DELIMITERS, MAX_TOKEN, 0);
        if (strcmp(token[0], PLTIT) == 0) {
            if (expect == PLTIT) {
                ProcessPLTIT(token, ntoken, copy, lineno);
                expect = GPGGA;
            } else {
                discard(input, lineno, "(expecting $GPGGA)");
            }
        } else if (strcmp(token[0], GPGGA) == 0) {
            if (expect == GPGGA) {
                if (ProcessGPGGA(token, ntoken, copy, lineno)) SaveReading(list);
                expect = PLTIT;
            } else {
                discard(input, lineno, "(expecting $PLTIT)");
            }
        } else {
            discard(input, lineno, expect == PLTIT ? "(expecting $PLTIT)" : "(expecting $GPGGA)");
        }
    }

    if (status != 1) {
        perror("utilGetLine");
        exit(1);
    }

    if (Verbose) {
        for (fix = 0; fix < NFIX; fix++) for (nsv = 0; nsv < NSV; nsv++) ComputeMean(list, fix, nsv);
        ComputeMean(list, -1, -1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.10  2014/09/11 21:53:55  dechavez
 * fixed "first line bug" (now we toss first line until we get a PLTIT, which contains the azimuth).
 *
 * Revision 1.9  2014/07/07 18:10:10  dechavez
 * include line number and display unprintable characters as '?' when noting ignored lines
 *
 * Revision 1.8  2014/07/07 17:51:52  dechavez
 * Strip out all non-printable characters from input before processing.
 * Fixed some printf format string vs data type errors.
 *
 * Revision 1.7  2014/04/15 20:56:27  dechavez
 * added some bounds checking to ProcessGPGGA()
 *
 * Revision 1.6  2013/07/09 18:43:02  dechavez
 * added -p and -q options
 *
 * Revision 1.5  2012/02/28 17:35:00  dechavez
 * print out version ident
 *
 * Revision 1.4  2012/02/27 22:53:59  dechavez
 * Fixed bug where final average was just the final reading(!).  Allow for GPGGA
 * strings with less than 15 tokens as long as the units field is "M".
 *
 * Revision 1.3  2011/12/15 21:36:43  dechavez
 * don't print out reformatted readings, compute ave/stdev for all valid readings
 *
 * Revision 1.2  2011/12/15 18:32:32  dechavez
 * Fix bug computing mean, and prevent divide by zero error
 *
 * Revision 1.1  2011/11/29 22:22:53  dechavez
 * first release
 *
 */
