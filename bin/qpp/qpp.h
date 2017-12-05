#pragma ident "$Id: qpp.h,v 1.2 2015/12/09 18:43:23 dechavez Exp $"
#ifndef qpp_h_defined
#define qpp_h_defined
#include "zlib.h"
#include "isi.h"
#include "qdplus.h"
#include "ida10.h"

#define DEFAULT_TRECS 3000
#define DEFAULT_DETECTOR "enz00,en100,en200 2 3 40 stalta 250 4000 12.0 5.0"
#define DEFAULT_LOG "-"

/* log.c */
void LogMsg(char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, BOOL debug);

/* tee.c */
void TeeHLP(QDP_HLP *hlp);
void CheckCalibrationStatus(UINT64 serialno, QDP_LCQ *lcq);
BOOL InitTee(QDP_HLP_RULES *rules, char *dir, char *detect);
void TeeCallback(void *unused, QDP_HLP *hlp);

#endif /* qpp_h_defined */

/* Revision History
 *
 * $Log: qpp.h,v $
 * Revision 1.2  2015/12/09 18:43:23  dechavez
 * cleaned up prototypes
 *
 * Revision 1.1  2009/07/09 22:32:20  dechavez
 * created
 *
 */
