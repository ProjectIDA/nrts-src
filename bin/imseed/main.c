#pragma ident "$Id: main.c,v 1.19 2017/11/01 21:39:10 dechavez Exp $"
/*======================================================================
 *
 *  IDA to MiniSEED filter - uses a modified version of the IRIS DMC
 *  libmseed (modified to include support for FSDH flags and blockette
 *  1001 timing quality).
 *
 *====================================================================*/
#include "imseed.h"

#define MY_MOD_ID MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "usage: %s sta=name [options] < IDA input (gziped OK)\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments:\n");
    fprintf(stderr, "       sta=name => station code\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Optional arguments:\n");
    fprintf(stderr, "                   net=name => network code (default '%s', no quotes)\n", DEFAULT_NET);
    fprintf(stderr, "                  q=D|R|Q|M => FSDH data quality code (default '%c')\n", DEFAULT_QCODE);
    fprintf(stderr, "                   log=spec => log file (set spec to '-' for stdout or 'syslogd' for syslog, no quotes)\n");
    fprintf(stderr, "                  bs=reclen => output record length (default %d)\n", DEFAULT_RECLEN);
    fprintf(stderr, "             locked=integer => GPS clock locked percentage (default %d)\n", MSEED_DEFAULT_CLOCK_LOCKED_PERCENT);
    fprintf(stderr, "            suspect=integer => GPS time suspect percentage (default %d)\n", MSEED_DEFAULT_CLOCK_SUSPECT_PERCENT);
    fprintf(stderr, "               drift=double => drift rate for determining blockette 100 sample rates (applied to MK8 and femto only)\n");
    fprintf(stderr, "                   dir=path => output directory (must not exist, default is stdout)\n");
    fprintf(stderr, "rechan:old=new[,old=new...] => rename selected input channels\n");
    fprintf(stderr, "                         -v => verbose output\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "For pre-IDA10 data:\n");
    fprintf(stderr, "    rev=integer => input data rev\n");
    fprintf(stderr, "        db=spec => database specifier\n");
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i;
int rev = DEFAULT_REV, reclen=DEFAULT_RECLEN;
UINT32 in, out;
gzFile *gz;
DBIO *db = NULL;
IDA *ida;
int nread, unused, seqno;
UINT8 input[2 * IDA10_MAXRECLEN];
BOOL verbose = FALSE;
char *nname=NULL, *sname=NULL, qcode=DEFAULT_QCODE, *dir=NULL, *dbspec=NULL, *log=NULL, *map=NULL;
char *default_log = DEFAULT_LOG;
char *default_net = DEFAULT_NET;
REAL64 drift = MSEED_UNDEFINED_DRIFT;
LOGIO *lp;
int slinkdebug = 0;
LNKLST *chanmap = NULL;
MSEED_HANDLE *handle;
MSEED_RECORD record;
MSEED_THRESH threshold;

/* Parse and load command line arguments */

    nname  = default_net;
    reclen = DEFAULT_RECLEN;
    qcode  = DEFAULT_QCODE;
    log    = default_log;
    threshold.clock.locked  = MSEED_DEFAULT_CLOCK_LOCKED_PERCENT;
    threshold.clock.suspect = MSEED_DEFAULT_CLOCK_SUSPECT_PERCENT;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sname = argv[i] + strlen("sta=");
        } else if (strncasecmp(argv[i], "net=", strlen("net=")) == 0) {
            nname = argv[i] + strlen("net=");
        } else if (strncasecmp(argv[i], "dir=", strlen("dir=")) == 0) {
            dir = argv[i] + strlen("dir=");
        } else if (strncasecmp(argv[i], "q=", strlen("q=")) == 0) {
            qcode = toupper((argv[i] + strlen("q="))[0]);
        } else if (strncasecmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i] + strlen("rev="));
        } else if (strncasecmp(argv[i], "bs=", strlen("bs=")) == 0) {
            reclen = atoi(argv[i] + strlen("bs="));
        } else if (strncasecmp(argv[i], "locked=", strlen("locked=")) == 0) {
            threshold.clock.locked = atoi(argv[i] + strlen("locked="));
        } else if (strncasecmp(argv[i], "suspect=", strlen("suspect=")) == 0) {
            threshold.clock.suspect = atoi(argv[i] + strlen("suspect="));
        } else if (strncasecmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncasecmp(argv[i], "map=", strlen("map=")) == 0) {
            map = argv[i] + strlen("map=");
        } else if (strncasecmp(argv[i], "slinkdebug=", strlen("slinkdebug=")) == 0) {
            slinkdebug = atoi(argv[i] + strlen("slinkdebug="));
        } else if (strncasecmp(argv[i], "drift=", strlen("drift=")) == 0) {
            drift = (REAL64) atof(argv[i] + strlen("drift="));
        } else if (strncasecmp(argv[i], "rechan:", strlen("rechan:")) == 0) {
            if (chanmap != NULL) {
                fprintf(stderr, "%s: multiple 'rechan' definitions are not permitted\n", argv[0]);
                help(argv[0]);
            } else if ((chanmap = ida10BuildChannelMap(argv[i]+strlen("rechan:"))) == NULL) {
                fprintf(stderr, "%s: ", argv[0]);
                perror("ida10BuildChannelMap");
                exit(1);
            }
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
            if (slinkdebug == 0) slinkdebug = 1;
        } else if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "--h") == 0 || strcasecmp(argv[i], "--help") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized '%s' argument\n", argv[i]);
            help(argv[0]);
        }
    }

    if (util_powerof(reclen, 2) < 0) {
        fprintf(stderr, "illegal record length '%d' (must be a power of 2)\n", reclen);
        help(argv[0]);
    }

    if (sname == NULL) {
        fprintf(stderr, "ERROR: station name must be specified\n");
        help(argv[0]);
    }
    util_ucase(sname);

/* Start logging */

    lp = InitLogging(argv[0], log, verbose);
    if (verbose) LogCommandLine(argc, argv);

/* Initialize the output subsystem */

    seqno = InitWriter(dir);

/* Create the data converter handle */

    if ((handle = mseedCreateHandle(lp, reclen, qcode, seqno, WriteRecord, NULL)) == NULL) {
        perror("mseedCreateHandle");
        exit(1);
    }
    if (!mseedSetStaid(handle, sname)) {
        perror("mseedSetStaid");
        exit(1);
    }
    if (!mseedSetNetid(handle, nname)) {
        perror("mseedSetNetid");
        exit(1);
    }
    if (!mseedSetClockThreshold(handle, threshold.clock.locked, threshold.clock.suspect)) {
        perror("mseedSetClockThreshold");
        exit(1);
    }
    if (chanmap != NULL && !mseedSetIDA10Chanmap(handle, chanmap)) {
        perror("mseedSetIDA10Chanmap");
        exit(1);
    }
    if (drift != MSEED_UNDEFINED_DRIFT && !mseedSetDriftRate(handle, drift)) {
        perror("mseedSetIDA10Chanmap");
        exit(1);
    }
    if (!mseedSetDebug(handle, slinkdebug)) {
        perror("mseedSetDebug");
        exit(1);
    }

/* Open the input stream */

    if (rev != 10) {
        if ((db = dbioOpen(dbspec, NULL)) == NULL) {
            LogErr("dbioOpen: %s: %s\n", dbspec, strerror(errno));
            exit(1);
        }
        if ((ida = idaCreateHandle(sname, rev, map, db, NULL, 0)) == NULL) {
            LogErr("idaCreateHandle: sta=%s, rev=%d, map=%s: %s\n", sname, rev, map == NULL ? "NULL" : map, strerror(errno));
            exit(1);
        }
    }

    if ((gz = gzdopen(fileno(stdin), "rb")) == NULL) {
        LogErr("gzdopen: %s\n", strerror(errno));
        exit(1);
    }

/* Main loop.  Read IDA records, convert to MSRecord and process until done */

    /* IDA10 packets here */

    if (rev == 10) {
        do {
            nread = ida10ReadGz(gz, input, IDA10_MAXRECLEN, &unused, FALSE);
            if (nread > 0) {
                IncrementInputCount();
                if (ida10SubFormatCode(input) == IDA10_SUBFORMAT_12) {
                    ProcessIDA1012(handle, &input[64]);
                } else {
                    if (mseedConvertIDA10(handle, &record, input)) ProcessRecord(handle, &record);
                }
            } else if (nread != IDA10_EOF) {
                IncrementBadCount();
                LogErr("ida10ReadGz: %s\n", ida10ErrorString(nread));
                GracefulExit(MY_MOD_ID + 1);
            }
        } while (nread != IDA10_EOF);

    /* Legacy IDA packets here */

    } else {
        while ((nread = gzread(gz, input, IDA_BUFSIZ)) > 0) {
            IncrementInputCount();
            if (mseedConvertIDA9(handle, &record, input)) ProcessRecord(handle, &record);
        }
    }

/* Flush any partial records */

    mseedFlushHandle(handle);

/* All done */

    GracefulExit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.19  2017/11/01 21:39:10  dechavez
 * added a special branch for IDA10.12 (encapsulated Miniseed)
 *
 * Revision 1.18  2016/02/01 18:47:00  dechavez
 * give up immediately on any ida10ReadGz() errors
 *
 * Revision 1.17  2015/12/05 00:30:59  dechavez
 * declare input to be UINT8 instead of char, to calm OS X compiles
 *
 * Revision 1.16  2015/05/21 21:54:46  dechavez
 * count bad records
 *
 * Revision 1.15  2015/05/21 21:52:30  dechavez
 * Added tests for bad IDA10 reads and sane recovery from same
 *
 * Revision 1.14  2014/09/02 18:54:15  dechavez
 * fixed bug setting netID
 *
 * Revision 1.13  2014/08/26 21:24:27  dechavez
 * added slinkdebug option
 *
 * Revision 1.12  2014/08/11 20:45:06  dechavez
 * Rework to support libmseed 2.0.0
 *
 * Revision 1.11  2014/04/28 21:58:43  dechavez
 * removed dangling debug code
 *
 * Revision 1.10  2014/04/28 21:32:53  dechavez
 * changed MSClockQualityThreshold variable name from "clock" to "threshold"
 *
 * Revision 1.9  2014/04/25 17:21:12  dechavez
 * removed duplicate line in help message (so much trouble with that help message!)
 *
 * Revision 1.8  2014/04/18 19:21:28  dechavez
 * Included rechan option description on help message, suppressed the (unused)
 * options for specifying clock thresholds (locked=percent suspect=percent)
 *
 * Revision 1.7  2014/04/14 19:12:52  dechavez
 * added drift and rechan option support
 *
 * Revision 1.6  2014/03/06 18:51:48  dechavez
 * rework to accomodate the move of template.c over to libmseed
 *
 * Revision 1.5  2014/03/04 22:15:39  dechavez
 * fixed more help typos
 *
 * Revision 1.4  2014/03/04 22:11:41  dechavez
 * fixed help message cut and paste typo
 *
 * Revision 1.3  2014/03/04 22:04:55  dechavez
 * added support for Global clock lock/suspect parameters, expanded help message
 *
 * Revision 1.2  2014/03/04 21:10:43  dechavez
 * Major rework to make use of the new MSFlags field in the modified libmseed
 * which allows propagating input conditions to the MiniSEED FSDH flags and
 * blockette 1001
 *
 * Revision 1.1  2014/02/05 21:16:25  dechavez
 * initial release
 *
 */
