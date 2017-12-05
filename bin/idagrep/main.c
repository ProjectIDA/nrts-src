#pragma ident "$Id"
/*======================================================================
 *
 *  Simple minded filter to extract data packets based on channel
 *  and filter codes. 
 *
 *====================================================================*/
#include "util.h"
#include "ida.h"
#include "ida10.h"

extern char *VersionIdentString;

#define UNDEFINED_REV -1
#define DEFAULT_IFP stdin
#define DEFAULT_OFP stdout
#define MAX_CF 128

typedef struct {
    UINT16 chan;
    UINT16 filt;
    char cname[IDA10_CNAMLEN + 1];
} CF;
static int subformat[IDA10_MAX_SUBFORMAT];
static CF cf[MAX_CF];
static int ncf = -1;

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [if=input_file of=output_file rev=code -v] { cf=C,F[:C,F...] | keep=chn,[chn,...] | reject=chn[,chn...] | boxid=boxid }\n", myname);
    fprintf(stderr, "defaults: if=stdin of=stdout (revision autosensed)\n");
    fprintf(stderr, "boxid: 4 character numeric boxid of digitizer of data packets to keep/reject. For Q330, is hexadecimal. \n");
    fprintf(stderr, "       Only availablef or IDA10 format input. \n");
    fprintf(stderr,"\n");
    exit(1);
}

static BOOL ParseCF(char *input, UINT16 *chan, UINT16 *filt)
{
char *token[2];

    if (utilParse(input, token, ",", 2, 0) != 2) return FALSE;
    *chan = (UINT16) atoi(token[0]);
    *filt = (UINT16) atoi(token[1]);
    return TRUE;
}

static BOOL ParseSubformatString(char *input)
{
int i, ntoken, sf;
char *copy, *arg, *token[IDA10_MAX_SUBFORMAT];

    if (input == NULL) {
        for (i = 0; i < IDA10_MAX_SUBFORMAT; i++) subformat[i] = 1;
        return TRUE;
    }

    if ((copy = strdup(input)) == NULL) {
        perror("strdup");
        exit(1);
    }

    ntoken = utilParse(copy, token, ",", IDA10_MAX_SUBFORMAT, 0);
    for (i = 0; i < ntoken; i++) {
        if (strncmp(token[i], "10.", strlen("10.")) != 0) return FALSE;
        arg = token[i] + strlen("10.");
        if (!isdigit(arg[0])) return FALSE;
        sf = atoi(arg);
        if (sf < 0 || sf > IDA10_MAX_SUBFORMAT) {
            fprintf(stderr, "illegal format '%s'\n", token[i]);
            return FALSE;
        }
        subformat[sf] = 1;
    }

    free(copy);

    return TRUE;
}

static BOOL ParseKeepString(int rev, char *input)
{
int i;
char *token[MAX_CF];
char *copy;

    if (input == NULL) {
        ncf = 0;
        return TRUE;
    }

    if ((copy = strdup(input)) == NULL) {
        perror("strdup");
        exit(1);
    }

    if (rev >= 10) {
        ncf = utilParse(copy, token, ",", MAX_CF, 0);
        for (i = 0; i < ncf; i++) {
            memcpy(cf[i].cname, token[i], IDA10_CNAMLEN);
            cf[i].cname[IDA10_CNAMLEN] = 0;
        }
    } else {
        ncf = utilParse(copy, token, ":", MAX_CF, 0);
        for (i = 0; i < ncf; i++) {
            if (!ParseCF(token[i], &cf[i].chan, &cf[i].filt)) {
                free(copy);
                return FALSE;
            }
        }
    }

    free(copy);
    return TRUE;
}

static BOOL ExtractCF(char *buf, int rev, UINT16 *chan, UINT16 *filt)
{
int i;
static struct {
    int rev;
    int coff;
    int foff;
    int nbytes;
} *map, OffsetMap[] = {
    { 2, 38, 40,  1},
    { 3,  3,  4,  1},
    { 4,  3,  4,  1},
    { 5, 36, 38,  2},
    { 6, 36, 38,  2},
    { 7, 36, 38,  2},
    { 8, 36, 38,  2},
    { 9, 36, 38,  2},
    {-1, -1, -1, -1}
};

    if (rev >= 10) return TRUE;

    for (map = NULL, i = 0; map == NULL && OffsetMap[i].rev > 0; i++) if (OffsetMap[i].rev == rev) map = &OffsetMap[i];
    if (map == NULL) return FALSE;

    switch (map->nbytes) {
      case 1:
        *chan = (UINT16) buf[map->coff];
        *filt = (UINT16) buf[map->foff];
        return TRUE;
      case 2:
        utilUnpackUINT16((UINT8 *) &buf[map->coff], chan);
        utilUnpackUINT16((UINT8 *) &buf[map->foff], filt);
        return TRUE;
    }

    return FALSE;
}

static BOOL WantRecord(char *buf, int rev, CF *cf, char *boxid, BOOL MatchAction)
{
int i;
UINT16 chan, filt;
static IDA10_TSHDR hdr;
BOOL ChanMatch;
BOOL BoxIDMatch;
UINT16 boxid_bits; // low order 16 bits of boxid or serialno
char boxid_str[5]; // low order 16 bits converted to uppercase string for coparison

    if (rev == 10) {

        // check error conditions and return opposite of requested.
        if (ida10Type((UINT8 *) buf) != IDA10_TYPE_TS) return !MatchAction;
        if (!ida10UnpackTSHdr((UINT8 *) buf, &hdr)) return !MatchAction;
        if (subformat[hdr.cmn.subformat] == 0) return !MatchAction;

        // check for boxid filter match, if specified...

        if (boxid[0] != '\0') {
           // convert filter to upper
           int ndx;
           for (ndx = 0; ndx < strlen(boxid); ndx++) 
              boxid[ndx] = toupper(boxid[ndx]);

           // get boxid from data buffer, checking for 16 vs 64 bit values

           if (hdr.cmn.boxid != IDA10_64BIT_BOXID) {
               // convert 16-bit boxid to 4 digit string
               boxid_bits = (UINT16)hdr.cmn.boxid;
               sprintf(boxid_str, "%d", boxid_bits);
           } else {
               // using 64-bit bit boxid. 
               // convert low-order 16 bits to 4 digit uppercase hex string
               boxid_bits = hdr.cmn.serialno & 0xffff;
               sprintf(boxid_str, "%X", boxid_bits);
           }

           // compare with boxid filter value
           BoxIDMatch = (strcmp(boxid_str, boxid) == 0);
        } else {
           // match all boxid's if filter not specified
           BoxIDMatch = TRUE;
        }

        // check for channel filter match
        if (ncf == 0) 
           ChanMatch = TRUE; // no chan filter specified, keep or reject all
        else {
           ChanMatch = FALSE;
           // Assume not match, check if match with any chan filter...
           for (i = 0; i < ncf; i++)
               ChanMatch |= (strcmp(hdr.cname, cf[i].cname) == 0);
        }

        if (ChanMatch && BoxIDMatch)
           return MatchAction;
        else
           return !MatchAction;

    } else {
       if (ida_rtype((UINT8 *) buf, rev) != IDA_DATA) return !MatchAction;
       if (!ExtractCF(buf, rev, &chan, &filt)) return !MatchAction;
       for (i = 0; i < ncf; i++) {
           if (chan == cf[i].chan && filt == cf[i].filt) return MatchAction;
       }
       return !MatchAction;
    }

}

int main(int argc, char **argv)
{
int i, RecordType;
int rev = UNDEFINED_REV;
FILE *ifp = DEFAULT_IFP;
FILE *ofp = DEFAULT_OFP;
UINT16 test1, test2;
char buffer[IDA10_MAXRECLEN];
BOOL MatchAction = TRUE;
char *KeepString = NULL;
char *SubformatString = NULL;
char *boxid = "";

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i]+strlen("rev="));
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
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
            KeepString = argv[i] + strlen("cf=");
        } else if (strncmp(argv[i], "format=", strlen("format=")) == 0) {
            SubformatString = argv[i] + strlen("format=");
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            if (KeepString != NULL) help(argv[0]);
            KeepString = argv[i] + strlen("keep=");
        } else if (strncmp(argv[i], "reject=", strlen("reject=")) == 0) {
            if (KeepString != NULL) help(argv[0]);
            KeepString = argv[i] + strlen("reject=");
            MatchAction = FALSE;
        } else if (strncmp(argv[i], "boxid=", strlen("boxid=")) == 0) {
            boxid = argv[i] + strlen("boxid=");
        } else if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-v") == 0) {
            MatchAction = FALSE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (KeepString == NULL && SubformatString == NULL) help(argv[0]);

    if (fread(buffer, 1, IDA_BUFLEN, ifp) != IDA_BUFLEN) {
        if (feof(ifp)) {
            fprintf(stderr, "%s: empty input file\n", argv[0]);
        } else {
            fprintf(stderr, "%s: fread: %s\n", argv[0], strerror(errno));
        }
        exit(1);
    }

    if (rev == UNDEFINED_REV) {
        if ((rev = idaPacketFormat((UINT8 *) buffer)) == 0) { 
            fprintf(stderr, "%s: unable to infer data format from input\n", argv[0]);
        } else {
            fprintf(stderr, "assuming rev=%d\n", rev);
        }
    }

    if (!ParseKeepString(rev, KeepString)) {
        fprintf(stderr, "%s: illegal %s\n", argv[0], KeepString);
        exit(1);
    } else if (ncf < 0) {
        help(argv[0]);
    }

    if (!ParseSubformatString(SubformatString)) {
        fprintf(stderr, "%s: illegal format %s\n", argv[0], SubformatString);
        exit(1);
    }

    if (!ExtractCF(buffer, rev, &test1, &test2)) {
        fprintf(stderr, "%s: No map information for rev %d\n", argv[0], rev);
        exit(1);
    }

    do {
        if (WantRecord(buffer, rev, cf, boxid, MatchAction)) {
            if (fwrite(buffer, 1, IDA_BUFLEN, ofp) != IDA_BUFLEN) {
                fprintf(stderr, "%s: ", argv[0]);
                perror("fwrite");
                exit(1);
            }
        }
    } while (fread(buffer, 1, IDA_BUFLEN, ifp) == IDA_BUFLEN);

    if (!feof(ifp)) fprintf(stderr, "%s: fread: %s\n", argv[0], strerror(errno));

    if (ifp != stdin) fclose(ifp);
    if (ofp != stdout) fclose(ofp);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2017/05/24 21:22:45  dauerbach
 * added box id support
 *
 * Revision 1.2  2015/12/08 21:07:11  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.1  2009/02/04 17:57:25  dechavez
 * added format selection (IDA10) and code moved to its own src/bin dir
 *
 */
