#pragma ident "$Id: addoss.h,v 1.4 2014/01/27 17:24:10 dechavez Exp $"
#ifndef addoss_include_defined
#define addoss_include_defined

/* platform specific stuff */

#include "platform.h"
#include "list.h"
#include "util.h"
#include "ida10.h"
#include "sbd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* constants */

#define ADDOSS_FRAME_ID_LEN      2
#define ADDOSS_FRAME_PAYLOAD_LEN 254
#define ADDOSS_FRAME_LEN         (ADDOSS_FRAME_ID_LEN + ADDOSS_FRAME_PAYLOAD_LEN) /* 256 byte ADDOSS frames */
#define ADDOSS_TICS_PER_SEC      32768000
#define ADDOSS_SAMPLES_PER_FRAME 82
#define ADDOSS_IDA10_PKTLEN      (IDA10_FIXEDHDRLEN + (ADDOSS_SAMPLES_PER_FRAME * sizeof(UINT32)))

/* payload idents */

#define ADDOSS_ID_SENSOR_DATA_COMP  0x01 
#define ADDOSS_ID_REQ_OSG_STATUS    0x02 
#define ADDOSS_ID_OSG_STATUS        0x03 
#define ADDOSS_ID_REQ_OBP_STATUS    0x04 
#define ADDOSS_ID_OBP_STATUS        0x05 
#define ADDOSS_ID_SET_OBP_POWER     0x06 
#define ADDOSS_ID_SET_SENSOR_POWER  0x07 
#define ADDOSS_ID_REQ_SENSOR_DATA   0x08 
#define ADDOSS_ID_SENSOR_DATA_INT24 0x0a 

/* timestamp used by OSG status messages */

typedef struct {
    UINT16 year;
    UINT8  month;
    UINT8  day;
    UINT8  hour;
    UINT8  minute;
    UINT8  second;
    UINT32 epoch; /* above values converted to Unix time */
} ADDOSS_OSG_TIMESTAMP;

/* OSG status */

#define ADDOSS_OSG_STATUS_HEARTBEAT     1
#define ADDOSS_OSG_STATUS_LOST_OBS_COMM 2
#define ADDOSS_OSG_STATUS_RECV_OBS_PING 3

typedef struct {
    UINT16               reason;              /* one of ADDOSS_OSG_STATUS_x above */
    ADDOSS_OSG_TIMESTAMP tstamp;              /* what this measures depends on reason */
    UINT16               nCAMERR;             /* number of CAMERR messages since initialization */
    UINT16               nSendPacket;         /* number of times SendPacket was called since initialization */
    UINT16               nSendPacketRetries;  /* number of SendPacket retries since initialization */
    UINT16               nPacketsSent;        /* number of successful SendPacket calls since initialization */
    UINT16               nPacketsReceived;    /* number of packets received since initialization */
    UINT8                nQueue;              /* number of messages currently in send queue */
    struct {
        UINT8  mode;                 /* mode */
        UINT8  PSKerror;             /* PSK Error Code */
        UINT8  nFramesExpected;      /* number of frames expected */
        UINT8  nCRCerrors;           /* number of frames iwth bad CRCs */
        UINT8  msesign;              /* equalizer mean squared error sign (0=pos, 1=neg) */
        UINT32 MeanSquaredError;     /* equalizer mean sqaured error */
        UINT8  rsssign;              /* received signal strength sign (0=pos, 1=neg) */
        UINT32 RecvSignalStrength;   /* received signal strength */
        UINT8  isnrsign;             /* input SNR sign (0=pos, 1=neg) */
        UINT32 InputSNR;             /* input SNR */
        UINT8  osnrsign;             /* output SNR sign (0=pos, 1=neg) */
        UINT32 OutputSNR;            /* output SNR */
        UINT8  sdnsign;              /* standard deviation of noise sign (0=pos, 1=neg) */
        UINT32 StdDevNoise;          /* standard deviation of noise */
        ADDOSS_OSG_TIMESTAMP tstamp; /* time of last packet receipt */
    } lastpacket;
} ADDOSS_OSG_STATUS;

typedef struct {
    UINT32 first_lba;
    UINT32 last_lba;
    UINT32 crnt_lba;
    UINT32 srate;
} ADDOSS_OBP_CHANFO;

#define ADDOSS_MAXCHAN 8
typedef struct {
    ADDOSS_OBP_CHANFO chan[ADDOSS_MAXCHAN];
} ADDOSS_OBP_STATUS;

#define ADDOSS_MAX_SENSOR_ID 7
#define ADDOSS_NUMCHAN (ADDOSS_MAX_SENSOR_ID + 1)

typedef struct {
    UINT8 id;
    UINT64 tstamp;
#define ADDOSS_SENSOR_DATA_LEN 246
#define ADDOSS_SENSOR_DATA_NSAMP (ADDOSS_SENSOR_DATA_LEN / 3) /* 3-byte samples */
    INT32 data[ADDOSS_SENSOR_DATA_NSAMP]; /* 3-byte samples */
    int nsamp; /* set by decoder as ADDOSS_SENSOR_DATA_NSAMP */
} ADDOSS_SENSOR_DATA_INT24;

/* function return codes */

#define ADDOSS_OK                 0
#define ADDOSS_ILLEGAL_SENSOR_ID -1
#define ADDOSS_UNSUPPORTED_FRAME -2

/* function prototypes */

/* extract.c */
LNKLST *addossExtractSensorDataInt24(SBD_MESSAGE *message);

/* ida10.c */
int addossIDA10(UINT8 *start, ADDOSS_SENSOR_DATA_INT24 *sensor, SBD_MO_HEADER *header, UINT32 zerotime);

/* print.c */
void addossPrintOSG_STATUS(FILE *fp, ADDOSS_OSG_STATUS *status);
void addossPrintOBP_STATUS(FILE *fp, ADDOSS_OBP_STATUS *status);
void addossPrintREQ_SENSOR_DATA(FILE *fp, ADDOSS_OBP_CHANFO *chan);
void addossPrintSENSOR_DATA_INT24(FILE *fp, ADDOSS_SENSOR_DATA_INT24 *sensor, ADDOSS_SENSOR_DATA_INT24 *prev);

/* string.c */
char *addossTypeString(int code);

/* unpack.c */
int addossUnpackOSGstatus(UINT8 *start, ADDOSS_OSG_STATUS *dest);
int addossUnpackOBPstatus(UINT8 *start, ADDOSS_OBP_STATUS *dest);
int addossUnpackReqSensorData(UINT8 *start, ADDOSS_OBP_CHANFO *dest);
int addossUnpackSensorDataInt24(UINT8 *start, ADDOSS_SENSOR_DATA_INT24 *dest);

/* version.c */
char *addossVersionString(VOID);
VERSION *addossVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif /* addoss_include_defined */

/* Revision History
 *
 * $Log: addoss.h,v $
 * Revision 1.4  2014/01/27 17:24:10  dechavez
 * added extract.c prototypes and required includes (list.h, sbd.h)
 *
 * Revision 1.3  2013/07/19 17:46:49  dechavez
 * changed addossIDA10() prototype to use 32-bit zerotime (for IDA10.11, which
 * specificies the reference time in secs instead of nsec)
 *
 * Revision 1.2  2013/05/11 22:48:40  dechavez
 * developmental checkpoint
 *
 * Revision 1.1  2013/03/07 21:04:06  dechavez
 * initial release
 *
 */
