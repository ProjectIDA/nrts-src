#pragma ident "$Id: main.c,v 1.3 2015/12/08 20:32:26 dechavez Exp $"
/*======================================================================
 *
 *  Byteswap 2, 4, or 8 byte data from stdin
 *
 *====================================================================*/
#include "util.h"
#include "zlib.h"

#define UNDEFINED_WORDSIZE -1
#define DEFAULT_WORDSIZE    4

#define BUFFER_LEN 32768
static UINT8 buffer[BUFFER_LEN];

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ 2 4 8 ] < orig > swapped\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "If word size isn't specified then 4-byte data are assumed.\n");
    fprintf(stderr, "The program accepts uncompressed and gzip compressed input.\n");
    exit(0);
}

static void Swap2Bytes(gzFile *gz)
{
int i, nbyte, nsamp, wordlen=2;

    while ((nbyte = gzread(gz, buffer, BUFFER_LEN)) > 0) {
        nsamp = nbyte / wordlen;
        utilSwapUINT16((UINT16 *) buffer, nsamp);
        if (fwrite(buffer, sizeof(char), nbyte, stdout) != nbyte) {
            perror("fwrite");
            exit(1);
        }
    }

    exit(gzeof(gz) ? 0 : 1);
}

static void Swap4Bytes(gzFile *gz)
{
int i, nbyte, nsamp, wordlen=4;

    while ((nbyte = gzread(gz, buffer, BUFFER_LEN)) > 0) {
        nsamp = nbyte / wordlen;
        utilSwapUINT32((UINT32 *) buffer, nsamp);
        if (fwrite(buffer, sizeof(char), nbyte, stdout) != nbyte) {
            perror("fwrite");
            exit(1);
        }
    }

    exit(gzeof(gz) ? 0 : 1);
}

static void Swap8Bytes(gzFile *gz)
{
int i, nbyte, nsamp, wordlen=8;

    while ((nbyte = gzread(gz, buffer, BUFFER_LEN)) > 0) {
        nsamp = nbyte / wordlen;
        utilSwapUINT64((UINT64 *) buffer, nsamp);
        if (fwrite(buffer, sizeof(char), nbyte, stdout) != nbyte) {
            perror("fwrite");
            exit(1);
        }
    }

    exit(gzeof(gz) ? 0 : 1);
}

int main(int argc, char **argv)
{
gzFile *gz;
int i, wordsize = -1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else {
            if (wordsize != -1) {
                fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
                help(argv[0]);
            } else if ((wordsize = atoi(argv[i])) <= 0) {
                fprintf(stderr, "%s: illegal wordsize '%s'\n", argv[0], argv[i]);
                help(argv[0]);
            }
        }
    }

    if (wordsize == UNDEFINED_WORDSIZE) wordsize = DEFAULT_WORDSIZE;

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    switch (wordsize) {
      case 2: Swap2Bytes(gz); break;
      case 4: Swap4Bytes(gz); break;
      case 8: Swap8Bytes(gz); break;
      default:
        fprintf(stderr, "%s: illegal wordsize '%d'\n", argv[0], wordsize);
        help(argv[0]);
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2015/12/08 20:32:26  dechavez
 * include util.h to bring in prototypes
 *
 * Revision 1.2  2014/04/15 20:57:54  dechavez
 * added note about gzip support the help message
 *
 * Revision 1.1  2014/03/19 17:11:44  dechavez
 * initial release
 *
 */
