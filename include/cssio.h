#pragma ident "$Id: cssio.h,v 1.13 2015/10/02 15:31:14 dechavez Exp $"
/*======================================================================
 *
 *  Include file for cssio library routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef cssio_h_included
#define cssio_h_included

#include <stdio.h>
#include "css/2.8/wfdiscio.h"
#include "css/3.0/wfdiscio.h"
#ifdef INCLUDE_SACIO
#include "sacio.h"
#endif /* INCLUDE_SACIO */
#include "platform.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSS_28 0
#define CSS_30 1

typedef struct cssio_wfdisc WFDISC;

#define CSS30_DATABASE_ENVSTR "CSS30_DATABASE_PATH"

/* datatype enums */

#define CSSIO_DATATYPE_UNKNOWN 0x00000000
#define CSSIO_DATATYPE_S2      0x00000001
#define CSSIO_DATATYPE_I2      0x00000002
#define CSSIO_DATATYPE_S4      0x00000004
#define CSSIO_DATATYPE_I4      0x00000008
#define CSSIO_DATATYPE_S8      0x00000010
#define CSSIO_DATATYPE_I8      0x00000020
#define CSSIO_DATATYPE_T4      0x00000040
#define CSSIO_DATATYPE_F4      0x00000080
#define CSSIO_DATATYPE_F8      0x00000100
#define CSSIO_DATATYPE_T8      0x00000200
#define CSSIO_DATATYPE_TEXT    0x10000000

/*  cssio library function prototypes  */

#ifdef INCLUDE_SACIO
/* css2sac.c */
int css2sac(struct cssio_wfdisc *, INT32, int, struct sac_header *, int);
#endif /* INCLUDE_SACIO */

/* datatype.c */
char *cssio_datatype(int, UINT32, int);
int cssio_samplesize(char *datatype);

/* desc.c */
BOOL cssioWriteDescriptor(char *base, char *prefix);

/* jdate.c */
INT32 cssio_jdate(double dtime);

/* readata.c */
BOOL cssioReadDataIntoReal64(WFDISC *wd, REAL64 *dest);

/* rwfdisc.c */
INT32 rwfdisc(FILE *, struct cssio_wfdisc **);

/* rwfdrec.c */
int rwfdrec(FILE *, WFDISC *);
#define CSSIO_RETAIN_CASE 0
#define CSSIO_UCASE       1
#define CSSIO_LCASE       2
int cssioReadWfdiscC(FILE *, WFDISC_C *, int flags);

/* string.c */
char *cssioFieldString(int code);
int cssioFieldCode(char *string);
BOOL cssioFieldMatch(WFDISC_C *wd, int field, char *value);
char *cssioDatatypeString(int code);
int cssioDatatypeCode(char *string);
char *cssioDatatypeListString(char *delimiter, char *buf);

/* togse.c */
BOOL CssToGse(FILE *ofp, struct cssio_wfdisc *wfdisc);

/* version.c */
char *cssioVersionString(VOID);
VERSION *cssioVersion(VOID);

/* wdcut.c */
struct cssio_wfdisc *wdcut(struct cssio_wfdisc *input, double beg, double end);

/* wf28to30.c */
void wf28to30(struct cssio_wfdisc *wfdisc30, struct cssio_wfdisc28 *wfdisc28);

/* wf30to28.c */
void wf30to28(struct cssio_wfdisc28 *wfdisc28, struct cssio_wfdisc *wfdisc30);

/* wrdsize.c */
int cssio_wrdsize(char *datatype);

/* wwfdisc.c */
char *cssioWfdiscString(WFDISC *wfdisc, char *buf);
char *wdtoa(struct cssio_wfdisc *wfdisc);
int wwfdisc(FILE *fp, struct cssio_wfdisc *wfdisc);
int wwfdisc28(FILE *fp, struct cssio_wfdisc28 *wfdisc28);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: cssio.h,v $
 * Revision 1.13  2015/10/02 15:31:14  dechavez
 * added cssio_samplesize() prototype
 *
 * Revision 1.12  2014/04/18 18:25:34  dechavez
 * added CSSIO_DATATYPE_x enums, updated prototypes
 *
 * Revision 1.11  2014/04/15 19:30:57  dechavez
 * updated prototypes
 *
 * Revision 1.10  2012/02/14 21:45:15  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.9  2012/02/14 19:51:19  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.8  2011/03/17 17:20:36  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.7  2005/10/11 22:41:09  dechavez
 * updated prototypes, defined CSS30_DATABASE_ENVSTR
 *
 * Revision 1.6  2004/06/25 18:34:56  dechavez
 * C++ compatibility
 *
 * Revision 1.5  2003/12/10 05:35:25  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.4  2003/11/13 19:28:28  dechavez
 * wrap SAC prototypes with INCLUDE_SACIO conditional
 *
 * Revision 1.3  2003/10/16 17:50:52  dechavez
 * updated prototypes
 *
 * Revision 1.2  2001/12/20 18:13:18  dec
 * cleaned up prototypes
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
