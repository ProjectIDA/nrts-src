#pragma ident "$Id: wfdiscio.h,v 1.5 2015/11/04 22:16:14 dechavez Exp $"
#ifndef cssio_28_wfdiscio_h
#define cssio_28_wfdiscio_h
#include "platform.h"

#ifndef CSSIO_TRIM
#define CSSIO_TRIM(s,l) {int i;for(i=l-1;i>0,s[i-1]==' ';i--);s[i]='\0';}
#endif
#ifndef CSSIO_FPAD
#define CSSIO_FPAD(s,l) {int i;for(i=strlen(s);i<l;s[i++]=' ');}
#endif
#ifndef CSSIO_BFIL
#define CSSIO_BFIL(s,l) {int i;for(i=0;i<l;s[i++]=' ');}
#endif

/*  structure for waveform index info  */

struct cssio_wfdisc28 {
/*  1 */   INT32  date;       /* 3.0 jdate    */
/*  2 */   double time;       /* 3.0 time     */
/*  3 */   char   sta[8];     /* 3.0 sta      */
/*  4 */   char   chan[9];    /* 3.0 chan     */
/*  5 */   INT32  nsamp;      /* 3.0 nsamp    */
/*  6 */   float  smprat;     /* 3.0 smprate  */
/*  7 */   float  calib;      /* 3.0 calib    */
/*  8 */   float  calper;     /* 3.0 calper   */
/*  9 */   char   instyp[8];  /* 3.0 instype  */
/* 10 */   char   segtyp;     /* 3.0 segtype  */
/* 11 */   char   dattyp[4];  /* 3.0 datatype */
/* 12 */   char   clip;       /* 3.0 clip     */
/* 13 */   INT32  chid;       /* 3.0 chanid   */
/* 14 */   INT32  wfid;       /* 3.0 wfid     */
/* 15 */   char   dir[32];    /* 3.0 dir      */
/* 16 */   char   file[24];   /* 3.0 dfile    */
/* 17 */   INT32  foff;       /* 3.0 foff     */
/* 18 */   INT32  adate;      /* N/A in 3.0   */
/* 19 */   char   remark[32]; /* N/A in 3.0   */
}; 

#define CSSIO_WFDISC28_NFIELD 19

static struct cssio_wfdisc28 wfdisc28_null = {
/*  1 date    */ -1,
/*  2 time    */ -9999999999.999,
/*  3 sta     */ "_",
/*  4 chan    */ "_",
/*  5 nsamp   */ -1,
/*  6 smprat  */ -1.,
/*  7 calib   */ 0.,
/*  8 calper  */ -1.,
/*  9 instyp  */ "_",
/* 10 segtyp  */ '_',
/* 11 dattyp  */ "_",
/* 12 clip    */ '_',
/* 13 chid    */ -1,
/* 14 wfid    */ -1,
/* 15 dir     */ "_",
/* 16 file    */ "_",
/* 17 foff    */ 0,
/* 18 adate   */ -1,
/* 19 remark  */ "_"
};

#define CSSIO_WFDISC28_RCS \
"%ld%lf%*c%6c%*c%2c%ld%f%f%f%*c%6c%*c%c%*c%2c%*c%c%ld%ld%*c/**/%30c%*c%20c%ld%ld%*c%30c"
/* following extra read string is a gift from Keith */
/* NF means Number of Fields, (just like in awk) */
#define CSSIO_WFDISC28_NF    19
/* SCS means Scanf Control String, used by archive tape processing programs */
#define CSSIO_WFDISC28_SCS \
"%d %lf %s %s %d %f %f %f %s %c %s %c %d %d %s %s %d %d%*c%30[^\n] \n"
/* xxx_SIZE is external file record size in bytes including newline */
#define CSSIO_WFDISC28_SIZE 209

#define CSSIO_WFDISC28_RVL(SP) \
&(SP)->date,&(SP)->time,(SP)->sta,(SP)->chan,&(SP)->nsamp,\
&(SP)->smprat,&(SP)->calib,&(SP)->calper,(SP)->instyp,&(SP)->segtyp,\
(SP)->dattyp,&(SP)->clip,&(SP)->chid,&(SP)->wfid,(SP)->dir,\
(SP)->file,&(SP)->foff,&(SP)->adate,(SP)->remark

#define CSSIO_WFDISC28_WCS \
"%8ld %15.3f %-6.6s %-2.2s %8ld %11.7f %9.6f %7.4f %-6.6s %c %-2.2s %c %8ld %8ld %-30.30s %-20.20s %10ld %8ld %-30.30s\n"

#define CSSIO_WFDISC28_WVL(SP) \
(SP)->date,(SP)->time,(SP)->sta,(SP)->chan,\
(SP)->nsamp,(SP)->smprat,(SP)->calib,(SP)->calper,\
(SP)->instyp,(SP)->dattyp,(SP)->chid,\
(SP)->wfid,(SP)->dir,(SP)->file,(SP)->foff,\
(SP)->adate,(SP)->remark

#define CSSIO_WFDISC28_TRM(SP) \
CSSIO_TRIM((SP)->sta,7); CSSIO_TRIM((SP)->chan,3); CSSIO_TRIM((SP)->instyp,7); \
CSSIO_TRIM((SP)->dattyp,3); \
CSSIO_TRIM((SP)->dir,31); CSSIO_TRIM((SP)->file,21); CSSIO_TRIM((SP)->remark,31)

#define CSSIO_WFDISC28_FPD(SP) \
CSSIO_FPAD((SP)->sta,8); CSSIO_FPAD((SP)->chan,4); CSSIO_FPAD((SP)->instyp,8); \
CSSIO_FPAD((SP)->dattyp,4); \
CSSIO_FPAD((SP)->dir,32); CSSIO_FPAD((SP)->file,24); CSSIO_FPAD((SP)->remark,32)

#define CSSIO_WFDISC28_BFL(SP) \
CSSIO_BFIL((SP)->sta,8); CSSIO_BFIL((SP)->chan,4); CSSIO_BFIL((SP)->instyp,8); \
CSSIO_BFIL((SP)->dattyp,4); \
CSSIO_BFIL((SP)->dir,32); CSSIO_BFIL((SP)->file,24); CSSIO_BFIL((SP)->remark,32)

#endif /* cssio_28_wfdiscio_h */

/* Revision History
 *
 * $Log: wfdiscio.h,v $
 * Revision 1.5  2015/11/04 22:16:14  dechavez
 * changed %ld in CSSIO_WFDISC28_SCS macro to %d (to calm OS X)
 *
 * Revision 1.4  2012/02/14 19:47:21  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2011/03/17 17:20:39  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.2  2000/06/23 21:19:36  dec
 * fixed ident keyword
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
