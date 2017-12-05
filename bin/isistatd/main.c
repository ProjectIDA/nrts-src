#pragma ident "$Id: main.c,v 1.3 2015/09/15 19:36:42 dauerbach Exp $"
/*======================================================================
 *
 *  Program to populate the DCC web queues with status information from
 *  a single ISI data server.
 *
 *====================================================================*/
#include "isistatd.h"

#define MY_MOD_ID ISISTATD_MOD_MAIN

static MUTEX mutex;

/******************************************************************************
* Some utility function for sorting and formatting SOH and CNF data
******************************************************************************/
static void UcaseStationName(ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf)
{
int i;

    for (i = 0; i < soh->nentry; i++) {
        util_ucase(soh->entry[i].name.sta);
        util_ucase(cnf->entry[i].name.sta);
    }
}

static int SortSohName(const void *a, const void *b)
{
int result;

    result = strcmp(((ISI_STREAM_SOH *) a)->name.sta, ((ISI_STREAM_SOH *) b)->name.sta);
    if (result != 0) return result;

    result = strcmp(((ISI_STREAM_SOH *) a)->name.chn, ((ISI_STREAM_SOH *) b)->name.chn);
    if (result != 0) return result;

    return strcmp(((ISI_STREAM_SOH *) a)->name.loc, ((ISI_STREAM_SOH *) b)->name.loc);
}

static VOID SortSoh(ISI_SOH_REPORT *soh)
{
    qsort(soh->entry, soh->nentry, sizeof(ISI_STREAM_SOH), SortSohName);
}

static int SortCnfName(const void *a, const void *b)
{
int result;

    result = strcmp(((ISI_STREAM_CNF *) a)->name.sta, ((ISI_STREAM_CNF *) b)->name.sta);
    if (result != 0) return result;

    result = strcmp(((ISI_STREAM_CNF *) a)->name.chn, ((ISI_STREAM_CNF *) b)->name.chn);
    if (result != 0) return result;

    return strcmp(((ISI_STREAM_CNF *) a)->name.loc, ((ISI_STREAM_CNF *) b)->name.loc);
}

static VOID SortCnf(ISI_CNF_REPORT *cnf)
{
    qsort(cnf->entry, cnf->nentry, sizeof(ISI_STREAM_CNF), SortCnfName);
}
/******************************************************************************/

void InitISI(ISISTATD_PAR *statpar)
{
    isiInitDefaultPar(&statpar->isipar);
    isiSetServerPort(&statpar->isipar, statpar->port);
    MUTEX_INIT(&mutex);
}


static void help(char *myname)
{
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ server=name:[port] apiurl=string interval=seconds log=name -bd ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    server=name:[port] => ISI server name [ and port ]\n");
    fprintf(stderr, "    apiurl=string      => web services URL\n");
    fprintf(stderr, "    interval=seconds   => SOH update interval\n");
    fprintf(stderr, "    log=name           => log descriptor\n");
    fprintf(stderr, "    -bd                => run in the background\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "default:");
    fprintf(stderr, " server=%s:%d", DEFAULT_SERVER, DEFAULT_PORT);
    fprintf(stderr, " apiurl=%s",    DEFAULT_APIURL);
    fprintf(stderr, " interval=%d",  DEFAULT_INTERVAL);
    fprintf(stderr, "\n");

     exit(1);
}

int main (int argc, char **argv)
{
int i;
LOGIO *lp;

ISI_SOH_REPORT *soh = NULL;
ISI_CNF_REPORT *cnf = NULL;


char *log = NULL;
char *user   = DEFAULT_USER;
BOOL daemon  = DEFAULT_DAEMON;
BOOL debug   = DEFAULT_DEBUG;
char server[MAXPATHLEN+1];
static char *default_server = DEFAULT_SERVER;
static char *default_apiurl = DEFAULT_APIURL;
ISISTATD_PAR statpar;

    utilNetworkInit();
    statpar.apiurl = default_apiurl;
    statpar.port   = DEFAULT_PORT;
    statpar.interval = DEFAULT_INTERVAL;
    strncpy(statpar.server, default_server, MAXPATHLEN);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            if (!utilParseServer(argv[i] + strlen("server="), statpar.server, &statpar.port)) {
                fprintf(stderr, "utilParseServer(%s): %s\n", argv[i] + strlen("server="), strerror(errno));
                exit(1);
            }
        } else if (strncmp(argv[i], "apiurl=", strlen("apiurl=")) == 0) {
            statpar.apiurl = argv[i] + strlen("apiurl=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "interval=", strlen("interval=")) == 0) {
            statpar.interval = atoi(argv[i] + strlen("interval="));
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else {
           fprintf(stderr, "%s: unexpected argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

/* Switch to runtime user */

    utilSetIdentity(user);

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(argv[0], log, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID + 1);
    }

/* Go into the background, if applicable */

    if (daemon && !utilBackGround()) {
        perror("BackGround");
        exit(MY_MOD_ID + 2);
    }
    LogRunParameters(0, argc, argv, &statpar);

/* Initialize the ISI connection parameters */

    InitISI(&statpar);

/* Initialize exit handler */

    InitExitHandler();

/* Start signal handling thread */

    StartSignalHandler();

/* Main loop */

    while (1) {

        LogMsg(1, "contacting %s:%d", statpar.server, statpar.port);

        // get ISI_SOH_REPORT list
        if ((soh = isiSoh(statpar.server, &statpar.isipar)) == NULL) {
            LogMsg(0, "failed SOH request from %s:%d\n", statpar.server, statpar.port, strerror(errno));
        }

        // get ISI_CNF_REPORT list
        if ((cnf = isiCnf(statpar.server, &statpar.isipar)) == NULL) {
            LogMsg(0, "failed CNF request from %s:%d\n", statpar.server, statpar.port, strerror(errno));
        }

        if ((soh != NULL) && (cnf != NULL)) {
            // upper case station names using ISI_STREAM_SOH.name.sta and ISI_STREAM_CNF.name.sta

            LogMsg(1, "Ucase Station names for SOH and CNF data from %s:%d", statpar.server, statpar.port);
            UcaseStationName(soh, cnf);

            // sort SOH and CNF each by name.sta
            // it is assumed that soh and cnf will be the same length entries for the same channels/stations
            LogMsg(1, "Sort SOH data from %s:%d", statpar.server, statpar.port);
            SortSoh(soh);

            LogMsg(1, "Sort CNF data from %s:%d", statpar.server, statpar.port);
            SortCnf(cnf);

            // process soh/cnf lists and post data by station to "apisohurl"
            ProcessIsiInfo(statpar.server, statpar.apiurl, soh, cnf);

        }

        // (deep) free soh/cnf data structures
        isiFreeSoh(soh);
        isiFreeCnf(cnf);

        sleep(statpar.interval);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |       
 | Copyright (C) 2015 Regents of the University of California            |       
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
 * Revision 1.3  2015/09/15 19:36:42  dauerbach
 * Changed to use isiSoh and isiCnf functions that return Structs with arrays
 *
 * Revision 1.2  2015/09/02 21:40:31  dechavez
 * checkpoint commit
 *
 * Revision 1.1  2015/09/02 21:13:13  dechavez
 * checkpoint commit
 *
 */
