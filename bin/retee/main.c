#pragma ident "$Id: main.c,v 1.1 2015/11/16 22:14:09 dechavez Exp $"
/*======================================================================
 *
 *  Generate tee files using new blocking (nrec) factor
 *
 *====================================================================*/
#include "ida10.h"

static void help(char *myname)
{
    fprintf(stderr, "usage: %s sig=0xHexSignature nrec=RecsPerFile < IDA10\n", myname);
    exit(0);
}

int main(int argc, char **argv)
{
int i, nread, type;
UINT32 sig, nrec;
UINT8 buf[IDA10_MAXRECLEN];
IDA10_TS ts;
struct {
    char crnt[MAXPATHLEN+1];
    char this[MAXPATHLEN+1];
} path;
gzFile *gz;
FILE *fp = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "sig=", strlen("sig=")) == 0) {
            sig = (UINT32) strtoul(argv[i] + strlen("sig="), NULL, 16);
        } else if (strncmp(argv[i], "nrec=", strlen("nrec=")) == 0) {
            nrec = atol(argv[i]+ strlen("nrec="));
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized argument `%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    memset(path.this, 0, MAXPATHLEN+1);

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    while ((nread = ida10ReadGz(gz, buf, IDA10_MAXRECLEN, &type, FALSE)) > 0) {
        if (type != IDA10_TYPE_TS) continue; /* all live systems produce only TS these days */
        if (!ida10UnpackTS(buf, &ts)) {
            perror("ida10UnpackTS");
            exit(1);
        }
        if (!ts.hdr.cmn.extra.valid) continue; /* all live systems produce the "extra" data these days */
        sprintf(path.this, "%08X00000000%08X", sig, (ts.hdr.cmn.extra.seqno / nrec) * nrec);
        if (strcmp(path.this, path.crnt) != 0) {
            if (fp != NULL) fclose(fp);
            strcpy(path.crnt, path.this);
            if ((fp = fopen(path.crnt, "w")) == NULL) {
                fprintf(stderr, "fopen: ");
                perror(path.crnt);
                exit(1);
            }
            printf("%s\n", path.crnt);
        }
        if (fwrite(buf, sizeof(UINT8), nread, fp) != nread) {
            perror("fwrite");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2015/11/16 22:14:09  dechavez
 * initial release
 *
 */
