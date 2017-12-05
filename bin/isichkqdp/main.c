#pragma ident "$Id: main.c,v 1.1 2009/07/27 17:51:51 dechavez Exp $"
/*======================================================================
 *
 *  Check QDP packets in an ISI disk loop for gross errors
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "qdplus.h"
#include "zlib.h"

extern char *VersionIdentString;
static char *Myname;
BOOL Verbose = FALSE;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ bs=len ] < ISI/QDP disk loop file\n", myname);
    exit(1);
}

static void CheckCRC(UINT8 *buf, UINT32 index)
{
QDPLUS_PKT pkt;

    if (!qdpVerifyCRC(&pkt.qdp)) printf("\n%lu - *** CRC ERROR ***\n", index);
    qdplusUnpackWrappedQDP(buf, &pkt);
    if (Verbose) {
        printf("%lu -", index);
        printf(" %016llX %08lx%016llx", pkt.serialno, pkt.seqno.signature, pkt.seqno.counter);
        printf(" cmd=%s", qdpCmdString(pkt.qdp.hdr.cmd));
        printf(" len=%hu", pkt.qdp.hdr.dlen);
        printf(" seqno=%hu", pkt.qdp.hdr.seqno);
        printf(" ack=%hu", pkt.qdp.hdr.ack);
        printf("            \r");
    }
}

static CheckForNullPackets(UINT8 *buf, UINT32 index, int bs)
{
UINT8 i;
static UINT32 FirstNullPacketIndex = 0;
static BOOL NullPacketFound = FALSE;

    for (i = 0; i < bs && buf[i] == 0; i++);
    if (i == bs) {
        if (!NullPacketFound) {
            FirstNullPacketIndex = index;
            NullPacketFound = TRUE;
        }
    } else if (NullPacketFound) {
        printf("%lu - non-NULL packet follows NULL\n", index);
    }
}

int main(int argc, char **argv)
{
int i, errnum;
UINT32 index;
gzFile *gz = NULL;
#define DEFAULT_PKTLEN 596
int bs = DEFAULT_PKTLEN;
UINT8 *buf;

    if (argc < 1) help(Myname);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcmp(argv[i], "-v") == 0) {
            Verbose = TRUE;
        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            if ((bs = atoi(argv[i] + strlen("bs="))) < 1) {
                fprintf(stderr, "%s: illegal %s\n", argv[i]);
                exit(1);
            }
        }
    }

    if ((buf = (UINT8 *) malloc(bs)) == NULL) {
        perror("malloc");
        exit(1);
    }

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    index = 0;
    while (gzread(gz, buf, bs) == bs) {
        CheckCRC(buf, index);
        CheckForNullPackets(buf, index, bs);
        ++index;
    }
    printf("\n%lu packets read\n", index);
    gzerror(gz, &errnum);
    if (errnum != 0) {
        printf("gzerror %d\n", errnum);
        exit(1);
    }

    exit (0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2009/07/27 17:51:51  dechavez
 * checkpoint
 *
 */
