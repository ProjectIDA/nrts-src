#pragma ident "$Id: main.c,v 1.2 2018/01/10 23:52:16 dechavez Exp $"
/*======================================================================
 *
 *  Listen for IDA10 packets coming in via ISI push and write to local dl
 *
 *====================================================================*/
#include "i10dld.h"

#define MY_MOD_ID MOD_MAIN

static int help(char *myname)
{
    fprintf(stderr, "usage: %s port=value site=name [db=spec log=spec -noseedlink -bd]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments:\n");
    fprintf(stderr, "port=value  => listen for connections at specified port\n");
    fprintf(stderr, "site=name   => ISI/NRTS site/station name\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Optional arguments:\n");
    fprintf(stderr, "db=spec     => set database root directory to 'spec'\n");
    fprintf(stderr, "log=spec    => log file name or syslog specification\n");
    fprintf(stderr, "-laxnrts    => relax NRTS disk loop time stamp sanity requirements\n");
    fprintf(stderr, "-noseedlink => disable SeedLink support\n");
    fprintf(stderr, "-bd         => run in the background\n");

    Exit(MY_MOD_ID + 1);
}

int main(int argc, char **argv)
{
int i;
char *myname;
LOGIO *lp = NULL;

int port       = UNDEFINED_PORT;
int options    = DEFAULT_OPTIONS;

char *site   = NULL;
char *SITE   = NULL;
char *log    = NULL;
char *user   = DEFAULT_USER;
char *dbspec = DEFAULT_DB;
char *slink  = ISI_DL_DEFAULT_SLINK_CFGSTR;

BOOL daemon  = DEFAULT_DAEMON;

ISI_DL *dl = NULL;

/* Command line processing */

    myname = argv[0];

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncmp(argv[i], "site=", strlen("site=")) == 0) {
            site = argv[i] + strlen("site=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("dbspec=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "-laxnrts") == 0) {
            options |= OPTION_LAXNRTS;
        } else if (strcmp(argv[i], "-noseedlink") == 0) {
            slink = NULL;
        } else if (strncmp(argv[i], "seedlink=", strlen("seedlink=")) == 0) {
            slink = argv[i] + strlen("seedlink=");
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-h") == 0) {
            help(myname);
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[i]);
            help(myname);
        }
    }

/* Ensure required arguments supplied */

    if (port <= 0) {
        fprintf(stderr, "ERROR: bad or missing 'port' argument\n");
        help(myname);
    }

    if (site == NULL) {
        fprintf(stderr, "ERROR: required 'site' argument not provided\n");
        help(myname);
    }

    if ((SITE = strdup(site)) == NULL) { // (SITE is just the upper case equivalent of the site name)
        perror("strdup");
        Exit(MY_MOD_ID + 2);
    } else {
        util_ucase(SITE);
    }

/* switch to runtime user */

    utilSetIdentity(user);

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(myname, log, SITE)) == NULL) {
        perror("InitLogging");
        Exit(MY_MOD_ID + 3);
    }

    LogCommandLine(argc, argv);

/* go into the background if applicable */

    if (daemon && !utilBackGround()) {
        LogMsg("ERROR: utilBackGround: %s", strerror(errno));
        Exit(MY_MOD_ID + 4);
    }

/* Open the disk loop (do this after background so process id will match what's in the dl sys files) */

    dl = OpenDiskLoop(dbspec, myname, site, lp, options, slink);

/* Launch the signal handler */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit();

/* Fire up the packet server and let it work its magic down in the libraray */

    if (!isidlPacketServer(dl, port, lp, USER_SUPPLIED_WRITE_FUNCTION)) {
        LogMsg("ERROR: isidlPacketServer: %s", strerror(errno));
        Exit(MY_MOD_ID + 6);
    }

/* Periodically poll for shutdown notifications from the signal handler */

    while (1) {
        sleep(1);
        CheckExitStatus();
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2018 Regents of the University of California            |
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
 * Revision 1.2  2018/01/10 23:52:16  dechavez
 * first production release
 *
 * Revision 1.1  2018/01/10 21:20:18  dechavez
 * created
 *
 */
