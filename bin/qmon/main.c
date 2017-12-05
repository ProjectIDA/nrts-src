#pragma ident "$Id: main.c,v 1.17 2016/01/21 21:22:02 dechavez Exp $"
/*======================================================================
 *
 *  Monitor, and control, multiple Q330 digitizers
 *
 *====================================================================*/
#include "qmon.h"

#define MY_MOD_ID QMON_MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ digitizer [ digitizer ] root=path debug=0|1|2 log=path ]\n", myname);
    exit(1);
}

static BOOL InitQmon(char *myname, QMON *qmon)
{
static ISI_MON mon;

    qmon->cfg    = NULL;
    qmon->newcfg = FALSE;
    if ((qmon->avail = listCreate()) == NULL) {
        perror("listCreate");
        return FALSE;
    }
    qmon->win.lst  = NULL;
    qmon->win.sp1  = NULL;
    qmon->win.top  = NULL;
    qmon->win.sp2  = NULL;
    qmon->win.bot  = NULL;
    qmon->win.sp3  = NULL;
    qmon->win.cmd  = NULL;
    qmon->debug    = QDP_TERSE;
    qmon->interval = DEFAULT_LOOP_INTERVAL;
    qmon->sp2char  = QMON_WIN_SP2_BLANK;
    qmon->PopDebugMessage = FALSE;

    if (isidlInitMON(myname, &mon, NULL, NULL)) {
        qmon->mon = &mon;
    } else {
        qmon->mon = NULL;
    }

    return TRUE;
}

static BOOL ActivateInitialSystems(QMON *qmon, char **start)
{
int i;
LNKLST_NODE *crnt;
DIGITIZER *this, *digitizer[QMON_NWIN];

/* First look to see if specific digitizer(s) specified */

    for (i = 0; i < QMON_NWIN; i++) {
        digitizer[i] = NULL;
        if (start[i] != NULL) {
            crnt = listFirstNode(qmon->avail);
            while (crnt != NULL && digitizer[i] == NULL) {
                this = (DIGITIZER *) crnt->payload;
                if (strcmp(this->addr->connect.ident, start[i]) == 0) digitizer[i] = this;
                crnt = listNextNode(crnt);
            }
            if (digitizer[i] == NULL) {
                endwin();
                printf("ERROR: '%s' is not a valid digitizer name\n", start[i]);
                return FALSE;
            }
        }
    }

    for (i = 0; i < QMON_NWIN; i++) if (start[i] != NULL && digitizer[i] == NULL) return FALSE;

/* If nothing specified, just grap the first digitizer in the list */

    if (digitizer[0] == NULL) {
        if ((crnt = listFirstNode(qmon->avail)) == NULL) {
            endwin();
            printf("no Q330s specified in the configuration file\n");
            return FALSE;
        }
        digitizer[0] = (DIGITIZER *) crnt->payload;
        digitizer[1] = NULL;
    }

/* Activate the digitizer(s) */

    Activate(qmon, digitizer[0], digitizer[1]);
    return TRUE;
}

static BOOL NoRoomInStartArray(char **start, char *name)
{
int i;

    for (i = 0; i < QMON_NWIN; i++) {
        if (start[i] == NULL) {
            start[i] = name;
            return FALSE;
        }
    }
    return TRUE;
}

BOOL SingleActiveDigitizer(QMON *qmon)
{
LNKLST_NODE *crnt;
DIGITIZER *digitizer;
Q330_ADDR *addr = NULL;

    qmon->active = NULL;
    crnt = listFirstNode(qmon->avail);
    while (crnt != NULL) {
        digitizer = (DIGITIZER *) crnt->payload;
        if (isActiveDigitizer(digitizer)) {
            if (qmon->active == NULL) {
                qmon->active = digitizer;
            } else {
                return FALSE;
            }
        }
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

int main(int argc, char **argv)
{
int i, errcode, errors;
QMON qmon;
char *cmd;
Q330_ADDR *addr;
char *log = NULL;
char *start[QMON_NWIN] = {NULL, NULL};
LNKLST_NODE *crnt;
DIGITIZER *digitizer;
char *root = NULL;

    if (!InitQmon(argv[0], &qmon)) exit(1);
    InitCommandMap();

    for (i = 0; i < QMON_NWIN; i++) start[i] = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "root=", strlen("root=")) == 0) {
            root = argv[i] + strlen("root=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "debug=0") == 0) {
            qmon.debug = QDP_TERSE;
        } else if (strcmp(argv[i], "debug=1") == 0) {
            qmon.debug = QDP_INFO;
        } else if (strcmp(argv[i], "debug=2") == 0) {
            qmon.debug = QDP_DEBUG;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            printf("illegal debug option\n");
            help(argv[0]);
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (NoRoomInStartArray(start, argv[i])) {
            help(argv[0]);
        }
    }

    if (qmon.debug != QDP_TERSE && log == NULL) {
        printf("the debug option requires that you also specify the log file\n");
        help(argv[0]);
    }
    InitDebug(argv[0], log);

/* Read the config file */

    if ((qmon.cfg = q330ReadCfg(root, &errcode)) == NULL) {
        q330PrintErrcode(stdout, "q330ReadCfg: ", root, errcode);
        exit(1);
    }

    for (errors = 0, i = 0; i < QMON_NWIN; i++) {
        if (start[i] != NULL && q330LookupAddr(start[i], qmon.cfg) == NULL) {
            printf("ERROR: %s is not a valid digitizer identifier\n", start[i]);
            ++errors;
        }
    }
    if (errors) exit(1);

/* Start signal handler thread */

    StartSignalHandler();

/* Launch I/0 threads to all Q330s in the configuration file */
/* Console is included if user specified it on the command line */

    crnt = listFirstNode(qmon.cfg->addr);
    while (crnt != NULL) {
        addr = (Q330_ADDR *) crnt->payload;
        if (strlen(addr->connect.ident) > 0 && !StartDigitizerThread(&qmon, addr)) exit(1);
        crnt = listNextNode(crnt);
    }
    if (!listSetArrayView(qmon.avail)) {
        perror("listSetArrayView");
        exit(1);
    }

/* Set initial system(s) to display */

    if (!InitWindows(&qmon)) exit(1);
    if (!ActivateInitialSystems(&qmon, start)) exit(1);

/* Lay on the display */

    RefreshDisplay(&qmon);

/* Process user commands until we are done */

   while ((cmd = GetUserInput(&qmon)) != NULL) ProcessCommand(&qmon, cmd);

   GracefulExit(&qmon, 0);

}

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
 * $Log: main.c,v $
 * Revision 1.17  2016/01/21 21:22:02  dechavez
 * fixed dumb (but benign) unused relational comparison result caught by OS X compiles
 *
 * Revision 1.16  2015/12/23 22:55:17  dechavez
 * InitQmon() no longer fails if isidlInitMON() does.  Instead it sets qmon->mon
 * to NULL to indicate to other code that no disk loops are available.
 * Restored the "test all start digitizer entries via q330LookupAddr()" fix
 * added in 1.12 that somehow got lost in subsequent revs and breaking console support.
 *
 * Revision 1.15  2014/09/08 18:37:17  dechavez
 * Print error message and fail when configuration is incomplete (ie, no internal serial number found)
 *
 * Revision 1.14  2014/08/11 19:13:30  dechavez
 * added -h option (for help)
 *
 * Revision 1.13  2011/07/27 23:48:42  dechavez
 * added ISI_MON support
 *
 * Revision 1.12  2011/02/01 20:21:54  dechavez
 * test all start digitizer entries via q330LookupAddr(), thus ensuring that
 * a serial device entry will enable to console connection (libq330 3.0.0)
 *
 * Revision 1.11  2011/01/25 18:53:32  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.10  2011/01/07 23:02:10  dechavez
 * Improved behavior when invoked with no entries in q330.cfg file
 *
 * Revision 1.9  2010/12/21 20:15:00  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 *
 * Revision 1.8  2010/12/17 19:53:09  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.7  2010/12/13 22:45:34  dechavez
 * initial Q330 command support
 *
 * Revision 1.6  2010/12/08 22:54:59  dechavez
 * checkpoint commit, adding command and control
 *
 * Revision 1.5  2010/12/08 19:17:45  dechavez
 * addded SingleActiveDigitizer
 *
 * Revision 1.4  2010/11/24 19:29:20  dechavez
 * slightly more helpful error message when configuration file fails to load
 *
 * Revision 1.3  2010/10/22 22:59:04  dechavez
 * set array view of digitizer list to make wrap-around walks easier
 *
 * Revision 1.2  2010/10/20 18:39:16  dechavez
 * allow user to specify initial digitizer(s) to display on command line,
 * and to require name of debug log file to turn on debug output
 *
 * Revision 1.1  2010/10/19 18:39:32  dechavez
 * initial release
 *
 */
