#pragma ident "$Id: qdplus.h,v 1.15 2011/04/07 22:41:20 dechavez Exp $"
/*======================================================================
 *
 *  QDP packet extensions
 *
 *====================================================================*/
#ifndef qdplus_h_included
#define qdplus_h_included

#include "platform.h"
#include "zlib.h"
#include "qdp.h"
#include "isi.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  Wrapped QDP packet
 *
 *  Offset Type    Length      Description
 *    0    UINT64     8        Digitizer serial number
 *    8    UINT32     4        ISI disk loop sequence number signature
 *   12    UINT64     8        ISI disk loop sequence number counter
 *   20    UINT8   12 to 576   Raw QDP packet
 */

typedef struct {
    UINT64 serialno;        /* digitizer serial number */
    ISI_SEQNO seqno;        /* packet sequence number */
    QDP_PKT qdp;            /* the packet, decoded */
} QDPLUS_PKT;

#define QDPLUS_HDRLEN 20
#define QDPLUS_PKTLEN (QDPLUS_HDRLEN + QDP_MAX_MTU)

#define QDPLUS_SERIALNO_STRING_LEN 16

/* Identifier tags for DT_USER packets */

#define QDPLUS_DT_USER_PAROSCI 1
/*
    Offset Type    Length       Description
      0    UINT8         1      type identifier (QDPLUS_DT_USER_PAROSCI)
      1    UINT8         1      reserved
      2    CHAR          3      channel name
      5    CHAR          2      location code
      7    UINT8         1      reserved
      8    UINT64        8      sample interval, nanoseconds
     16    UINT64        8      time of first sample (nanosecs since 1/1/1999)
     24    UINT8         1      receiver specific clock status
     25    UINT8         1      device independent clock status bitmap
     26    INT32         4      uncompressed first sample
     28    UINT8        10      reserved
     38    UINT16        2      number of bytes to follow
     40    UINT16      var      up to 248 UINT16 first differences
*/

#define QDPLUS_DT_USER_PAROSCI_MAX_PAYLOAD 496
#define QDPLUS_DT_USER_PAROSCI_MAXSAMP ((QDPLUS_DT_USER_PAROSCI_MAX_PAYLOAD / sizeof(INT16)) + 1)

/* time tag is designed to be identical to IDA10_GENTAG, but is not
 * explicitly one to avoid introducing libida10 dependencies to libqdplus
 */

typedef struct {
    UINT64 tstamp;  /* nanoseconds since 1/1/1999 */
    struct {
        UINT8 receiver; /* receiver specific clock status */
        UINT8 generic;  /* device independent status bitmap */
#define QDPLUS_GENTAG_LOCKED     0x01
#define QDPLUS_GENTAG_SUSPICIOUS 0x02
#define QDPLUS_GENTAG_DERIVED    0x04
    } status;
} QDPLUS_GENTAG;

/* A decoded DT_USER payload when type is QDPLUS_DT_USER_PAROSCI */

typedef struct {
    char chn[ISI_CHNLEN+1]; /* channel name */
    char loc[ISI_LOCLEN+1]; /* location code */
    UINT64 sint;            /* sample interval, nanosec */
    QDPLUS_GENTAG tofs;     /* time of first sample */
    INT32 first;            /* uncompressed first sample */
    UINT16 ndiff;           /* number of first differences to follow */
    INT16 *diff;            /* points to up to 248 UINT16 first differences */
} QDPLUS_PAROSCI;

/* All types of user packets (even though I'm sure we'll only ever see QDPLUS_PAROSCI */

typedef struct {
    int type; /* QDPLUS_DT_USER_x */
    union {
        QDPLUS_PAROSCI parosci;
    } data;
} QDPLUS_USERPKT;

/* QDP metadata for a single instrument */

typedef struct {
    char path[MAXPATHLEN+1]; /* path name of metadata file */
    ISI_SEQNO_RANGE range;   /* range of sequence numbers for which data valid */
    QDP_META data;           /* the metadata */
} QDPLUS_META_DATUM;

typedef struct {
    UINT64 serialno;  /* digitizer serial number */
    LNKLST *list;     /* linked list of QDPLUS_META_DATUM structures */
} QDPLUS_META_DATA;

/* Used for recovering state */

typedef struct {
    ISI_SEQNO seqno;
    char chn[QDP_CNAME_LEN+1];
    char loc[QDP_LNAME_LEN+1];
    INT32 offset;
} QDPLUS_STATE;

#define QDPLUS_FLAG_CLEAN 0xabcd
#define QDPLUS_FLAG_DIRTY 0xcdef

#define QDPLUS_STATE_OK    0
#define QDPLUS_STATE_BUG   1
#define QDPLUS_STATE_NOMEM 2
#define QDPLUS_STATE_IOERR 3
#define QDPLUS_STATE_EMPTY 4
#define QDPLUS_STATE_DIRTY 5

/* Handle for processing a packet stream */

typedef struct {
    UINT64 serialno;  /* digitizer serial number */
    QDP_LCQ lcq;      /* logical channel queue for this digitizer */
    LNKLST *state;    /* linked list of QDPLUS_STATE structures */
} QDPLUS_DIGITIZER;

typedef struct {
    struct {
        char *meta;      /* metadata directory */
        char *state;     /* hlp state file */
    } path;
    char *site;      /* site name */
    QDP_LCQ_PAR lcq; /* handle parameters */
} QDPLUS_PAR;

#define QDPLUS_DEFAULT_PAR {NULL, NULL, NULL, QDP_DEFAULT_LCQ_PAR}

typedef struct {
    struct {
        LNKLST *digitizer; /* linked list of QDPLUS_DIGITIZER structures */
        LNKLST *meta;      /* linked list of QDPLUS_META_DATA structures */
    } list;
    QDPLUS_PAR par;   /* user parameters */
    int state;        /* status of state file */
} QDPLUS;

/* decode.c */
int qdplusUnpackWrappedQDP(UINT8 *start, QDPLUS_PKT *dest);
void qdplusDecodeUser(UINT8 *start, QDPLUS_USERPKT *dest);

/* digitizer.c */
void qdplusDestroyDigitizer(QDPLUS_DIGITIZER *digitizer);
void qdplusDestroyDigitizerList(LNKLST *list);
QDPLUS_DIGITIZER *qdplusGetDigitizer(QDPLUS *handle, UINT64 serialno);
QDPLUS_DIGITIZER *qdplusLoadPacket(QDPLUS *handle, QDPLUS_PKT *pkt);

/* handle.c */
void qdplusDestroyHandle(QDPLUS *handle);
QDPLUS *qdplusCreateHandle(QDPLUS_PAR *par);
void qdplusSetLcqStateFlag(QDPLUS *handle, int value);

/* io.c */
BOOL qdplusRead(FILE *fp, QDPLUS_PKT *pkt);
BOOL qdplusReadgz(gzFile *fp, QDPLUS_PKT *pkt);
BOOL qdplusWrite(FILE *fp, QDPLUS_PKT *pkt);

/* meta.c */
void qdplusDestroyMetaData(QDPLUS_META_DATA *meta);
void qdplusDestroyMetaDataList(LNKLST *list);
LNKLST *qdplusReadMetaData(char *base);
void qdplusPrintMetaDatum(FILE *fp, QDPLUS_META_DATUM *datum);
void qdplusPrintMetaData(FILE *fp, QDPLUS_META_DATA *meta);
void qdplusPrintMetaDataList(FILE *fp, LNKLST *list, BOOL verbose);
BOOL qdplusInitializeLCQMetaData(QDP_LCQ *lcq, LNKLST *head, UINT64 serialno, ISI_SEQNO *seqno);

/* print.c */
void qdplusPrintPkt(FILE *fp, QDPLUS_PKT *pkt, UINT16 print);

/* process.c */
QDP_LCQ *qdplusProcessPacket(QDPLUS *handle, QDPLUS_PKT *pkt);

/* state.c */
void qdplusPrintState(FILE *fp, QDPLUS *handle);
BOOL qdplusSaveState(QDPLUS *handle);
int  qdplusReadStateFile(QDPLUS *handle);
BOOL qdplusStateSeqnoLimits(QDPLUS *handle, ISI_SEQNO *beg, ISI_SEQNO *end);

/* version.c */
char *qdplusVersionString(void);
VERSION *qdplusVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* qdplus_h_included */

/* Revision History
 *
 * $Log: qdplus.h,v $
 * Revision 1.15  2011/04/07 22:41:20  dechavez
 * replaced QDPLUS_DT_USER_AUX with QDPLUS_DT_USER_PAROSCI, defined QDPLUS_USERPKT
 *
 * Revision 1.14  2009/07/02 16:56:05  dechavez
 * updated prototypes
 *
 * Revision 1.13  2009/02/23 21:48:19  dechavez
 * added new prototypes
 *
 * Revision 1.12  2008/12/15 23:23:47  dechavez
 * added QDPLUS_STATE values
 *
 * Revision 1.11  2007/05/17 22:25:39  dechavez
 * initial production release
 *
 */
