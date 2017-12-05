#pragma ident "$Id: main.c,v 1.3 2016/11/16 21:54:59 dechavez Exp $"
/*======================================================================
 *
 *  SBD to IDA10 converter
 *
 *====================================================================*/
#include "sbd.h"
#include "addoss.h"
#include "isi/dl.h"
#include "ida10.h"

static ISI_GLOB glob;
static LNKLST *imeilist;

typedef struct {
    char imei[SBD_IMEI_LEN+1];
    UINT32 zerotime;
} IMEI_INFO;

static UINT32 LookupZerotime(DBIO *db, char *imei, char *site, char *net)
{
static int count = 0;
UINT64 zerotime_ns;
LNKLST_NODE *crnt;
IMEI_INFO info, *ptr;
static LNKLST *list = NULL;
static char *fid = "LookupZerotime";

    if (list == NULL && (list = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

/* Search the list first to see if we already know the zerotime */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        ptr = (IMEI_INFO *) crnt->payload;
        if (strcmp(imei, ptr->imei) == 0) return ptr->zerotime;
        crnt = listNextNode(crnt);
    }

/* Didn't find it, so look it up in the database */

    if (!isidbLookupSiteByIMEI(db, imei, site, net, &zerotime_ns)) {
        fprintf(stderr, "*** ERROR *** %s: no site info for IMEI '%s'\n", fid, imei);
        exit(1);
    }

    strncpy(info.imei, imei, SBD_IMEI_LEN+1);
    info.zerotime = (UINT32) (zerotime_ns / NANOSEC_PER_SEC);
    if (!listAppend(list, &info, sizeof(IMEI_INFO))) {
        fprintf(stderr, "*** ERROR *** %s: listAppend:%s\n", fid, strerror(errno));
        exit(1);
    }

/* Print a message with the zero time being applied */

    fprintf(stderr, "Using %s for %s zerotime\n", utilDttostr(info.zerotime, 1000, NULL), imei);

    return info.zerotime;
}

static void ProcessSensorData(ADDOSS_SENSOR_DATA_INT24 *sensor, SBD_MO_HEADER *header, UINT32 zerotime)
{
int len;
UINT8 ida10[IDA10_MAXRECLEN];
static char *fid = "ProcessSensorData";

    if ((len = addossIDA10(ida10, sensor, header, zerotime)) < 0) {
        perror("addossIDA10");
        exit(1);
    }

    if (fwrite(ida10, 1, len, stdout) != len) {
        perror("fwrite");
        exit(1);
    }
}

static void ProcessSBD(SBD_MESSAGE *message)
{
char site[IDA_SNAMLEN+1];
char net[IDA_NNAMLEN+1];
UINT32 zerotime;
LNKLST *list;
LNKLST_NODE *crnt;
static char *fid = "ProcessSBD";

/* This check should already have been done by caller */

    if (!message->mo.header.valid || !message->mo.payload.valid) return;

/* Ignore WG mailbox checks (empty payloads) */

    if (message->mo.payload.len == 0) return;

/* Ignore invalid payloads */

    if (message->mo.payload.len % ADDOSS_FRAME_LEN) {
        fprintf(stderr, "*** WARNING *** %s: SBD payload is not a multiple of %d (ignored)\n", fid, ADDOSS_FRAME_LEN);
        return;
    }

/* Extract the frames into a time ordered list */

    if ((list = addossExtractSensorDataInt24(message)) == NULL) {
        fprintf(stderr, "*** ERROR *** %s: addossExtractSensorDataInt24: %s\n", fid, strerror(errno));
        exit(1);
    }

/* Lookup the zero time for this IMEI */

    zerotime = LookupZerotime(glob.db, message->mo.header.imei, site, net);

/* Convert each frame to IDA10 */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        ProcessSensorData((ADDOSS_SENSOR_DATA_INT24 *) crnt->payload, &message->mo.header, zerotime);
        crnt = listNextNode(crnt);
    }

    listDestroy(list);
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s < SBD_data | i10dmx\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i, count;
gzFile *gz;
SBD_MESSAGE message;
BOOL strip = FALSE;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-s") == 0) {
            strip = TRUE;
        } else {
            help(argv[0]);
        }
    }

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    if (!isidlSetGlobalParameters(NULL, argv[0], &glob)) {
        perror("isidlSetGlobalParameters");
        exit(1);
    }

    while (sbdReadMessage(gz, &message)) if (message.mo.header.valid && message.mo.payload.valid) ProcessSBD(&message);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2016/11/16 21:54:59  dechavez
 * lookup zerotime from imei "database" file
 *
 * Revision 1.2  2014/01/27 18:26:25  dechavez
 * use addossExtractSensorDataInt24() to sort all packets in a frame before decoding
 *
 * Revision 1.1  2013/05/14 20:54:43  dechavez
 * initial production release
 *
 */
