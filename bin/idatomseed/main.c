#pragma ident "$Id: main.c,v 1.2 2011/10/28 21:57:11 dechavez Exp $"
/*======================================================================
 *
 *  IDA to MiniSEED filter
 *
 *====================================================================*/
#include "ida.h"
#include "dbio.h"
#include "ida10.h"

static void help(char *myname)
{
    fprintf(stderr, "usage: %s sta=name [net=name q=D|R|Q|M rev=x db=spec]\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i, rev = 10;
UINT32 in, out, seqno;
char *net = "II";
char qcode = 'R';
char *sta = NULL;
char *dbspec = NULL;
gzFile *gz;
DBIO *db = NULL;
IDA *ida;
char *map = NULL;
int status, nread, unused;
char input[2 * IDA10_MAXRECLEN];
LISS_MSEED_HDR mseed;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta = argv[i] + strlen("sta=");
        } else if (strncasecmp(argv[i], "net=", strlen("net=")) == 0) {
            net = argv[i] + strlen("net=");
        } else if (strncasecmp(argv[i], "q=", strlen("q=")) == 0) {
            qcode = (argv[i] + strlen("q="))[0];
        } else if (strncasecmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i] + strlen("rev="));
        } else if (strncasecmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncasecmp(argv[i], "map=", strlen("map=")) == 0) {
            map = argv[i] + strlen("map=");
        } else {
            fprintf(stderr, "unrecognized '%s' argument\n", argv[i]);
            help(argv[0]);
        }
    }

    if (sta == NULL) {
        fprintf(stderr, "%s: missing station name\n", argv[0]);
        help(argv[0]);
    }

    if (rev != 10) {
        if ((db = dbioOpen(dbspec, NULL)) == NULL) {
            fprintf(stderr, "%s: dbioOpen: %s\n", argv[0], strerror(errno));
            exit(1);
        }
        if ((ida = idaCreateHandle(sta, rev, map, db, NULL, 0)) == NULL) {
            fprintf(stderr, "%s: idaCreateHandle: %s\n", argv[0], strerror(errno));
            exit(1);
        }
    }

    util_ucase(sta);
    util_ucase(net);
    switch (qcode) {
      case 'D':
      case 'R':
      case 'Q':
      case 'M':
        break;
      default:
        fprintf(stderr, "illegal data quality code '%c'\n", qcode);
        help(argv[0]);
    } 

    if ((gz = gzdopen(fileno(stdin), "rb")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    in = out = seqno = 0;

    if (rev == 10) {
        while ((nread = ida10ReadGz(gz, input, IDA10_MAXRECLEN, &unused, FALSE)) > 0) {
            ++in;
            if (ida10ToMseed(input, nread, ++seqno, qcode, sta, net, &mseed)) {
                memset(input, 0xee, IDA10_FIXEDHDRLEN);
                lissPackMseedHdr(input, &mseed);
                fwrite(input, 1, nread, stdout);
                ++out;
            }
        }
    } else {
        while ((nread = gzread(gz, input, IDA_BUFSIZ)) > 0) {
            ++in;
            if (idaToMseed(ida, input, ++seqno, qcode, net, &mseed)) {
                memset(input, 0xee, IDA_HEADLEN);
                lissPackMseedHdr(input, &mseed);
                fwrite(input, 1, nread, stdout);
                ++out;
            }
        }
    }
    fprintf(stderr, "%d/%d records read/converted\n", in, out);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2011/10/28 21:57:11  dechavez
 * use lissPackMseedHdr() to update headers
 *
 * Revision 1.1  2011/10/24 19:40:41  dechavez
 * initial release
 *
 */
