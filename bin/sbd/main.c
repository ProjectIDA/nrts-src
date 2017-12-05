#pragma ident "$Id: main.c,v 1.3 2013/05/14 20:59:08 dechavez Exp $"
/*======================================================================
 *
 *  Read and parse Iridium SBD messages, assume ADDOSS payloads
 *
 *====================================================================*/
#include "sbd.h"
#include "addoss.h"

static struct {
    ADDOSS_SENSOR_DATA_INT24 sensor;
    BOOL first;
} prev[0xff];

static void PrintOSG_STATUS(UINT8 *start)
{
ADDOSS_OSG_STATUS status;

    addossUnpackOSGstatus(start, &status);
    addossPrintOSG_STATUS(stdout, &status);
}

static void PrintOBP_STATUS(UINT8 *start)
{
ADDOSS_OBP_STATUS status;

    addossUnpackOBPstatus(start, &status);
    addossPrintOBP_STATUS(stdout, &status);
}

static void PrintREQ_SENSOR_DATA(UINT8 *start)
{
ADDOSS_OBP_CHANFO chan;

    addossUnpackReqSensorData(start, &chan);
    addossPrintREQ_SENSOR_DATA(stdout, &chan);
}

static void PrintSENSOR_DATA_INT24(UINT8 *start)
{
ADDOSS_SENSOR_DATA_INT24 sensor;

    addossUnpackSensorDataInt24(start, &sensor);
    if (prev[sensor.id].first) {
        addossPrintSENSOR_DATA_INT24(stdout, &sensor, NULL);
        prev[sensor.id].first = FALSE;
    } else {
        addossPrintSENSOR_DATA_INT24(stdout, &sensor, &prev[sensor.id].sensor);
    }

    prev[sensor.id].sensor = sensor;
}

static void PrintADDOSSframe(SBD_PAYLOAD *payload)
{
int i;
static UINT32 count = 0;
UINT8 *ptr;
UINT16 type;
static BOOL first = TRUE;

    if (!payload->valid || (payload->len % ADDOSS_FRAME_LEN)) return;

    if (!first) {
        for (i = 0; i < 110; i++) printf("=");
        printf("\n\n");
    }
    first = FALSE;

    ptr = payload->data;
    while (ptr < (payload->data + payload->len)) {

        utilUnkcapUINT16(ptr, &type);
        printf("Frame # %d: type = %d (%s)\n", ++count, type, addossTypeString(type));
        utilPrintHexDump(stdout, ptr, ADDOSS_FRAME_LEN);
        printf("\n");

        ptr += 2;

        switch (type) {
          case ADDOSS_ID_SENSOR_DATA_INT24:
            PrintSENSOR_DATA_INT24(ptr);
            break;
          case ADDOSS_ID_OSG_STATUS:
            PrintOSG_STATUS(ptr);
            break;
          case ADDOSS_ID_OBP_STATUS:
            PrintOBP_STATUS(ptr);
            break;
          case ADDOSS_ID_REQ_SENSOR_DATA:
            PrintREQ_SENSOR_DATA(ptr);
            break;
          case ADDOSS_ID_REQ_OSG_STATUS:
          case ADDOSS_ID_REQ_OBP_STATUS:
          case ADDOSS_ID_SET_OBP_POWER:
          case ADDOSS_ID_SET_SENSOR_POWER:
            printf("Frame is recognized but no decoder has been written\n");
            break;
          default:
            printf("Not an ADDOSS frame? Type 0x%04x is unrecognized.\n", type);
        }
        ptr += (ADDOSS_FRAME_LEN - 2);
        printf("\n");
    }
}

static void TeePayload(FILE *fp, SBD_PAYLOAD *payload)
{
    if (fp == NULL || payload == NULL) return;
    if (!payload->valid || (payload->len % ADDOSS_FRAME_LEN)) return;

    if (fwrite(payload->data, 1, payload->len, fp) != payload->len) {
        perror("TeePayload: fwrite");
        exit(1);
    }
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s < SBD_data > TextReport\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i, count;
char *tfile = NULL;
gzFile *gz;
SBD_MESSAGE message;
BOOL strip = FALSE;

    if (argc != 1) help(argv[0]);

    if ((gz = gzdopen(fileno(stdin), "r")) == NULL) {
        perror("gzdopen");
        exit(1);
    }

    for (i = 0; i < 0xff; i++) prev[i].first = FALSE;

    count = 0;
    while (sbdReadMessage(gz, &message)) {
        printf("%d ********************************************************************************************************************\n", ++count);
        sbdPrintMessageIE(stdout, &message);
        if (message.mo.payload.valid) PrintADDOSSframe(&message.mo.payload);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2013/05/14 20:59:08  dechavez
 * removed tee and strip options
 *
 * Revision 1.2  2013/03/13 21:31:04  dechavez
 * replaced PrintSBD with new library function sbdPrintMessageIE()
 *
 * Revision 1.1  2013/03/11 23:18:47  dechavez
 * initial release
 *
 */
