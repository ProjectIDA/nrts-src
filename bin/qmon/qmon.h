#pragma ident "$Id: qmon.h,v 1.17 2017/09/20 19:29:01 dauerbach Exp $"
#ifndef qmon_h_included
#define qmon_h_included
#include <curses.h>
#include "platform.h"
#include "stdtypes.h"
#include "qdp.h"
#include "q330.h"
#include "isi/dl.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2010 - Regents of the University of California.";

/* Module ident's for exit codes */

#define QMON_MOD_MAIN  0

/* constants */

#define QMON_WIN_BOUNDARY_CHAR '='
#define QMON_WIN_SP2_DASH  '-'
#define QMON_WIN_SP2_BLANK ' '

#define QMON_LST_ROWS 2
#define QMON_SP1_ROWS 1
#define QMON_TOP_ROWS 9
#define QMON_SP2_ROWS 1
#define QMON_BOT_ROWS 9
#define QMON_SP3_ROWS 1
#define QMON_CMD_ROWS 1
#define QMON_ISI_ROWS (QMON_LST_ROWS + QMON_SP1_ROWS + QMON_TOP_ROWS + QMON_SP2_ROWS + QMON_BOT_ROWS)

#define QMON_COLS     80
#define QMON_LST_COLS QMON_COLS
#define QMON_SP1_COLS QMON_COLS
#define QMON_TOP_COLS QMON_COLS
#define QMON_SP2_COLS QMON_COLS
#define QMON_BOT_COLS QMON_COLS
#define QMON_SP3_COLS QMON_COLS
#define QMON_CMD_COLS QMON_COLS
#define QMON_ISI_COLS QMON_COLS

#define QMON_NRTS_ROWS QMON_ISI_ROWS
#define QMON_NRTS_COLS QMON_ISI_COLS

#define QMON_MAX_NAME_LEN 6

#define QMON_ROWS ( \
    QMON_LST_ROWS + \
    QMON_SP1_ROWS + \
    QMON_TOP_ROWS + \
    QMON_SP2_ROWS + \
    QMON_BOT_ROWS + \
    QMON_SP3_ROWS + \
    QMON_CMD_ROWS   \
)

#define QMON_DIM (QMON_ROWS * QMON_COLS)

#define DEFAULT_LOOP_INTERVAL 1
#define DEFAULT_MAX_IDLE_TIME 600

#define QMON_NWIN 2

/* data types */

typedef struct {
   WINDOW *lst; /* list of digitizers */
   WINDOW *sp1; /* separator */
   WINDOW *top; /* top status display window */
   WINDOW *sp2; /* separator */
   WINDOW *bot; /* bottom status display window */
   WINDOW *sp3; /* separator */
   WINDOW *cmd; /* command window */
   WINDOW *isi; /* ISI/NRTS status window */
} QMON_WINDOW;

#define QMON_STATE_UNCONNECTED 0
#define QMON_STATE_CONNECTING  1
#define QMON_STATE_CONNECTED   2
#define QMON_STATE_BUSY        3
#define QMON_STATE_REGERR      4
#define QMON_STATE_SHUTDOWN    5
#define QMON_STATE_REBOOT      6
#define QMON_STATE_BADSERIALNO 7

/* Command and control stuff */

#define Q330_CMD_ERROR      -1
#define Q330_CMD_UNDEFINED   0
#define Q330_CMD_SAVE        1
#define Q330_CMD_BOOT        2
#define Q330_CMD_SAVEBOOT    3
#define Q330_CMD_RESYNC      4
#define Q330_CMD_GPS         5
#define Q330_CMD_GPS_ON      6
#define Q330_CMD_GPS_OFF     7
#define Q330_CMD_GPS_COLD    8
#define Q330_CMD_GPS_CNF     9
#define Q330_CMD_GPS_ID     10
#define Q330_CMD_GPS_SV     11
#define Q330_CMD_GPS_INT    12
#define Q330_CMD_GPS_EXT    13
#define Q330_CMD_SC         14
#define Q330_CMD_CALIB      15
#define Q330_CMD_CAL_START  16
#define Q330_CMD_CAL_STOP   17
#define Q330_CMD_CAL_LSCUT  18
#define Q330_CMD_PULSE      19
#define Q330_CMD_FLUSH      20
#define Q330_CMD_INBITMAP   21
#define Q330_CMD_LIMIT      22
#define Q330_CMD_PREAMP     23

typedef struct {
    int code; /* one of Q33_CMD_x, above */
    char *command; /* command the user should type */
    char *description; /* command description */
} COMMAND_MAP;

extern COMMAND_MAP *CommandMap;

typedef struct {
    int code;
    int argc;
    char **argv;
} COMMAND;

/* handle for a single digitizer */

typedef struct {
    Q330_ADDR *addr;        /* digitizer coordinates */
    QDP_TYPE_MONITOR mon;   /* status information */
    int state;              /* bit mask of QMON_STATE_x flags */
    BOOL fresh;             /* TRUE when mon had been updated but not displayed */
    MUTEX mutex;            /* protects mon, state, and fresh */
    int debug;              /* QDP debug flag */
    int interval;           /* update interval, sec */
    SEMAPHORE sem;          /* for synchronization */
    WINDOW *win[QMON_NWIN]; /* display windows, if selected */
    BOOL clrbot;            /* TRUE when bottom window needs a clear */
#define REBOOT_REQUIRED    0x01
#define UNSAVED_CHANGES    0x02
#define REBOOT_IN_PROGRESS 0x04
    int flags;              /* state flags */
} DIGITIZER;

/* general I/O handle */

#define QMON_MODE_Q330 0
#define QMON_MODE_ISI  1
#define QMON_MODE_NRTS 2

typedef struct {
    Q330_CFG *cfg;           /* q330.cfg configuration data */
    BOOL newcfg;             /* TRUE when changes to cfg have been made */
    LNKLST *avail;           /* list of available DIGITIZERs */
    QMON_WINDOW win;         /* display screens */
    int debug;               /* QDP debug flag */
    int interval;            /* update interval, sec */
    char sp2char;            /* current top/bot window separator character */
    int cmdrow;              /* absolute location of command window */
    COMMAND cmd;             /* command */
    DIGITIZER *active;       /* currently selected digitizer for command and control */
    QDP *qdp;                /* config port I/O handle to active digitizer */
    char poptxt[QMON_DIM+1]; /* to hold popup text */
    BOOL PopDebugMessage;    /* for debugging convenience */
    int mode;                /* QMON_MODE_x */
    ISI_MON *mon;            /* for ISI and NRTS disk loop status */
} QMON;

/* canned popup messages */

#define QMON_MSG_NOT_SINGLE             \
"Q330 commands are allowed only when\n" \
"display is in single digitizer mode\n" \
"      (use TAB to toggle)          \n"

#define QMON_MSG_NOT_IMPLEMENTED \
"unimplemented command\n"

#define QMON_MSG_BUSY                                               \
"               configuration port already in use               \n" \
"You wouldn't have a Willard or q330 session running, would you?\n"

/* Function prototypes */

/* main.c */
BOOL SingleActiveDigitizer(QMON *qmon);
int main (int argc, char **argv);

/* boot.c */
void ExecuteBoot(QMON *qmon, int argc, char **argv);

/* calib.c */
void ExecuteCalib(QMON *qmon, int argc, char **argv);

/* cmd.c */
void InitCommandMap(void);
void Q330Command(QMON *qmon, int argc, char **argv);

/* debug.c */
void debug(char *format, ...);
void InitDebug(char *myname, char *path);
LOGIO *dbglp(void);

/* display.c */
void DrawSeparator(WINDOW *win, char value, int count);
void ClearToEol(WINDOW *win, int row, int col);
void PrintHostTime(WINDOW *win, int row);
void ShowVersionNumber(WINDOW *win);
void UpdateStatus(QMON *qmon);
void RefreshDisplay(QMON *qmon);
BOOL InitWindows(QMON *qmon);

/* exit.c */
void SetShutdownFlag(void);
BOOL ShutdownInProgress(void);
void GracefulExit(QMON *qmon, int status);

/* flush.c */
void ExecuteFlush(QMON *qmon, int argc, char **argv);

/* gps.c */
void ExecuteGps(QMON *qmon, int argc, char **argv);

/* inbitmap.c */
void ExecuteInputBitmap(QMON *qmon, int argc, char **argv);

/* input.c */
void SetCursorPosition(QMON *qmon);
void DrawCmdWindow(QMON *qmon);
void DisplayShutdownMessage(QMON *qmon);
void ClearCmdWindow(QMON *qmon);
void SetMaxIdle(time_t value);
char *GetUserInput(QMON *qmon);

/* isi.c */
void UpdateISI(QMON *qmon);
void SetISIdisplay(QMON *qmon);

/* limit.c */
void ExecuteLimit(QMON *qmon, int argc, char **argv);

/* list.c */
void UpdateHts(QMON *qmon);
void DrawDigitizerList(QMON *qmon);

/* nrts.c */
void UpdateNRTS(QMON *qmon);
void SetNRTSdisplay(QMON *qmon);

/* pop.c */
void PopupMessage(QMON *qmon, char *string);
void PopupQ330Help(QMON *qmon);
void PopupHelp(QMON *qmon, int argc, char **argv);
void PopupUnrecognized(QMON *qmon, char *command);
void PopupRegerr(QMON *qmon, int errcode);
BOOL ActivePopup(void);
void ClearPopup(void);

/* preamp.c */
void ExecutePreamp(QMON *qmon, int argc, char **argv);


/* process.c */
void ProcessCommand(QMON *qmon, char *cmdstr);

/* pulse.c */
void ExecutePulse(QMON *qmon, int argc, char **argv);

/* q330.c */
void TogglePercentResentView(void);
void ToggleDecimalDegreesView(void);
void UpdateQ330(QMON *qmon);

/* qio.c */
BOOL isActiveDigitizer(DIGITIZER *digitizer);
BOOL GetMonData(DIGITIZER *digitizer, QDP_TYPE_MONITOR *mon);
void Activate(QMON *qmon, DIGITIZER *top, DIGITIZER *bot);
BOOL UpdateDigitizerSelection(QMON *qmon, char *top, char *bot);
void ShiftSelectionRight(QMON *qmon);
void ShiftSelectionLeft(QMON *qmon);
BOOL AddDigitizer(QMON *qmon, char *nxtname);
void AddRemDigitizer(QMON *qmon);
BOOL StartDigitizerThread(QMON *qmon, Q330_ADDR *addr);

/* resync.c */
void ExecuteResync(QMON *qmon, int argc, char **argv);

/* save.c */
void ExecuteSave(QMON *qmon, int argc, char **argv);

/* sc.c */
BOOL SensorControlMismatch(DIGITIZER *digitizer, UINT32 *actual);
void ExecuteSc(QMON *qmon, int argc, char **argv);

/* signals.c */
void StartSignalHandler(void);

#endif /* qmon_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/


/* Revision History
 *
 * $Log: qmon.h,v $
 * Revision 1.17  2017/09/20 19:29:01  dauerbach
 * added preamp support
 *
 * Revision 1.16  2017/09/06 22:05:18  dechavez
 * added ToggleDecimalDegreesView() prototype
 *
 * Revision 1.15  2015/12/23 22:43:07  dechavez
 * defined QMON_STATE_BADSERIALNO, changed QMON "mon" field to a pointer to
 * make it easy to see if there are any disk loops available to display
 *
 * Revision 1.14  2013/01/24 22:21:30  dechavez
 * added TogglePercentResentView() prototype
 *
 * Revision 1.13  2011/07/27 23:48:21  dechavez
 * added ISI_MON and display mode to QMON handle, plus all prototype changes to support these new features
 *
 * Revision 1.12  2011/01/07 22:58:24  dechavez
 * added Q330_CMD_LIMIT
 *
 * Revision 1.11  2011/01/03 21:32:10  dechavez
 * added 'input' command support
 *
 * Revision 1.10  2010/12/27 22:43:41  dechavez
 * updated prototypes
 *
 * Revision 1.9  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.8  2010/12/13 22:43:08  dechavez
 * initial Q330 command support
 *
 * Revision 1.7  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.6  2010/12/08 19:16:28  dechavez
 * updated prototypes
 *
 * Revision 1.5  2010/10/25 16:10:15  dechavez
 * added prototypes
 *
 * Revision 1.4  2010/10/22 22:56:51  dechavez
 * added new prototypes
 *
 * Revision 1.3  2010/10/20 18:36:54  dechavez
 * cleaned up prototypes, use QMON_NWIN instead of hardcoded 2
 *
 * Revision 1.2  2010/10/19 21:08:15  dechavez
 * reordered includes to clean up Solaris 9 build
 *
 * Revision 1.1  2010/10/19 21:01:52  dechavez
 * initial release
 *
 */
