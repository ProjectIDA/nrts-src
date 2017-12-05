#pragma ident "$Id: main.c,v 1.4 2011/02/25 20:33:31 dechavez Exp $"
/*======================================================================
 *
 * Demultiplex IDA10 data in a manner consistent with IDA DCC processing
 *
 *====================================================================*/
#include "dccdmx.h"

#define MY_MOD_ID DCCDMX_MOD_MAIN

static COUNTER count;

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s volid [ if=path ]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Required Arguments:\n");
    fprintf(stderr,"volid        => volume identifier string (eg, sta123)\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Optional Arguments:\n");
    fprintf(stderr,"if=path      => input path (default stdin, gzip compressed OK)\n");
    fprintf(stderr,"\n");
    exit(1);
}

void ReportCounts(void)
{
    LogMsg("%7ld errors encountered\n", count.error);
    LogMsg("%7ld TS records\n", count.ts);
    LogMsg("%7ld CA records\n", count.ca);
    LogMsg("%7ld CF records\n", count.cf);
    LogMsg("%7ld LM records\n", count.lm);
    LogMsg("%7ld ISP log records\n", count.ii);
    LogMsg("%7ld ignored records\n", count.ignored);
    LogMsg("-------\n");
    LogMsg("%7ld records processed\n", count.total);
}

int main(int argc, char **argv)
{
int i, status, type;
INT32 nread;
char *ident = NULL, *ipath = NULL;
gzFile *gz;
FILE *fp = stdin;
UINT8 buf[IDA10_MAXRECLEN];
static char *fid = "main";

    InitLog(argv[0]);
    InitExit();

    for (i = 1; i < argc; i++) {
        if (ident == NULL) {
            ident = argv[i];
        } else if (strncasecmp(argv[i], "if=", strlen("if=")) == 0) {
            ipath = argv[i] + strlen("if=");
        } else if (strncasecmp(argv[i], "station=", strlen("station=")) == 0) {
            ;
        } else if (strncasecmp(argv[i], "jdate=", strlen("jdate=")) == 0) {
            ;
        } else {
            fprintf(stderr, "%s: unrecogized argument '%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    if (ident == NULL) {
        fprintf(stderr, "ERROR: missing `ident' command line argument\n");
        help(argv[0]);
    }

    if (ipath != NULL && (fp = fopen(ipath, "rb")) == NULL) {
        LogMsg("ERROR: %s: fopen: %s: %s\n", fid, ipath, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }

    if ((gz = gzdopen(fileno(fp), "r")) == NULL) {
        LogMsg("ERROR: %s: gzdopen: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 3);
    }

    InitTS(ident);
    InitLM(ident);
    InitCF(ident);
    InitCA(ident);
    InitII(ident);

    count.total = count.ts = count.lm = count.cf = count.ca = count.ii = count.ignored = count.error = 0;

    StartSignalHandler();

    LogMsg("%s - %s\n", argv[0], VersionIdentString);
    LogMsg("input = %s\n", ipath == NULL ? "stdin" : ipath);
    LogMsg("ident = %s\n", ident);

    while ((nread = ida10ReadGz(gz, buf, IDA10_MAXRECLEN, &type, FALSE)) > 0) {
        ++count.total;
        switch (type) {
          case IDA10_TYPE_TS: ProcessTS(buf, nread, &count); break;
          case IDA10_TYPE_LM: ProcessLM(buf, nread, &count); break;
          case IDA10_TYPE_CF: ProcessCF(buf, nread, &count); break;
          case IDA10_TYPE_CA: ProcessCA(buf, nread, &count); break;
          case IDA10_TYPE_ISPLOG: ProcessII(buf, nread, &count); break;
          default:
            LogMsg("record no. %ld ignored (unrecognized type '%d')\n", count.total, type);
            ++count.ignored;
        }
        CheckForShutdown();
    }
    status = nread;

    if (status != IDA10_EOF) LogMsg("%s: ida10ReadGz failed at record no. %d: %s\n", fid, count.total, ida10ErrorString(status));

    Exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.4  2011/02/25 20:33:31  dechavez
 * accept (and ignore) station= and jdate= command line options
 *
 * Revision 1.3  2011/02/25 19:36:10  dechavez
 * added TSheader (constant FALSE) to ida10ReadGz() parameters
 *
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
