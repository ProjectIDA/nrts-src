/*======================================================================
 *
 *  Aquire data from Q330 using Quanterra Lib330 library
 *
 *=+===================================================================*/
#ifndef isi330_h_included
#define isi330_h_included

<<<<<<< HEAD
#include "/ida/dev/src/lib/3rdparty/lib330/libclient.h"
#include "/ida/dev/src/lib/3rdparty/lib330/libmsgs.h"
=======
#include "libtypes.h"
#include "libclient.h"
#include "libmsgs.h"
>>>>>>> 99d0283f02e1e325503df43615c1c0f320a9482e
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
<<<<<<< HEAD
	UINT16 dp;
	tpar_create *tpc;
	tpar_register *tpr;
=======
    char sn_str[16+1];
    char *sn_str_hi; /* ptr to high order 8 chars. same as sn_str */
    char *sn_str_lo; /* ptr to low order 8 chars */
    longword sn_hi; /* ptr to high order 8 chars */
    longword sn_lo; /* ptr to low order 8 chars */
    UINT64 sn;
	UINT16 dp;
	tpar_create *tpc;
	tpar_register tpr;
>>>>>>> 99d0283f02e1e325503df43615c1c0f320a9482e
} ISI330_CONFIG;


/* prototypes */

/* callbacks.c */
void isi330_state_callback(pointer p);
<<<<<<< HEAD

/* exit.c */
void Exit(INT32 status);
=======
void isi330_msg_callback(pointer p);
void isi330_miniseed_callback(pointer p);

/* exit.c */
void GracefulExit(INT32 status);
>>>>>>> 99d0283f02e1e325503df43615c1c0f320a9482e
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
<<<<<<< HEAD
=======
void PrintLib330Tliberr(enum tliberr);
void PrintLib330Topstat(topstat *popstat);
>>>>>>> 99d0283f02e1e325503df43615c1c0f320a9482e

/* q330db.c */
int q330db_get_sn(UINT64 *snbuf);
int q330db_get_dp(UINT16 *dp);
int q330db_get_station_code(char *stabuf);

/* signals.c */
VOID StartSignalHandler(VOID);
void SignalHandler(int sig);


#endif
