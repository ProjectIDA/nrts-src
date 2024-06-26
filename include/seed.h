#pragma ident "$Id: seed.h,v 1.5 2012/02/14 19:43:42 dechavez Exp $"
/*
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *
 */

#ifndef seed_h_included
#define seed_h_included

#include "platform.h"
#include "ida.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Selected name lengths (all less trailing NULL) */

#define SEED_SNAMLEN 5  /* station name        */
#define SEED_CNAMLEN 3  /* channel code        */
#define SEED_LNAMLEN 2  /* location identifier */
#define SEED_NNAMLEN 2  /* network code        */

/* Selected encoding formats */

#define SEED_INT_16 1  /* 16 bit integers                      */
#define SEED_INT_24 2  /* 24 bit integers                      */
#define SEED_INT_32 3  /* 32 bit integers                      */
#define SEED_IEEE_F 4  /* IEEE floading point                  */
#define SEED_IEEE_D 5  /* IEEE double precision floating point */
#define SEED_STEIM1 10 /* Steim (1) compression                */
#define SEED_STEIM2 11 /* Steim (2) compression                */

#define SEED_LTL_ENDIAN 0
#define SEED_BIG_ENDIAN 1

/* Selected flag masks */

#define SEED_FLAG_START 0x08  /* B00001000 = start of time series */
#define SEED_FLAG_STOP  0x10  /* B00010000 = end   of time series */

/* Misc. constants */

#define SEED_MINRECEXP  8
#define SEED_MAXRECEXP 16

/* Structure templates */

typedef struct seed_fsdh {
    UINT32   seqno;                 /* sequence number                   */
    char    staid[SEED_SNAMLEN+1]; /* station identifier code           */
    char    locid[SEED_LNAMLEN+1]; /* location identifier               */
    char    chnid[SEED_CNAMLEN+1]; /* channel identifier                */
    char    netid[SEED_NNAMLEN+1]; /* network code                      */
    REAL64  start;                 /* record start time                 */
    UINT16  nsamp;                 /* number of samples                 */
    INT16   srfact;                /* sample rate factor                */
    INT16   srmult;                /* sample rate multiplier            */
    UINT8   active;                /* activity flags                    */
    UINT8   ioclck;                /* I/O and clock flags               */
    UINT8   qual;                  /* data quality flags                */
    UINT8   more;                  /* number of blockettes that follow  */
    INT32   tcorr;                 /* time correction                   */
    INT16   bod;                   /* offset to begining of data        */
    INT16   first;                 /* first blockette                   */
    /* The following are for internal convenience and not part of SEED  */
    UINT32  order;                 /* byte ordering used in raw data    */
    int     swap;                  /* swap/no swap flag, based on above */
} SEED_FSDH;

typedef struct seed_b1000 {
    UINT16 next;     /* next blockette's byte number */
    UINT8  format;   /* encoding format              */
    UINT8  order;    /* word order                   */
    UINT8  length;   /* record length                */
} SEED_B1000;

typedef struct seed_b1001 {
    UINT16 next;    /* next blockette's byte number */
    UINT8  tqual;   /* vendor specificy qual eval   */
    UINT8  usec;    /* offset to start time in usec */
    UINT8  fcount;  /* frame count                  */
} SEED_B1001;

/* A mini-seed packet */

struct seed_minipacket {
    SEED_FSDH fsdh;   /* a standard FSDH,                   */
    SEED_B1000 b1000; /* followed by a blockette 1000,      */
    INT32 *data;      /* followed by some decompressed data */
    int datlen;       /* size of above array                */
    /* The following come from stuff in the sub-headers */
    char *sname;      /* station name                       */
    char *cname;      /* channel name                       */
    char *nname;      /* network name                       */
    UINT32 nsamp;     /* number of samples in data          */
    REAL64 beg;       /* time of first sample               */
    REAL64 sint;      /* sample interval                    */
    UINT32 srclen;
    UINT32 order;
};
    
/* Function prototypes */

/* b1000.c */
void seed_load1000(UINT8 *dest, struct seed_b1000 *src, UINT32 order);
int seed_b1000(struct seed_b1000 *dest, UINT8 *src);

/* b1001.c */
void seed_load1001(UINT8 *dest, struct seed_b1001 *src, UINT32 order);
int seed_b1001(struct seed_b1001 *dest, UINT8 *src);

/* decode.c */
int seed_minihdr(struct seed_minipacket *output, UINT8 *buffer);

/* fsdh.c */
void seed_loadfsdh(UINT8 *dest, struct seed_fsdh *src, UINT32 order);
int seed_fsdh(struct seed_fsdh *dest, UINT8 *src);

/* mini.c */
INT32 seed_mini(INT32 *input, INT32 count, struct seed_fsdh *fsdh, UINT8 *output, int outexp, INT32 *used);

/* read.c */
int seed_readmini(int fd, struct seed_minipacket *output, UINT8 *buffer, size_t blksiz, int to);

/* sint.c */
void seed_sintsplit(double sint, INT16 *fact, INT16 *mult);
double seed_makesint(INT16 fact, INT16 mult);

/* tear.c */
int seed_timetear(struct seed_minipacket *prev, struct seed_minipacket *crnt, INT32 *error);

/* time.c */
UINT8 *seed_dtconv(UINT8 *output, double dtime, UINT32 order);
double seed_ttodt(UINT8 *seedtime, UINT32 *order);

/* toida.c */
void *seed_toidainit(char *scsc, IDA *ida, int nbuf, int tolerance);
int seed_type(UINT8 *src, INT32 *next_ptr, int swap);
int seed_minitoida(struct seed_minipacket *seed, void *info_ptr, UINT8 ***out);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: seed.h,v $
 * Revision 1.5  2012/02/14 19:43:42  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.4  2007/01/11 17:45:19  dechavez
 * cleaned up prototypes, added platform.h and some of the stdtypes.h data types
 *
 * Revision 1.3  2006/02/09 20:08:17  dechavez
 * updated prototypes
 *
 * Revision 1.2  2004/06/25 18:34:57  dechavez
 * C++ compatibility
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
