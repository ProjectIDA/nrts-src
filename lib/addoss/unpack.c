#pragma ident "$Id: unpack.c,v 1.3 2013/05/14 20:50:40 dechavez Exp $"
/*======================================================================
 * 
 * Unpack ADDOSS frames from little-endian order into host order
 *
 *====================================================================*/
#include "addoss.h"
#include "util.h"

static int UnkcapOSGtstamp(UINT8 *start, ADDOSS_OSG_TIMESTAMP *dest)
{
UINT32 julianday;
UINT8 *ptr;
struct css_date_time dt;

    ptr = start;
    ptr += utilUnkcapUINT16(ptr, &dest->year);
    dest->month  = *ptr++;
    dest->day    = *ptr++;
    dest->hour   = *ptr++;
    dest->minute = *ptr++;
    dest->second = *ptr++;

    julianday = utilYmdtojd(dest->year, dest->month, dest->day);
    dt.date = dest->year * 1000 + julianday;
    dt.hour = dest->hour;
    dt.minute = dest->minute;
    dt.second = (float) dest->second;
    util_htoe(&dt);
    dest->epoch = (UINT32) dt.epoch;

    return (int) (ptr - start);
}

static void PrintOffset(UINT8 *start, UINT8 *ptr, char *where)
{
    printf("%s offset = %d\n", where, (int) (ptr - start) + 2);
}

int addossUnpackOSGstatus(UINT8 *start, ADDOSS_OSG_STATUS *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnkcapUINT16(ptr, &dest->reason);
    ptr += UnkcapOSGtstamp(ptr, &dest->tstamp);
    ptr += utilUnkcapUINT16(ptr, &dest->nCAMERR);
    ptr += utilUnkcapUINT16(ptr, &dest->nSendPacket);
    ptr += utilUnkcapUINT16(ptr, &dest->nSendPacketRetries);
    ptr += utilUnkcapUINT16(ptr, &dest->nPacketsSent);
    ptr += utilUnkcapUINT16(ptr, &dest->nPacketsReceived);
    dest->nQueue = *ptr++;
    dest->lastpacket.mode = *ptr++;
    dest->lastpacket.PSKerror = *ptr++;
    dest->lastpacket.nFramesExpected = *ptr++;
    dest->lastpacket.nCRCerrors = *ptr++;
    dest->lastpacket.msesign = *ptr++;
    ptr += utilUnkcapUINT32(ptr, &dest->lastpacket.MeanSquaredError);
    dest->lastpacket.rsssign = *ptr++;
    ptr += utilUnkcapUINT32(ptr, &dest->lastpacket.RecvSignalStrength);
    dest->lastpacket.isnrsign = *ptr++;
    ptr += utilUnkcapUINT32(ptr, &dest->lastpacket.InputSNR);
    dest->lastpacket.osnrsign = *ptr++;
    ptr += utilUnkcapUINT32(ptr, &dest->lastpacket.OutputSNR);
    dest->lastpacket.sdnsign = *ptr++;
    ptr += utilUnkcapUINT32(ptr, &dest->lastpacket.StdDevNoise);
    ptr += UnkcapOSGtstamp(ptr, &dest->lastpacket.tstamp);

    return (int) (ptr - start);
}

static int UnkcapOBP_Chanfo(UINT8 *start, ADDOSS_OBP_CHANFO *chan)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnkcapUINT32(ptr, &chan->first_lba);
    ptr += utilUnkcapUINT32(ptr, &chan->last_lba);
    ptr += utilUnkcapUINT32(ptr, &chan->crnt_lba);
    ptr += utilUnkcapUINT32(ptr, &chan->srate);

    return (int) (ptr - start);
}

int addossUnpackOBPstatus(UINT8 *start, ADDOSS_OBP_STATUS *dest)
{
int i;
UINT8 *ptr;

    ptr = start;
    for (i = 0; i < ADDOSS_MAXCHAN; i++) ptr += UnkcapOBP_Chanfo(ptr, &dest->chan[i]);

    return ADDOSS_FRAME_PAYLOAD_LEN;
}

int addossUnpackReqSensorData(UINT8 *start, ADDOSS_OBP_CHANFO *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += 2; /* skip over padding */
    ptr += utilUnkcapUINT32(ptr, &dest->first_lba);
    ptr += utilUnkcapUINT32(ptr, &dest->last_lba);

    return ADDOSS_FRAME_PAYLOAD_LEN;
}

static int UnpackSensorDataTstamp(UINT8 *start, uint64_t *dest)
{
int i;
UINT8 *ptr;
union {
    UINT8 i8[8];
    UINT64 i64;
} tmp;

    tmp.i64 = 0;

    ptr = start;
    for (i = 7; i > 0; i--) tmp.i8[i] = *ptr++;
    utilUnpackUINT64(tmp.i8, dest);

    return (int) (ptr - start);
}

static int UnpackInt24DataSample(UINT8 *start, INT32 *dest)
{
    *dest = ((start[2] << 24) | (start[1] << 16) | start[0] << 8) >> 8;

    return (int) 3;
}

int addossUnpackSensorDataInt24(UINT8 *start, ADDOSS_SENSOR_DATA_INT24 *dest)
{
int i;
UINT8 *ptr;
UINT64 b, bs;
static char *fid = "addossUnpackSensorDataInt24";

    ptr = start;

    dest->id = *ptr++;
    ptr += UnpackSensorDataTstamp(ptr, &dest->tstamp);
    for (i = 0; i < ADDOSS_SENSOR_DATA_NSAMP; i++) ptr += UnpackInt24DataSample(ptr, &dest->data[i]);
    dest->nsamp = ADDOSS_SENSOR_DATA_NSAMP;

    return (int) (ptr - start);
}

/* Revision History
 *
 * $Log: unpack.c,v $
 * Revision 1.3  2013/05/14 20:50:40  dechavez
 * initial production release
 *
 */
