/*======================================================================
 *
 *  Aquire data from Q330 using Quanterra Lib330 library
 *
 *=+===================================================================*/
#ifndef isi330_h_included
#define isi330_h_included

#include "/ida/dev/src/lib/3rdparty/lib330/libclient.h"
#include "/ida/dev/src/lib/3rdparty/lib330/libmsgs.h"
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
	UINT16 dp;
	tpar_create *tpc;
	tpar_register *tpr;
} ISI330_CONFIG;


/* prototypes */

/* callbacks.c */
void isi330_state_callback(pointer p);

/* exit.c */
void Exit(INT32 status);
void InitExit(void);

/* init.c */
void help(char *myname);
ISI330_CONFIG *init(char *myname, int argc, char **argv);

/* log.c */
void LogCommandLine(int argc, char **argv);
void LogMsg(char *format, ...);
void LogMsgLevel(int newlevel);
LOGIO *InitLogging(char *myname, char *path, char *SITE);
void PrintISI330Config(ISI330_CONFIG *cfg);

/* q330db.c */
int q330db_get_sn(UINT64 *snbuf);
int q330db_get_dp(UINT16 *dp);
int q330db_get_station_code(char *stabuf);

/* signals.c */
VOID StartSignalHandler(VOID);
void SignalHandler(int sig);


#endif
