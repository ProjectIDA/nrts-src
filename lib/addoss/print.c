#pragma ident "$Id: print.c,v 1.5 2015/11/04 22:01:02 dechavez Exp $"
/*======================================================================
 * 
 * Print decoded ADDOSS frames
 *
 *====================================================================*/
#include "addoss.h"
#include "util.h"

void addossPrintOSG_STATUS(FILE *fp, ADDOSS_OSG_STATUS *status)
{
static char *fid = "addossPrintOSG_STATUS";

    if (fp == NULL) fp = stdout;
    if (status == NULL) {
        fprintf(fp, "%s: NULL status!\n", fid);
        return;
    }

    fprintf(fp, "OSG status frame\n");
    fprintf(fp, "----------------\n");
    fprintf(fp, "            reason = ");
    switch (status->reason) {
      case 1:  fprintf(fp, "heartbeat\n"); break;
      case 2:  fprintf(fp, "lost OBS comm\n"); break;
      case 3:  fprintf(fp, "OBS ping received\n"); break;
      default: fprintf(fp, "unknown reason '%d'\n", status->reason);
    }
    fprintf(fp, "            tstamp = %s\n", utilLttostr(status->tstamp.epoch, 0, NULL));
    fprintf(fp, "          nnCAMERR = %d\n", status->nCAMERR);
    fprintf(fp, "       nSendPacket = %d\n", status->nSendPacket);
    fprintf(fp, "nSendPacketRetries = %d\n", status->nSendPacketRetries);
    fprintf(fp, "      nPacketsSent = %d\n", status->nPacketsSent);
    fprintf(fp, "  nPacketsReceived = %d\n", status->nPacketsReceived);
    fprintf(fp, "Last packet received status\n");
    fprintf(fp, "---------------------------\n");
    fprintf(fp, "              mode = %d\n", status->lastpacket.mode);
    fprintf(fp, "          PSKerror = %d\n", status->lastpacket.PSKerror);
    fprintf(fp, "   nFramesExpected = %d\n", status->lastpacket.nFramesExpected);
    fprintf(fp, "        nCRCerrors = %d\n", status->lastpacket.nCRCerrors);
    fprintf(fp, "  MeanSquaredError = %c%u\n", status->lastpacket.msesign  ? '-' : '+', status->lastpacket.MeanSquaredError);
    fprintf(fp, "RecvSignalStrength = %c%u\n", status->lastpacket.rsssign  ? '-' : '+', status->lastpacket.RecvSignalStrength);
    fprintf(fp, "          InputSNR = %c%u\n", status->lastpacket.isnrsign ? '-' : '+', status->lastpacket.InputSNR);
    fprintf(fp, "         OutputSNR = %c%u\n", status->lastpacket.osnrsign ? '-' : '+', status->lastpacket.OutputSNR);
    fprintf(fp, "       StdDevNoise = %c%u\n", status->lastpacket.sdnsign  ? '-' : '+', status->lastpacket.StdDevNoise);
    fprintf(fp, "            tstamp = %s\n", utilLttostr(status->lastpacket.tstamp.epoch, 0, NULL));
}

static void PrintChanfo(FILE *fp, int i, ADDOSS_OBP_CHANFO *chan)
{
    fprintf(fp, "%d 0x%08x 0x%08x 0x%08x %u\n", i, chan->first_lba, chan->last_lba, chan->crnt_lba, chan->srate);
}

void addossPrintOBP_STATUS(FILE *fp, ADDOSS_OBP_STATUS *status)
{
int i;
static char *fid = "addossPrintOBP_STATUS";

    if (fp == NULL) fp = stdout;
    if (status == NULL) {
        fprintf(fp, "%s: NULL status!\n", fid);
        return;
    }

    fprintf(fp, "OBP status frame\n");
    fprintf(fp, "----------------\n");
    for (i = 0; i < ADDOSS_MAXCHAN; i++) PrintChanfo(fp, i, &status->chan[i]);
}

void addossPrintREQ_SENSOR_DATA(FILE *fp, ADDOSS_OBP_CHANFO *chan)
{
static char *fid = "addossPrintOBP_STATUS";

    if (fp == NULL) fp = stdout;
    if (chan == NULL) {
        fprintf(fp, "%s: NULL chan!\n", fid);
        return;
    }

    fprintf(fp, "Request archived sensor data frame\n");
    fprintf(fp, "----------------------------------\n");
    fprintf(fp, "start sector = 0x%08x\n", chan->first_lba);
    fprintf(fp, "  end sector = 0x%08x\n", chan->last_lba);
}

void addossPrintSENSOR_DATA_INT24(FILE *fp, ADDOSS_SENSOR_DATA_INT24 *sensor, ADDOSS_SENSOR_DATA_INT24 *prev)
{
int i;
INT64 diff, error;
INT64 nominal, srate;
static REAL64 factor;
static char *fid = "addossPrintSENSOR_DATA_INT24";

    if (fp == NULL) fp = stdout;
    if (sensor == NULL) {
        fprintf(fp, "%s: NULL sensor!\n", fid);
        return;
    }

    fprintf(fp, "Sensor Data\n");
    fprintf(fp, "-----------\n");
    fprintf(fp, "channel id=%d, tstamp=0x%016llx", sensor->id, sensor->tstamp);
    if (prev != NULL) {
        srate = sensor->id < 4 ? 1LL : 50LL; /* should really get from status packet */
        nominal = (INT64) ADDOSS_TICS_PER_SEC * (INT64) ADDOSS_SAMPLES_PER_FRAME * srate;
        diff = sensor->tstamp - prev->tstamp;
        error = (diff > nominal) ? diff - nominal : nominal-diff;
        factor = (REAL64) error / (REAL64) nominal;
        fprintf(fp, " prev=0x%016llx,", prev->tstamp);
        fprintf(fp, " diff=0x%016llx,", diff);
        fprintf(fp, " error=0x%016llx,", error);
        fprintf(fp, " factor=%.4lf", factor);
        if (sensor->tstamp <= prev->tstamp) fprintf(fp, " *** TIME JUMPED BACKWARDS ***");
    }
    fprintf(fp, "\n");
    for (i = 0; i < sensor->nsamp; i++) {
        fprintf(fp, " %10d", sensor->data[i]);
        if (i > 0 && (i+1) % 10 == 0) fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.5  2015/11/04 22:01:02  dechavez
 * modify print format strings to calm OS X gcc
 *
 * Revision 1.4  2013/07/19 17:48:33  dechavez
 * minor tweak of addossPrintOSG_STATUS() formatting
 *
 * Revision 1.3  2013/05/14 20:50:40  dechavez
 * initial production release
 *
 */
