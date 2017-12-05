#pragma ident "$Id: wfdiscio.h,v 1.7 2014/04/14 18:36:18 dechavez Exp $"
#ifndef cssio_30_wfdiscio_h
#define cssio_30_wfdiscio_h
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

#define CSSIO_STA_LEN      6
#define CSSIO_CHAN_LEN     8
#define CSSIO_TIME_LEN    17
#define CSSIO_WFID_LEN     8
#define CSSIO_CHANID_LEN   8
#define CSSIO_JDATE_LEN    8
#define CSSIO_ENDTIME_LEN 17
#define CSSIO_NSAMP_LEN    8
#define CSSIO_SMPRATE_LEN 11
#define CSSIO_CALIB_LEN   16
#define CSSIO_CALPER_LEN  16
#define CSSIO_INSTYPE_LEN  6
#define CSSIO_SEGTYPE_LEN  1
#define CSSIO_DATATYPE_LEN 2
#define CSSIO_CLIP_LEN     1
#define CSSIO_DIR_LEN     64
#define CSSIO_DFILE_LEN   32
#define CSSIO_FOFF_LEN    10
#define CSSIO_COMMID_LEN   8
#define CSSIO_LDDATE_LEN  17
#define CSSIO_PATH_LEN    (CSSIO_DIR_LEN + CSSIO_DFILE_LEN + 1)

#define CSSIO_WFDISC_SIZE 284

#define CSSIO_FIELD_UNKNOWN  -1
#define CSSIO_FIELD_STA       0
#define CSSIO_FIELD_CHAN      1
#define CSSIO_FIELD_TIME      2
#define CSSIO_FIELD_WFID      3
#define CSSIO_FIELD_CHANID    4
#define CSSIO_FIELD_JDATE     5
#define CSSIO_FIELD_ENDTIME   6
#define CSSIO_FIELD_NSAMP     7
#define CSSIO_FIELD_SMPRATE   8
#define CSSIO_FIELD_CALIB     9
#define CSSIO_FIELD_CALPER   10
#define CSSIO_FIELD_INSTYPE  11
#define CSSIO_FIELD_SEGTYPE  12
#define CSSIO_FIELD_DATATYPE 13
#define CSSIO_FIELD_CLIP     14
#define CSSIO_FIELD_DIR      15
#define CSSIO_FIELD_DFILE    16
#define CSSIO_FIELD_FOFF     17
#define CSSIO_FIELD_COMMID   18
#define CSSIO_FIELD_LDDATE   19
#define CSSIO_FIELD_PATH     20
#define CSSIO_NUMFIELD       21

struct cssio_wfdisc {
/*  1 */   char   sta[CSSIO_STA_LEN+1];
/*  2 */   char   chan[CSSIO_CHAN_LEN+1];
/*  3 */   double time;
/*  4 */   INT32   wfid;
/*  5 */   INT32   chanid;
/*  6 */   INT32   jdate;
/*  7 */   double endtime;
/*  8 */   INT32   nsamp;
/*  9 */   float  smprate;
/* 10 */   float  calib;
/* 11 */   float  calper;
/* 12 */   char   instype[CSSIO_INSTYPE_LEN+1];
/* 13 */   char   segtype;
/* 14 */   char   datatype[CSSIO_DATATYPE_LEN+1];
/* 15 */   char   clip;
/* 16 */   char   dir[CSSIO_DIR_LEN+1];
/* 17 */   char   dfile[CSSIO_DFILE_LEN+1];
/* 18 */   INT32   foff;
/* 19 */   INT32   commid;
/* 20 */   char   lddate[CSSIO_LDDATE_LEN+1];
}; 

#define CSSIO_WFDISC_NFIELD 20

static struct cssio_wfdisc cssio_wfdisc_null = {
/*  1  sta      */ "-",
/*  2  chan     */ "-",
/*  3  time     */ -9999999999.999,
/*  4  wfid     */ -1,
/*  5  chanid   */ -1,
/*  6  jdate    */ -1,
/*  7  endtime  */ -9999999999.999,
/*  8  nsamp    */ -1,
/*  9  smprate  */ -1.0,
/* 10  calib    */  0.0,
/* 11  calper   */ -1.0,
/* 12  instype  */ "-",
/* 13  segtype  */ '-',
/* 14  datatype */ "-",
/* 15  clip     */ '-',
/* 16  dir      */ "-",
/* 17  dfile    */ "-",
/* 18  foff     */ -1,
/* 19  commid   */ -1,
/* 20  lddate   */ "-"
};

typedef struct {
/*  1 */   char sta[CSSIO_STA_LEN+1];
/*  2 */   char chan[CSSIO_CHAN_LEN+1];
/*  3 */   char time[CSSIO_TIME_LEN+1];
/*  4 */   char wfid[CSSIO_WFID_LEN+1];
/*  5 */   char chanid[CSSIO_CHANID_LEN+1];
/*  6 */   char jdate[CSSIO_JDATE_LEN+1];
/*  7 */   char endtime[CSSIO_ENDTIME_LEN+1];
/*  8 */   char nsamp[CSSIO_NSAMP_LEN+1];
/*  9 */   char smprate[CSSIO_SMPRATE_LEN+1];
/* 10 */   char calib[CSSIO_CALIB_LEN+1];
/* 11 */   char calper[CSSIO_CALPER_LEN+1];
/* 12 */   char instype[CSSIO_INSTYPE_LEN+1];
/* 13 */   char segtype[CSSIO_SEGTYPE_LEN+1];
/* 14 */   char datatype[CSSIO_DATATYPE_LEN+1];
/* 15 */   char clip[CSSIO_CLIP_LEN+1];
/* 16 */   char dir[CSSIO_DIR_LEN+1];
/* 17 */   char dfile[CSSIO_DFILE_LEN+1];
/* 18 */   char foff[CSSIO_FOFF_LEN+1];
/* 19 */   char commid[CSSIO_COMMID_LEN+1];
/* 20 */   char lddate[CSSIO_LDDATE_LEN+1];
/* 21 */   char path[CSSIO_PATH_LEN+1];
           char string[CSSIO_WFDISC_SIZE+1];
} WFDISC_C;

#define CSSIO_WFDISC_RCS \
"%-6.6s %-8.8s %17.5lf %8d %8d %8d %17.5lf %8d %11.7f %16.6f %16.6f %-6.6s %c %-2.2s %c %-64.64s %-32.32s %10d %8d %-17.17s"

#define CSSIO_WFDISC_SCS \
"%s %s %lf %d %d %d %lf %d %f %f %f %s %c %s %c %s %s %d %d %s"

#define CSSIO_WFDISC_RVL(SP) \
(SP)->sta, (SP)->chan, &(SP)->time, &(SP)->wfid, &(SP)->chanid, \
&(SP)->jdate, &(SP)->endtime, &(SP)->nsamp, &(SP)->smprate, \
&(SP)->calib, &(SP)->calper, (SP)->instype, &(SP)->segtype, (SP)->datatype, \
&(SP)->clip, (SP)->dir, (SP)->dfile, &(SP)->foff, &(SP)->commid, \
(SP)->lddate

#define CSSIO_WFDISC_WCS \
"%-6.6s %-8.8s %17.5lf %8d %8d %8d %17.5lf %8d %11.7f %16.6f %16.6f %-6.6s %c %-2.2s %c %-64.64s %-32.32s %10d %8d %-17.17s\n"

#define CSSIO_WFDISC_WVL(SP) \
(SP)->sta, (SP)->chan, (SP)->time, (SP)->wfid, (SP)->chanid, \
(SP)->jdate, (SP)->endtime, (SP)->nsamp, (SP)->smprate, \
(SP)->calib, (SP)->calper, (SP)->instype, (SP)->segtype, (SP)->datatype, \
(SP)->clip, (SP)->dir, (SP)->dfile, (SP)->foff, (SP)->commid, \
(SP)->lddate

#define CSSIO_WFDISC_TRM(SP) \
CSSIO_TRIM((SP)->sta,7); CSSIO_TRIM((SP)->chan,9); CSSIO_TRIM((SP)->instype,7); \
CSSIO_TRIM((SP)->datatype,3); CSSIO_TRIM((SP)->dir,65); CSSIO_TRIM((SP)->dfile,33); \

#define CSSIO_WFDISC_FPD(SP) \
CSSIO_FPAD((SP)->sta,7); CSSIO_FPAD((SP)->chan,9); CSSIO_FPAD((SP)->instype,7); \
CSSIO_FPAD((SP)->datatype,3); CSSIO_FPAD((SP)->dir,65); CSSIO_FPAD((SP)->dfile,33); \

#define CSSIO_WFDISC_BFL(SP) \
CSSIO_BFIL((SP)->sta,7); CSSIO_BFIL((SP)->chan,9); CSSIO_BFIL((SP)->instype,7); \
CSSIO_BFIL((SP)->datatype,3); CSSIO_BFIL((SP)->dir,65); CSSIO_BFIL((SP)->dfile,33); \
CSSIO_BFIL((SP)->lddate,18);

#endif /* cssio_30_wfdiscio_h */

/* Revision History
 *
 * $Log: wfdiscio.h,v $
 * Revision 1.7  2014/04/14 18:36:18  dechavez
 * defined WFDISC_C type and CSSIO_FIELD_x macros
 *
 * Revision 1.6  2013/12/02 21:07:56  dechavez
 * changed ld to d in format strings, to avoid -1 printing like 4 billion on 64-bit machines
 *
 * Revision 1.5  2012/02/14 19:47:18  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.4  2011/03/17 17:20:41  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.3  2007/01/07 17:28:43  dechavez
 * used #define constants instead of hard coded string lengths
 *
 * Revision 1.2  2000/06/23 21:19:38  dec
 * fixed ident keyword
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
