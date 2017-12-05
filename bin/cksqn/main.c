#pragma ident "$Id: main.c,v 1.3 2015/12/08 21:03:00 dechavez Exp $"
/*======================================================================
 *
 *  Check IDA10 packet headers for sequence number sanity
 *
 *====================================================================*/
#include "ida10.h"

#define TERSE   0x01
#define VERBOSE 0x02
#define DECIMAL 0x03

static void help(char *myname, int status)
{
    fprintf(stderr, "usage: %s { [-v | -d] -q } < stdin > stdout\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "-v - verbose output\n");
    fprintf(stderr, "-q - set exit status only (nothing sent to stdout)\n");
    fprintf(stderr, "-d - print sequence numbers using base 10\n");
    fprintf(stderr, "\n");

    exit(status);
}

static int SeqnoIncrementError(int options, int count, UINT32 crnt, UINT32 prev, UINT8 *raw)
{
int retval;
IDA10_TS ts;
UINT32 expected;
static char msgbuf[MAXPATHLEN+1];

    expected = prev + 1;
    retval = (crnt == expected) ? 0 : 1;
    if (retval == 0 || options & TERSE) return retval;

    if (options & DECIMAL) {
        sprintf(msgbuf, "record %d: expected %d, got %d", count, expected, crnt);
    } else {
        sprintf(msgbuf, "record %d: expected 0x%08x, got 0x%08x", count, expected, crnt);
    }
    if ((options & VERBOSE) && (raw != NULL)) {
        if (!ida10UnpackTS(raw, &ts)) {
            printf(" *** ERROR UNPACKING TS RECORD # %d\n", count);
            exit(-1);
        }
        sprintf(msgbuf+strlen(msgbuf), " TS: %s", ida10TStoString(&ts, (char *) raw));
    }

    printf("%s\n", msgbuf);

    return retval;
    
}

int main(int argc, char **argv)
{
int i, status, nread, type;
UINT32 count = 0, errors = 0;
UINT8 buf[IDA10_MAXRECLEN];
gzFile *gz;
IDA10_CMNHDR cmnhdr;
UINT32 crnt, prev;
BOOL first = TRUE, options = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            help(argv[i], 0);
        } else if (strcmp(argv[i], "-v") == 0) {
            options |= VERBOSE;
        } else if (strcmp(argv[i], "-q") == 0) {
            options |= TERSE;
        } else if (strcmp(argv[i], "-d") == 0) {
            options |= DECIMAL;
        } else {
            fprintf(stderr, "unrecognized argument `%s'\n", argv[i]);
            help(argv[0], -1);
        }
    }

    if ((options & VERBOSE) && (options & TERSE)) {
        fprintf(stderr, "%s: ERROR: -v and -q options are mutually exclusive\n", argv[0]);
        exit(-1);
    }

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(-1);
    }

    count = 0;
    while ((nread = ida10ReadGz(gz, buf, IDA10_MAXRECLEN, &type, FALSE)) > 0) {
        ++count;
        if (ida10UnpackCmnHdr(buf, &cmnhdr)) {
            if (cmnhdr.extra.valid) {
                crnt = cmnhdr.extra.seqno;
                if (first) {
                    first = FALSE;
                } else {
                    errors += SeqnoIncrementError(options, count, crnt, prev, type == IDA10_TYPE_TS ? buf : NULL);
                }
                prev = crnt;
            }
        } else {
            printf("record %u is not an IDA10 packet\n", count);
            ++errors;
        }
    }

    status = nread;
    if (status != IDA10_EOF) printf("ida10ReadGz failed at record no. %d: %s\n", count, ida10ErrorString(status));

    if (options & VERBOSE) printf("%u records checked, %d errors found\n", count, errors);

    exit(errors ? 1 : 0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2015/12/08 21:03:00  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.2  2014/05/08 17:51:13  dechavez
 * Include non-IDA10 records in the count of records with problems
 *
 * Revision 1.1  2014/05/08 17:29:07  dechavez
 * initial release
 *
 */
