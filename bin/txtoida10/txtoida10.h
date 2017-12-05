#pragma ident "$Id: txtoida10.h,v 1.2 2011/10/12 17:52:04 dechavez Exp $"
#ifndef txtoida10_h_defined
#define txtoida10_h_defined

extern char *VersionIdentString;

#include "util.h"
#include "ida10.h"
#include "isi.h"
#include "iacp.h"

#define TXTOIDA10_OPTION_LCASE 0x00000001
#define TXTOIDA10_DEFAULT_DEPTH 20

typedef void (*DECODE_FUNC)(LNKLST *list, char *buf, UINT32 options);

#define NNAME_LEN 2
#define SNAME_LEN 4
#define CNAME_LEN 3
#define LNAME_LEN 2
#define IDENT_LEN (NNAME_LEN+1 + SNAME_LEN+1 + CNAME_LEN + LNAME_LEN)

typedef struct {
    char ident[IDENT_LEN+1];      /* stream identifier */
    UINT64 tons;                  /* expected time of next sample */
    UINT64 sint;                  /* sample interval */
    int nsamp;                    /* number of samples collected so far */
    struct {
        int tofs;                 /* index to timestamp field buf */
        int nsamp;                /* index to nsamp field of buf */
        int data;                 /* index to next datum field of buf */
    } offset;
    UINT8 buf[IDA10_FIXEDRECLEN]; /* the packet */
} PROTO_PACKET;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    float second;
    UINT8 status;
    IDA10_GENTAG gentag;
} TIMESTAMP;

typedef struct {
    char ident[IDENT_LEN+1];            /* stream identifier */
    char nname[NNAME_LEN+1];            /* network code */
    char sname[SNAME_LEN+1];            /* station code */
    char cname[CNAME_LEN+1];            /* channel code */
    char loc[LNAME_LEN+1];              /* location code */
    char chnloc[CNAME_LEN+LNAME_LEN+1]; /* channel + location combined */
    TIMESTAMP tstamp;                   /* time stamp data */
    INT32 value;                        /* data sample */
    UINT64 sint;                        /* sample interval, nanosec */
    UINT8 gain;                         /* float to int conversion gain */
} LINE_SAMPLE;

/*  Function declarations  */

/* list.c */
LNKLST_NODE *GetProtoPacket(LNKLST *head, LINE_SAMPLE *sample);

/* log.c */
LOGIO *GetLogHandle();
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec);

/* proto.c */
BOOL ConsistentPacket(PROTO_PACKET *pkt, LINE_SAMPLE *sample);
void InitProtoPacket(PROTO_PACKET *new, LINE_SAMPLE *sample);
void FlushPacket(PROTO_PACKET *pkt);
void AppendSample(LNKLST *head, LINE_SAMPLE *sample);
BOOL SetDlOutput(char *string, LOGIO *lp);

/* rev01.c */
void DecodeRev01(LNKLST *list, char *buf, UINT32 options);

/* util.c */
void BuildIdentString(char *dest, char *nname, char *sname, char *cname);
void BuildGentag(IDA10_GENTAG *dest, TIMESTAMP *tstamp);

#endif /* txtoida10_h_defined */

/* Revision History
 *
 * $Log: txtoida10.h,v $
 * Revision 1.2  2011/10/12 17:52:04  dechavez
 * updated prototypes, added TXTOIDA10_DEFAULT_DEPTH
 *
 * Revision 1.1  2011/10/04 19:49:28  dechavez
 * initial release
 *
 */
