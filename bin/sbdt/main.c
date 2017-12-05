#pragma ident "$Id: main.c,v 1.1 2015/06/30 19:19:06 dechavez Exp $"
/*======================================================================
 *
 *  Feed canned SBD messages to a server
 *
 *====================================================================*/
#include "sbd.h"
#include "ida.h"
#include "addoss.h"

static DBIO *db;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ server[:port] -i if=pathname]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "-i => interactive mode (requires if=)\n");
    fprintf(stderr, "default server=%s:%d\n", SBD_DEFAULT_SERVER, SBD_DEFAULT_PORT);
    exit(1);
}

static void PrintSensorData(ADDOSS_SENSOR_DATA_INT24 *sensor, SBD_MO_HEADER *header, UINT64 zerotime)
{
int len;
IDA10_TS ts;
UINT8 ida10[IDA10_MAXRECLEN];

    if ((len = addossIDA10(ida10, sensor, header, (INT32) (zerotime / NANOSEC_PER_SEC))) < 0) {
        printf("*** WARNING *** addossIDA10 error %d, frame dropped\n", len);
        return;
    }
    ida10UnpackTS(ida10, &ts);
    printf("TS %s\n", ida10TStoString(&ts, NULL));
}

static void PrintADDOSS(SBD_MESSAGE *message)
{
UINT64 zerotime;
char site[IDA_SNAMLEN+1];
char net[IDA_NNAMLEN+1];
LNKLST *list;
LNKLST_NODE *crnt;
static char *fid = "PrintADDOSS";

    if (!isidbLookupSiteByIMEI(db, message->mo.header.imei, site, net, &zerotime)) {
        printf("*** WARNING *** %s: no site info for IMEI '%s'", fid, message->mo.header.imei);
        return;
    }
    if (message->mo.payload.len == 0) { 
        printf("empty message received (mailbox check)\n");
        return; 
    }
    if (message->mo.payload.len % ADDOSS_FRAME_LEN) {
        printf("*** WARNING *** %s: SBD payload is not a multiple of %d (ignored)", fid, ADDOSS_FRAME_LEN);
        return; 
    }
    printf("%d %d-byte ADDOSS frames received\n", message->mo.payload.len / ADDOSS_FRAME_LEN, ADDOSS_FRAME_LEN);
    if ((list = addossExtractSensorDataInt24(message)) == NULL) { 
        fprintf(stderr, "*** ERROR *** %s: addossExtractSensorDataInt24: %s\n", fid, strerror(errno));
        exit(1);
    }
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        PrintSensorData((ADDOSS_SENSOR_DATA_INT24 *) crnt->payload, &message->mo.header, zerotime);
        crnt = listNextNode(crnt);
    }

    listDestroy(list);
}

static void PrintMessage(SBD_MESSAGE *message)
{
char tstamp[MAXPATHLEN+1];

    if (message->mo.header.valid) {
        utilLttostr(message->mo.header.tstamp, 0, tstamp);
        printf("MO_HEADER: imei=%s cdr=0x%08x momsn=0x%04x status=%02d tstamp=%s\n",
            message->mo.header.imei,
            message->mo.header.cdr,
            message->mo.header.momsn,
            message->mo.header.status,
            tstamp  
        );      
    }

    if (message->mo.payload.valid) {
        printf("MO_PAYLOAD: %d bytes\n", message->mo.payload.len);
        utilPrintHexDump(stdout, message->mo.payload.data, message->mo.payload.len);
    }

    if (message->mo.location.valid) {
        printf("MO_LOCATION: lat=%.4f lon=%.4f cep=%d km\n",
            message->mo.location.lat,
            message->mo.location.lon,
            message->mo.location.cep
        );      
    }

    if (message->mo.confirm.valid) {
        printf("MO_CONFIRM: %s\n", message->mo.confirm.status ? "Success" : "FAILURE");
    }

    if (message->mt.header.valid) {
        printf("MT_HEADER: imei=%s myid=0x%08x flags=0x%04x\n",
            message->mt.header.imei,
            message->mt.header.myid,
            message->mt.header.flags
        );      
    }

    if (message->mt.payload.valid) {
        printf("MT_PAYLOAD: %d bytes\n", message->mt.payload.len);
        utilPrintHexDump(stdout, message->mt.payload.data, message->mt.payload.len);
    }

    if (message->mt.confirm.valid) {
        printf("MT_CONFIRM: imei=%s cdr=0x%08x myid=0x%08x status=%d\n",
            message->mt.confirm.imei,
            message->mt.confirm.cdr,
            message->mt.confirm.myid,
            message->mt.confirm.status
        );      
    }

    if (message->mt.priority.valid) {
        printf("MO_PRIORITY: %d\n", message->mt.priority.level);
    }
    
    if (message->mo.header.valid && message->mo.payload.valid) PrintADDOSS(message);
}

int main(int argc, char **argv)
{
SBD *sbd;
gzFile *gz;
int i, count;
FILE *fp = NULL;
SBD_MESSAGE message;
BOOL interactive = FALSE;
int port = SBD_DEFAULT_PORT;
char server[MAXPATHLEN+1], *path = NULL;
static char *dbid = "/ida/nrts";
static char *default_server = SBD_DEFAULT_SERVER;

    memset(server, 0, MAXPATHLEN+1);

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-i") == 0) {
            interactive = TRUE;
        } else if (strncasecmp(argv[i], "if=", strlen("if=")) == 0) {
            path = argv[i] + strlen("if=");
        } else if (strncasecmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else if (server[0] == 0) {
            utilParseServer(argv[i], server, &port);
        } else {
            fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }
    if (server[0] == 0) strcpy(server, default_server);

    if (path == NULL) {
        if (interactive) {
            fprintf(stderr, "ERROR: -i option requires input file specified via if=pathname\n\n");
            help(argv[0]);
        } else {
            fp = stdin;
        }
    } else if ((fp = fopen(path, "rb")) == NULL) {
        perror(path);
        exit(1);
    }

    if ((gz = gzdopen(fileno(fp), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    if ((db = dbioOpen(dbid, NULL)) == NULL) {
        fprintf(stderr, "dbioOpen: %s: %s\n", dbid, strerror(errno));
        exit(1);
    }

    count = 0;
    while (sbdReadMessage(gz, &message)) {
        if (interactive) {
            utilPause(TRUE);
            PrintMessage(&message);
        }
        if ((sbd = sbdOpen(server, port, NULL, NULL, 0)) == NULL) {
            perror("sbdOpen");
            exit(1);
        }
        if (!sbdSendMessage(sbd, &message)) {
            perror("sbdSendMessage");
            exit(1);
        }
        sbdClose(sbd);
        ++count;
    }

    printf("%d messages sent\n", count);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2015/06/30 19:19:06  dechavez
 * initial release
 *
 */
