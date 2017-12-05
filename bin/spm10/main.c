#pragma ident "$Id: main.c,v 1.4 2016/11/16 22:00:11 dechavez Exp $"
/*======================================================================
 *
 *  Generate IDA10 packets of SPM data, using the PollSPM script as the
 *  acquisition engine
 *
 *====================================================================*/
#include "spm10.h"

#define MY_MOD_ID SPM10_MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr, "usage: %s sta=name [ peer=name log=name user=name net=name dl=server:port -bd ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments:\n");
    fprintf(stderr, "sta=name               - set station name\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "peer=name              - SPM device name or dot decimal IP address\n");
    fprintf(stderr, "log=name               - set log file name\n");
    fprintf(stderr, "user=name              - run as the specified user\n");
    fprintf(stderr, "net=name               - set network code\n");
    fprintf(stderr, "dl=server:port[:depth] - write to remote disk loop\n");
    fprintf(stderr, "-bd                    - run in the background\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Defaults: peer=%s user=%s net=%s log=%s (foreground) or log=%s (background)\n",
        DEFAULT_PEER, DEFAULT_USER, DEFAULT_NET, DEFAULT_FOREGROUND_LOG, DEFAULT_BACKGROUND_LOG
    );
    exit(1);
}

#define COMMENT '#'
#define MAXLINELEN 1024

int main(int argc, char **argv)
{
FILE *fp;
LOGIO *lp;
int i, errors;
BOOL daemon = FALSE;
BOOL debug  = FALSE;
BOOL lograw = DEFAULT_LOGRAW;
INT32 options = DEFAULT_OPTIONS;
char command[MAXPATHLEN+1], input[MAXLINELEN+1];
static char *dl   = NULL;
static char *sta  = NULL;
static char *log  = NULL;
static char *net  = DEFAULT_NET;
static char *user = DEFAULT_USER;
static char *peer = DEFAULT_PEER;
static char *fid = "main";

    for (i = 1, errors = 0; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strncasecmp(argv[i], "dl=", strlen("dl=")) == 0) {
            dl = argv[i] + strlen("dl=");
        } else if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta = argv[i] + strlen("sta=");
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            net = argv[i] + strlen("net=");
        } else if (strncmp(argv[i], "peer=", strlen("peer=")) == 0) {
            peer = argv[i] + strlen("peer=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strcmp(argv[i], "-lograw") == 0) {
            lograw = TRUE;
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            ++errors;
        }
    }
    if (errors) help(argv[0]);

    if (sta == NULL) {
        fprintf(stderr, "%s: ERROR: must specify station name using sta=name option\n", argv[0]);
        help(argv[0]);
    }

/* Run as the specified user */

    utilSetIdentity(user);

/* Go into the background, if applicable */

    if (daemon && !utilBackGround()) {
        perror("utilBackGround");
        exit(MY_MOD_ID + 1);
    }

/* Start logging facility */
       
    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(argv[0], log, sta, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID + 2);
    }

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit();

/* Launch the packet pusher thread, if applicable */

    if (dl != NULL && !SetDlOutput(dl, lp)) {
        LogMsg(LOG_INFO, "ERROR with remote disk loop output option\n");
        GracefulExit(MY_MOD_ID + 3);
    }

/* Initialze the packet builder */

    InitPacketBuilder();

/* Initialize the raw line processor */

    InitStringProcessor(lograw);

/* Launch the SNMP query app */

    sprintf(command, "%s %s", DEFAULT_POLL_SCRIPT, peer);
    if ((fp = popen(command, "r")) == NULL) {
        LogMsg(LOG_INFO, "ERROR: popen(%s): %s\n", command, strerror(errno));
        GracefulExit(MY_MOD_ID + 4);
    }
    SetPipeFILE(fp);

/* Spend eternity reading strings from the co-process */

    while (utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL) == 0) ProcessString(input, sta, net, options);

/* This should never be reached */

    if (!ShutdownInProgress()) {
        LogMsg(LOG_INFO, "%s: ERROR: utilGetLine: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 5);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * Revision 1.4  2016/11/16 22:00:11  dechavez
 * added missing argument to the unrecognized argument error fprint
 *
 * Revision 1.3  2016/05/02 17:53:46  dechavez
 * added missing utilSetIdentity() call to run as specified user
 *
 * Revision 1.2  2016/04/30 19:08:42  dechavez
 * added the packet pusher to init phase (duh)
 *
 * Revision 1.1  2016/04/28 23:01:49  dechavez
 * initial release
 *
 */
