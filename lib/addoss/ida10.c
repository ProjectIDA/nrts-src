#pragma ident "$Id: ida10.c,v 1.6 2015/11/04 22:00:48 dechavez Exp $"
/*======================================================================
 *
 *  ADDOSS to IDA10.11 converter
 *
 *====================================================================*/
#include "addoss.h"
#include "ida10.h"

/* hardcode the channel map, for now */

static struct {
    char name[IDA10_CNAMLEN+1];
    INT16 fact;
    INT16 mult;
    INT32 delay;
} chan[ADDOSS_NUMCHAN] = {
  {"ldh00",  1, 1, 34515800},
  {"lh200",  1, 1, 34515800},
  {"lh100",  1, 1, 34515800},
  {"lhz00",  1, 1, 34515800},
  {"bdh00", 50, 1, 34519700},
  {"bh200", 50, 1, 34519700},
  {"bh100", 50, 1, 34519700},
  {"bhz00", 50, 1, 34519700} 
};

int addossIDA10(UINT8 *start, ADDOSS_SENSOR_DATA_INT24 *sensor, SBD_MO_HEADER *header, UINT32 zerotime)
{
int i;
UINT32 now;
UINT8 *ptr, descriptor;
UINT16 nbytes;

    if (sensor->id > ADDOSS_MAX_SENSOR_ID) return ADDOSS_ILLEGAL_SENSOR_ID;

    ptr = start;
    memset(ptr, 0xee, ADDOSS_IDA10_PKTLEN);

    /* 10.11 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 11;  /* subformat 11 */

    ptr += utilPackUINT64(ptr, sensor->tstamp);
    ptr += utilPackUINT32(ptr, zerotime);
    ptr += utilPackUINT32(ptr, chan[sensor->id].delay);
    ptr += utilPackUINT32(ptr, ADDOSS_TICS_PER_SEC);
    ptr += utilPackUINT64(ptr, atoll(header->imei));
    ptr += utilPackUINT16(ptr, header->momsn);
    ptr += utilPackUINT32(ptr, header->tstamp);
    ptr += utilPackUINT32(ptr, 0); /* sequence number will go here */
    now = (UINT32) time(NULL) - SAN_EPOCH_TO_1970_EPOCH;
    ptr += utilPackUINT32(ptr, now); /* host time stamp will go here */
    ptr += IDA1011_RESERVED_BYTES;
    nbytes = (IDA10_FIXEDHDRLEN - IDA10_SUBFORMAT_11_HDRLEN) + (sensor->nsamp * sizeof(INT32));
    ptr += utilPackUINT16(ptr, nbytes);

    /* stream name from lookup table */

    ptr += utilPackBytes(ptr, (UINT8 *) chan[sensor->id].name, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    ptr += utilPackUINT16(ptr, sensor->nsamp);

    /* sample rate from lookup table */

    ptr += utilPackINT16(ptr, chan[sensor->id].fact);
    ptr += utilPackINT16(ptr, chan[sensor->id].mult);

    /* data in network byte order */

    for (i = 0; i < sensor->nsamp; i++) ptr += utilPackINT32(ptr, sensor->data[i]);

    /* return IDA10 packet length */

    return (int) (ptr - start);
}

/* Revision History
 *
 * $Log: ida10.c,v $
 * Revision 1.6  2015/11/04 22:00:48  dechavez
 * explict cast of arguments needed to calm OS X gcc
 *
 * Revision 1.5  2014/01/27 17:25:36  dechavez
 *  pre-fill working space with 0xee instead of 0x33 (cosmetic only)
 *
 * Revision 1.4  2013/09/20 16:17:42  dechavez
 * changed IDA1010_x constants to IDA1011_x versions
 *
 * Revision 1.3  2013/07/19 17:47:23  dechavez
 * include hard-coded filter delay and emit IDA10.11 packets
 *
 * Revision 1.2  2013/05/14 20:50:40  dechavez
 * initial production release
 *
 */
