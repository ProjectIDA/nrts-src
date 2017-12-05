#pragma ident "$Id: liss.h,v 1.8 2011/11/03 17:43:38 dechavez Exp $"
/*======================================================================
 *
 * Defines, templates, and prototypes for LISS support library
 *
 *====================================================================*/
#ifndef liss_h_included
#define liss_h_included

#include "platform.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Selected name lengths (all less trailing NULL) */

#define LISS_SNAMLEN 5  /* station name        */
#define LISS_CNAMLEN 3  /* channel code        */
#define LISS_LNAMLEN 2  /* location identifier */
#define LISS_NNAMLEN 2  /* network code        */

/* Selected encoding formats */

#define LISS_INT_16 1  /* 16 bit integers                      */
#define LISS_INT_24 2  /* 24 bit integers                      */
#define LISS_INT_32 3  /* 32 bit integers                      */
#define LISS_IEEE_F 4  /* IEEE floading point                  */
#define LISS_IEEE_D 5  /* IEEE double precision floating point */
#define LISS_STEIM1 10 /* Steim (1) compression                */
#define LISS_STEIM2 11 /* Steim (2) compression                */

#define LISS_LTL_ENDIAN 0
#define LISS_BIG_ENDIAN 1

/* Selected flag masks */

#define LISS_FLAG_START 0x08  /* B00001000 = start of time series */
#define LISS_FLAG_STOP  0x10  /* B00010000 = end   of time series */

/* FSDH I/O and clock flag bits */

#define LISS_IOC_PARITY_ERROR    0x01
#define LISS_IOC_LONG_READ       0x02
#define LISS_IOC_SHORT_READ      0x04
#define LISS_IOC_START_OF_SERIES 0x08
#define LISS_IOC_END_OF_SERIES   0x10
#define LISS_IOC_CLOCK_LOCKED    0x20

/* FSDH data quality flag bits */

#define LISS_DQ_SATURATED     0x01
#define LISS_DQ_CLIPPED       0x02
#define LISS_DQ_SPIKES        0x04
#define LISS_DQ_GLITCHES      0x08
#define LISS_DQ_MISSING_DATA  0x10
#define LISS_DQ_SYNC_ERR      0x20
#define LISS_DQ_FILTER_CHARGE 0x40
#define LISS_DQ_TIME_SUSPECT  0x80

/* option bits */

#define LISS_OPTION_DECODE 0x01

/* unpack status (all errors must have values less than zero) */

#define LISS_PKT_OK           1
#define LISS_UNSUPPORTED     -1
#define LISS_NOT_DATA        -2
#define LISS_NOT_MINISEED    -3
#define LISS_NONSENSE_PKT    -4
#define LISS_DECOMP_ERROR_A  -5
#define LISS_DECOMP_ERROR_B  -6
#define LISS_DECOMP_ERROR_C  -7
#define LISS_DECOMP_ERROR_D  -8
#define LISS_DECOMP_ERROR_E  -9
#define LISS_DECOMP_ERROR_F -10
#define LISS_DECOMP_ERROR_G -11
#define LISS_DECOMP_ERROR_H -12
#define LISS_HEARTBEAT      -1000

/* Misc. constants */

#define LISS_MINRECEXP  8
#define LISS_MAXRECEXP 16
#define LISS_RETRY_INTERVAL 30
#define LISS_DEFAULT_TIMEOUT 30
#define LISS_DEFAULT_BLKSIZ 512

#define LISS_FSDH_LENGTH 48
#define LISS_MAX_FSDH_SEQNO 999999
#define LISS_FSDH_SEQNO_MODULO (LISS_MAX_FSDH_SEQNO + 1)

/* IMPORTANT!  If this ever changes, be sure to update isi.h:ISI_MSEED_HDR_LEN
 *             and rebuild the world (and disk loops!)
 */
#define LISS_MSEED_HDR_LEN 56 /* FSDH and Blockette 1000 */

/* Structure templates */

typedef struct liss_fsdh {
    long    seqno;                 /* sequence number                   */
    char    qcode;                 /* data header/quality indicator     */
    char    staid[LISS_SNAMLEN+1]; /* station identifier code           */
    char    locid[LISS_LNAMLEN+1]; /* location identifier               */
    char    chnid[LISS_CNAMLEN+1]; /* channel identifier                */
    char    netid[LISS_NNAMLEN+1]; /* network code                      */
    double  start;                 /* record start time                 */
    short   nsamp;                 /* number of samples                 */
    short   srfact;                /* sample rate factor                */
    short   srmult;                /* sample rate multiplier            */
    char    active;                /* activity flags                    */
    char    ioclck;                /* I/O and clock flags               */
    char    qual;                  /* data quality flags                */
    char    more;                  /* number of blockettes that follow  */
    long    tcorr;                 /* time correction                   */
    short   bod;                   /* offset to begining of data        */
    short   first;                 /* first blockette                   */
    /* The following are for internal convenience and not part of LISS  */
    UINT32  order;                 /* byte ordering used in raw data    */
    int     swap;                  /* swap/no swap flag, based on above */
} LISS_FSDH;

typedef struct liss_b1000 {
    short next;     /* next blockette's byte number */
    char  format;   /* encoding format              */
    char  order;    /* word order                   */
    char  length;   /* record length                */
    char  padding;  /* pad to an even number        */
} LISS_B1000;

typedef struct {
    LISS_FSDH  fsdh;  /* a standard FSDH,                   */
    LISS_B1000 b1000; /* followed by a blockette 1000       */
} LISS_MSEED_HDR;

/* A LISS (mini-seed) packet */

#define LISS_MAX_BLKSIZ 8192

typedef struct liss_pkt {
    LISS_MSEED_HDR hdr; /* FSDH and B1000 */
    UINT32 order;
    INT32 data[LISS_MAX_BLKSIZ]; /* decompressed/reordered data */
    INT32 srclen;
    UINT8 *bod; /* points to original data */
    int status; /* decompression status */
    UINT32 flags; /* processing options */
} LISS_PKT;

/* Connection handle */

typedef struct liss {
    SOCKET sd;        /* socket descriptor */
    UINT16 port;      /* port number       */
    char server[MAXPATHLEN]; /* server name or address */
    time_t connect;   /* time of connection */
    UINT32 count;     /* received packet count */
    UINT8 buf[LISS_MAX_BLKSIZ]; /* receive buffer */
    int blksiz;       /* expected packet length */
    int to;           /* read timeout interval */
    LOGIO *lp;        /* logging handle */
} LISS;
    
/* Function prototypes */

/* open.c */
LISS *lissClose(LISS *liss);
LISS *lissOpen(char *server, int port, int blksiz, int to, LOGIO *lp);

/* pack.c */
BOOL lissUnpackB1000(LISS_B1000 *dest, UINT8 *src);
void lissUnpackFSDH(LISS_FSDH *dest, UINT8 *src);
int  lissUnpackMiniSeed(LISS_PKT *out, UINT8 *src, UINT32 flags);
int  lissPackMseedHdr(UINT8 *dest, LISS_MSEED_HDR *mseed);

/* read.c */
BOOL lissRead(LISS *liss, LISS_PKT *pkt, UINT32 flags);

/* steim.c */
int lissDecompressSteim1(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count);
int lissDecompressSteim2(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count);

/* string.c */
char *lissDataFormatString(int code);
char *lissMiniSeedHdrString(LISS_MSEED_HDR *hdr, char *buf);
void lissPrintMiniSeedHdr(FILE *fp, LISS_MSEED_HDR *hdr);

/* time.c */
void lissEpochTimeToSeed(UINT8 *output, REAL64 dtime, UINT32 order);
REAL64 lissSeedTimeToEpoch(UINT8 *src, UINT32 *order);
REAL64 lissSint(LISS_FSDH *fsdh);

/* type.c */
INT16 lissTypeAndOffset(UINT8 *src, INT32 *next_ptr, BOOL swap);

/* version.c */
char *lissVersionString(VOID);
VERSION *lissVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: liss.h,v $
 * Revision 1.8  2011/11/03 17:43:38  dechavez
 * replaced fsdh and b1000 in LISS_PKT with LISS_MSEED_HDR hdr
 *
 * Revision 1.7  2011/10/31 16:54:26  dechavez
 * include warning about external dependencies on LISS_MSEED_HDR_LEN
 *
 * Revision 1.6  2011/10/28 21:44:35  dechavez
 * added LISS_MSEED_HDR type, updated prototypes
 *
 * Revision 1.5  2010/01/26 22:33:25  dechavez
 * added padding bytes to LISS_B1000 to keep it an even length
 *
 * Revision 1.4  2009/11/19 19:23:08  dechavez
 * added LISS_HEARTBEAT (aap)
 *
 * Revision 1.3  2008/02/03 21:22:29  dechavez
 * changed lissUnpackMiniSeed return value in prototype
 *
 * Revision 1.2  2008/01/07 21:55:08  dechavez
 * added various constants, added support for optional data, decoding in LISS_PKT handle
 *
 * Revision 1.1  2005/09/30 18:08:59  dechavez
 * initial release
 *
 */
