#pragma ident "$Id: main.c,v 1.3 2011/10/18 22:07:43 dechavez Exp $"
/*======================================================================
 *
 *  Construct IDA10 packets from ascii text streams
 *
 *====================================================================*/
#include "txtoida10.h"

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [format=rev -lcase] [dl=server:port] [log=name] < stdin [ > stdout ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "format=rev             - assume line format 'rev' (integer)\n");
    fprintf(stderr, "-lcase                 - set channel/loc name to lower case\n");
    fprintf(stderr, "log=name               - set log file name (or syslod:facility)\n");
    fprintf(stderr, "dl=server:port[:depth] - write to remote disk loop\n");
    fprintf(stderr, "tee=name               - copy input strings to named file\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "If the remote disk loop option is selected then output to stdout is supressed.\n");
    fprintf(stderr, "    server - name or dot decimal IP address of server\n");
    fprintf(stderr, "    port   - TCP/IP port remote disk loop server is listening at\n");
    fprintf(stderr, "    depth  - (optional) number of packets to buffer internally\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "If the time to generate a packet is close to the amount of time it will take to send\n");
    fprintf(stderr, "the packet to the remote disk loop then you will need to buffer packets internally to\n");
    fprintf(stderr, "to avoid data loss.\n");
    fprintf(stderr, "\n");

    exit(0);
}

static BOOL TeeInput(char *path, char *string)
{
FILE *fp;
static char *fid = "TeeInput";

    if ((fp = fopen(path, "a")) == NULL) {
        LogMsg(LOG_ERR, "%s: %s: %s\n", fid, path, strerror(errno));
        return FALSE;
    }

    fprintf(fp, "%s", string);
    if (string[strlen(string)-1] != '\n') fprintf(fp, "\n");
    fclose(fp);

    return TRUE;
}

int main(int argc, char **argv)
{
int i, lineno=0, status, rev = 1;
#define COMMENT '#'
#define MAXLINELEN 1024
char buf[MAXLINELEN+1];
char *log = NULL, *dl = NULL, *tee = NULL;
DECODE_FUNC DecodeFunc;
LNKLST *list;
LNKLST_NODE *crnt;
UINT32 options = 0;
LOGIO *lp;

    if ((list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "format=", strlen("format=")) == 0) {
            if ((rev = atoi(argv[i] + strlen("format="))) == 0) {
                fprintf(stderr, "%s: bad %s\n", argv[0], argv[i]);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "tee=", strlen("tee=")) == 0) {
            tee = argv[i] + strlen("tee=");
        } else if (strcasecmp(argv[i], "-lcase") == 0) {
            options |= TXTOIDA10_OPTION_LCASE;
        } else if (strncasecmp(argv[i], "dl=", strlen("dl=")) == 0) {
            dl = argv[i] + strlen("dl=");
        } else if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized argument `%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    switch (rev) {
      case 1: DecodeFunc = DecodeRev01; break;
      case 2: DecodeFunc = DecodeRev02; break;  // TPDin2 / IDA MPS
      default:
        fprintf(stderr, "unsupported line format '%d'\n", rev);
        exit(1);
    }

    lp = InitLogging(argv[0], log);
    if (dl != NULL && !SetDlOutput(dl, lp)) {
        fprintf(stderr, "ERROR with remote disk loop output option\n");
        exit(1);
    }

    while ((status = utilGetLine(stdin, buf, MAXLINELEN, COMMENT, &lineno)) == 0) {
        if (tee != NULL && !TeeInput(tee, buf)) {
            LogMsg(LOG_ERR, "input tee suspended\n");
            tee = NULL;
        }
        printf("line: %s\n", buf);
        (DecodeFunc)(list, buf, options);
    }

    if (status != 1) {
        LogMsg(LOG_INFO, "main: utilGetLine: %s", strerror(errno));
        exit(1);
    }

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        FlushPacket((PROTO_PACKET *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2011/10/18 22:07:43  dechavez
 * added tee option for saving a copy of input data
 *
 * Revision 1.2  2011/10/12 17:56:49  dechavez
 * added dl option for writing directly to a remote disk loop, log option
 * for logging via logio library
 *
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
