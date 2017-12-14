/*======================================================================
 *
 *  Aquire data from Q330 using Quanterra Lib330 library
 *
 *=+===================================================================*/
#ifndef isi330_h_included
#define isi330_h_included

#include "libtypes.h"
#include "libclient.h"
#include "libmsgs.h"
#undef SOCKET_ERROR
#undef TRUE
#undef FALSE

#include "util.h"
#include "logio.h"
#include "q330.h"
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2017 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define ISI330_MOD_MAIN      ((INT32)  100)
#define ISI330_MOD_INIT      ((INT32)  200)
#define ISI330_MOD_LOG       ((INT32)  300)
#define ISI330_MOD_SIGNALS   ((INT32)  400)
#define ISI330_MOD_Q330      ((INT32)  500)

#define ISI330_DEFAULT_USER          "nrts"

#define ISI330_STATION_CODE_SIZE 4

#define DEFAULT_USER           "nrts"
#define DEFAULT_DAEMON         FALSE
#define DEFAULT_SOURCE         SOURCE_UNDEFINED
#define DEFAULT_HOME           "/usr/nrts"
#define DEFAULT_TIMEOUT        30
#define DEFAULT_BACKGROUND_LOG "syslogd:local0"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DEBUG          FALSE

typedef struct {
    MUTEX mutex;
	char *host;
    LOGIO *lp;
	Q330_CFG *q330cfg;
	UINT16 dp;
	tpar_create tpc;
	tpar_register tpr;
	tcontext *ct;
    BOOL first;
    UINT64 sn;
    UINT64 authcode;
    UINT32 retry;     /* registration retry interval */
    UINT32 watchdog;  /* maximum msecs of no data before restarting connection */
    int debug;
} Q330;


typedef struct {
	char *site;		// Disk Loop Name
    ISI_DL *dl;     //ptr to disk loop struct
	char *cfgpath;      /* Q330 configuration file */
	LNKLST q330list;    /* zero or more Q330's */
	LOGIO *lp;
} ISI330_CONFIG;

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;


/* prototypes */

/* bground.c */
BOOL BackGround(ISI330_CONFIG *cfg);

/* callbacks.c */
void isi330_state_callback(pointer p);
void isi330_msg_callback(pointer p);
void isi330_miniseed_callback(pointer p);

/* exit.c */
void GracefulExit(INT32 status);
void InitExit(ISI330_CONFIG *cfg);
INT32 ExitStatus();
void SetExitStatus(INT32 status);

/* init.c */
void help(char *myname);
ISI330_CONFIG *init(char *myname, int argc, char **argv);

/* log.c */
void LogCommandLine(int argc, char **argv);
VOID LogMsg(int level, char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);
void PrintISI330Config(ISI330_CONFIG *cfg);
void PrintLib330Tliberr(enum tliberr);
void PrintLib330Topstat(topstat *popstat);

/* q330.c */
void ToggleQ330DebugState(void);
void SaveQ330Packet(void *args, QDP_PKT *pkt);
void SaveQ330Meta(void *args, QDP_META *meta);
char *AddQ330(ISI330_CONFIG *cfg, char *argstr, char *root);
void StartQ330Readers(ISI330_CONFIG *cfg);
void ShutdownQ330Readers(ISI330_CONFIG *cfg);

/* signals.c */
VOID StartSignalHandler(VOID);
void SignalHandler(int sig);


#endif
