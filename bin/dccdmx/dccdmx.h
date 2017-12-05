#pragma ident "$Id: dccdmx.h,v 1.2 2011/02/25 18:54:58 dechavez Exp $"
#ifndef dccdmx_h_included
#define dccdmx_h_included

#include "platform.h"
#include "ida10.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char *VersionIdentString;

/* Module Id's for generating meaningful exit codes */

#define DCCDMX_MOD_MAIN    ((INT32) 100)
#define DCCDMX_MOD_LOG     ((INT32) 200)
#define DCCDMX_MOD_TS      ((INT32) 300)
#define DCCDMX_MOD_LM      ((INT32) 400)
#define DCCDMX_MOD_CA      ((INT32) 500)
#define DCCDMX_MOD_CF      ((INT32) 600)
#define DCCDMX_MOD_II      ((INT32) 700)
#define DCCDMX_MOD_SIGNALS ((INT32) 800)

/* FILE handles */

typedef struct {
    char cname[IDA10_CNAMLEN+1]; /* used only with TS records */
    char path[MAXPATHLEN+1];     /* the file name */
    FILE *fp;                    /* and the handle */
} FILE_HANDLE;

/* packet counters */

typedef struct {
    UINT32 ts;
    UINT32 lm;
    UINT32 cf;
    UINT32 ca;
    UINT32 ii;
    UINT32 ignored;
    UINT32 error;
    UINT32 total;
} COUNTER;

/* function prototypes */

/* main.c */
void ReportCounts(void);
int main(int argc, char **argv);

/* ca.c */
void ProcessCA(UINT8 *buf, INT32 buflen, COUNTER *count);
void CloseCA(void);
void InitCA(char *string);

/* cf.c */
void ProcessCF(UINT8 *buf, INT32 buflen, COUNTER *count);
void CloseCF(void);
void InitCF(char *string);

/* exit.c */
INT32 ExitStatus(void);
void SetExitStatus(INT32 status);
void Exit(INT32 status);
void CheckForShutdown(void);
void InitExit(void);

/* ii.c */
void ProcessIL(UINT8 *buf, INT32 buflen, COUNTER *count);
void CloseIL(void);
void InitIL(char *string);

/* lm.c */
void ProcessLM(UINT8 *buf, INT32 buflen, COUNTER *count);
void CloseLM(void);
void InitLM(char *string);

/* log.c */
void LogMsg(char *format, ...);
void InitLog(char *myname);

/* signals.c */
void StartSignalHandler(void);

/* ts.c */
void ProcessTS(UINT8 *buf, INT32 buflen, COUNTER *count);
void CloseTS(void);
void InitTs(char *string);

#ifdef __cplusplus
}
#endif

#endif /* dccdmx_h_included */

/* Revision History
 *
 * $Log: dccdmx.h,v $
 * Revision 1.2  2011/02/25 18:54:58  dechavez
 * initial release
 *
 */
