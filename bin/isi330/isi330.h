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

extern char *VersionIdentString;

/* Module Id's for generating meaningful exit codes */

#define ISI330_MOD_MAIN      ((INT32)  100)
#define ISI330_MOD_INIT      ((INT32)  200)
#define ISI330_MOD_LOG       ((INT32)  300)
#define ISI330_MOD_SIGNALS   ((INT32)  400)

#define ISI330_DEFAULT_USER          "nrts"

#define ISI330_STATION_CODE_SIZE 4

typedef struct {
	char *stacode;
	char *q330host;
    char sn_str[16+1];
    char *sn_str_hi; /* ptr to high order 8 chars. same as sn_str */
    char *sn_str_lo; /* ptr to low order 8 chars */
    longword sn_hi; /* ptr to high order 8 chars */
    longword sn_lo; /* ptr to low order 8 chars */
    UINT64 sn;
	UINT16 dp;
	tpar_create *tpc;
	tpar_register tpr;
    tcontext *ct;
} ISI330_CONFIG;


/* prototypes */

/* callbacks.c */
void isi330_state_callback(pointer p);
void isi330_msg_callback(pointer p);
void isi330_miniseed_callback(pointer p);

/* exit.c */
void GracefulExit(INT32 status);
void InitExit(ISI330_CONFIG *);
INT32 ExitStatus();
void SetExitStatus(INT32 status);

/* init.c */
void help(char *myname);
ISI330_CONFIG *init(char *myname, int argc, char **argv);

/* log.c */
void LogCommandLine(int argc, char **argv);
void LogMsg(char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, char *SITE);
void PrintISI330Config(ISI330_CONFIG *cfg);
void PrintLib330Tliberr(enum tliberr);
void PrintLib330Topstat(topstat *popstat);

/* q330db.c */
int q330db_get_sn(UINT64 *snbuf);
int q330db_get_dp(UINT16 *dp);
int q330db_get_station_code(char *stabuf);

/* q330utils.c */
void disconnect_q330(tcontext *ct);

/* signals.c */
VOID StartSignalHandler(VOID);
void SignalHandler(int sig);


#endif
