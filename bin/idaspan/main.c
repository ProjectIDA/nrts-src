#pragma ident "$Id: main.c,v 1.4 2015/12/08 20:28:57 dechavez Exp $"
/*======================================================================
 *
 *  Print time range spanned by IDA data, gzip data automatically
 *  detected and decompresssed.
 *
 *====================================================================*/
#include "util.h"
#include "list.h"
#include "ida.h"
#include "ida10.h"
#include "zlib.h"

#define UNDEFINED_REV -1
#define FULL_STRING  0
#define YEAR_ONLY    1
#define YEAR_DAY_DIR 2

typedef struct {
    int yr;
    int da;
    int hr;
    int mn;
    int sc;
    int ms;
    BOOL host;
    double value;
} TSTAMP;

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [ rev=value -oneline ] < data_stream (gzip OK)\n", myname);
    fprintf(stderr,"\n");
    exit(1);
}

static TSTAMP SetTstamp(double dtag, IDA_EXTRA *extra)
{
TSTAMP tstamp;
int yr, da, hr, mn, sc, ms;

    tstamp.value = dtag;

    utilTsplit(tstamp.value, &tstamp.yr, &tstamp.da, &tstamp.hr, &tstamp.mn, &tstamp.sc, &tstamp.ms);

    if (extra->valid && tstamp.yr <= 2000) {
        tstamp.value = (double) (extra->tstamp + SAN_EPOCH_TO_1970_EPOCH);
        utilTsplit(tstamp.value, &tstamp.yr, &tstamp.da, &tstamp.hr, &tstamp.mn, &tstamp.sc, &tstamp.ms);
        tstamp.host = TRUE;
    } else {
        tstamp.host = FALSE;
    }

    return tstamp;
}

static void SetString(char *string, TSTAMP *tstamp, int what)
{
    switch (what) {
      case YEAR_ONLY:
        sprintf(string, "%04d", tstamp->yr);
        break;
      case YEAR_DAY_DIR:
        sprintf(string, "%04d/%03d", tstamp->yr, tstamp->da);
        break;
      default:
        if (!tstamp->host) {
            sprintf(string, "%04d:%03d-%02d:%02d", tstamp->yr, tstamp->da, tstamp->hr, tstamp->mn);
        } else {
            sprintf(string, "%04d:%03d-HH:HM", tstamp->yr, tstamp->da);
        }
    }
}

BOOL DecodeMKx(UINT8 *buffer, int rev, TSTAMP *tofs, TSTAMP *tols, IDA *ida)
{
IDA_DHDR hdr;

    if (ida_rtype(buffer, rev) != IDA_DATA) return FALSE;
    if (ida_dhead(ida, &hdr, buffer) != 0) return FALSE;

    *tofs = SetTstamp(hdr.beg.tru, &hdr.extra);
    *tols = SetTstamp(hdr.end.tru, &hdr.extra);

    return TRUE;
}

BOOL DecodeIDA10(UINT8 *buffer, TSTAMP *tofs, TSTAMP *tols)
{
IDA10_TSHDR hdr;

    if (ida10Type(buffer) != IDA10_TYPE_TS) return FALSE;
    if (ida10UnpackTSHdr(buffer, &hdr) == 0) return FALSE;

    *tofs = SetTstamp(hdr.tofs, &hdr.cmn.extra);
    *tols = SetTstamp(hdr.tols, &hdr.cmn.extra);

    return TRUE;
}

static int DetermineRev(gzFile gz, int rev)
{
static UINT8 buffer[IDA10_MAXRECLEN];

    if (rev != UNDEFINED_REV) return rev;

    if ((gzread(gz, buffer, IDA10_MAXRECLEN) < 0)) return UNDEFINED_REV;
    if ((rev = idaPacketFormat(buffer)) == 0) return UNDEFINED_REV;

    return rev;
}

static void UpdateYearDayList(LNKLST *list, TSTAMP *tstamp)
{
LNKLST_NODE *crnt;
#define STRLEN 9 /* yyyy/ddd+NULL = 9 */
static char *entry, string[STRLEN];

    sprintf(string, "%04d/%03d", tstamp->yr, tstamp->da);
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        entry = (char *) crnt->payload;
        if (strcmp(entry, string) == 0) return; /* already have this date */
        crnt = listNextNode(crnt);
    }

/* Haven't seen this date yet, add it to the list */

    if (!listAppend(list, string, STRLEN)) {
        perror("listAppend");
        exit(1);
    }
}

static void PrintAllYearDayPairs(LNKLST *list)
{
BOOL empty = TRUE;
LNKLST_NODE *crnt;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        if (!empty) printf(" ");
        printf("%s", (char *) crnt->payload);
        crnt = listNextNode(crnt);
        empty = FALSE;
    }
    if (!empty) printf("\n");
}

int main(int argc, char **argv)
{
char *dbspec=NULL, *log=NULL, *path=NULL;
int i, debug=-1, rev=UNDEFINED_REV, type, what=FULL_STRING;
BOOL first=TRUE, oneline=FALSE;
TSTAMP tofs, tols;
struct {
    TSTAMP tstamp;
    char string[64];
} oldest, yngest;
IDA ida;
gzFile gz;
FILE *fp = stdin;
UINT8 buffer[IDA10_MAXRECLEN];
static char *defdir = "/usr/nrts";
static LNKLST *list;

    if ((list = listCreate()) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("listCreate");
        exit(1);
    }

    idaInitHandle(&ida);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i]+strlen("rev="));
        } else if (strcasecmp(argv[i], "-oneline") == 0) {
            oneline = TRUE;
        } else if (strcasecmp(argv[i], "-y") == 0) {
            what = YEAR_ONLY;
        } else if (strcasecmp(argv[i], "-yd") == 0) {
            what = YEAR_DAY_DIR;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            path = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (path != NULL && (fp = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "%s: fopen: %s: %s\n", argv[0], path, strerror(errno));
        exit(1);
    }
    if ((gz = gzdopen(fileno(fp), "rb")) == NULL) {
        fprintf(stderr, "%s: gzdopen: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if ((rev = DetermineRev(gz, rev)) == UNDEFINED_REV) {
        fprintf(stderr, "%s: unable to determine packet format rev from the data!\n", argv[0]);
        help(argv[0]);
    }
    ida.rev.value = rev;

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);

/*  Select decoder function/handle */

    if (rev < 10) {
        if ((ida.db = dbioOpen(dbspec, NULL)) == NULL) {
            fprintf(stderr, "%s: dbioOpen: %s\n", argv[0], strerror(errno));
            exit(1);
        }
    } else {
        ida.db = NULL;
    }

/* Read/decode until done */

    strcpy(oldest.string, "undetermined");
    strcpy(yngest.string, "undetermined");

    if (rev < 10) {
        while (gzread(gz, buffer, IDA_BUFLEN) == IDA_BUFLEN) {
            if (DecodeMKx(buffer, rev, &tofs, &tols, &ida)) {
                if (first) {
                    oldest.tstamp = tofs;
                    yngest.tstamp = tols;
                    first = FALSE;
                } else {
                    if (tofs.value < oldest.tstamp.value) oldest.tstamp = tofs;
                    if (tols.value > yngest.tstamp.value) yngest.tstamp = tols;
                }
                if (what == YEAR_DAY_DIR) {
                    UpdateYearDayList(list, &tofs);
                    UpdateYearDayList(list, &tols);
                }
            }
        }
    } else {
        while (ida10ReadGz(gz, buffer, IDA10_MAXRECLEN, &type, FALSE) > 0) {
            if (DecodeIDA10(buffer, &tofs, &tols)) {
                if (first) {
                    oldest.tstamp = tofs;
                    yngest.tstamp = tols;
                    first = FALSE;
                } else {
                    if (tofs.value < oldest.tstamp.value) oldest.tstamp = tofs;
                    if (tols.value > yngest.tstamp.value) yngest.tstamp = tols;
                }
                if (what == YEAR_DAY_DIR) {
                    UpdateYearDayList(list, &tofs);
                    UpdateYearDayList(list, &tols);
                }
            }
        }
    }
    
    if (!first) {
        SetString(oldest.string, &oldest.tstamp, what);
        SetString(yngest.string, &yngest.tstamp, what);
    }

    switch (what) {
      case FULL_STRING:
        if (!oneline) {
            printf("%s\n%s\n", oldest.string, yngest.string);
        } else {
            printf("%s %s\n", oldest.string, yngest.string);
        }
        break;
      case YEAR_DAY_DIR: PrintAllYearDayPairs(list); break;
      default:           printf( "%s %s\n", oldest.string, yngest.string); break;
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.4  2015/12/08 20:28:57  dechavez
 * Cleaned up some benign UINT8 vs char confusion revealed by DARWIN builds
 *
 * Revision 1.3  2015/06/01 23:31:57  dechavez
 * added -oneline option
 *
 * Revision 1.2  2014/05/01 18:11:24  dechavez
 * print out all unique year/day pairs when -yd option is selected
 *
 * Revision 1.1  2014/05/01 17:39:05  dechavez
 * original version imported from src/bin/misc
 *
 */
