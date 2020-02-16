#pragma ident "$Id: main.c,v 1.5 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Build a POD compatible tree from the database
 *
 *====================================================================*/
#define DBPTR_EXISTS
#include "podir.h"

#define MY_MOD_ID PODIR_MOD_MAIN
int LogLevel = PODIR_DEFAULT_THRESHOLD + 1;

static void help(char *myname)
{
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "usage: %s dirpath [ db=path net=id [ keep=sta1,sta2,... | exclude=sta1,sta2,... ] debug=level]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments:\n");
    fprintf(stderr, "         dirpath => output directory (must not already exist)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Optional arguments:\n");
    fprintf(stderr, "              -v => verbose commentary\n");
    fprintf(stderr, "         db=path => path to database (default is %s)\n", PODIR_DEFAULT_DB_PATH);
    fprintf(stderr, "          net=id => network code     (default is %s)\n", PODIR_DEFAULT_NETWORK);
    fprintf(stderr, "   keep=sta1,... => comma delimited list of stations to include (case insensitive)\n");
    fprintf(stderr, "   KEEP=sta1,... => comma delimited list of stations to include (case sensitive)\n");
    fprintf(stderr, "exclude=sta1,... => comma delimited list of stations to exclude (case insensitive)\n");
    fprintf(stderr, "EXCLUDE=sta1,... => comma delimited list of stations to exclude (case sensitive)\n");
    fprintf(stderr, "instance=# => index (1-based) of SITE instance to build. (e.g. ARU, KIV, PFO, XPF)\n");
    fprintf(stderr, "     debug=level => verbosity (0=terse output, higher numbers increase verbosity)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The keep/KEEP/exclude/EXCLUDE options are mutually exlusive.  Use only one or none.\n");
    fprintf(stderr, "\n");
    exit(MY_MOD_ID + 1);
}

int main(int argc, char **argv)
{
DCCDB *db;
LOGIO *lp;
BOOL verbose = FALSE;
int i, logthreshold = PODIR_DEFAULT_THRESHOLD;
int site_instance = 1;  // request instances of site. relevant for sites have more than 1 entry in the database (e.g. ARU, KIV, PFO, XPF).
int site_count;         // instance counter used when searching for target instance.
char dirpath[MAXPATHLEN+1], workdir[MAXPATHLEN+1], headerdir[MAXPATHLEN+1];
char *net_id = NULL;;
char *dbpath = NULL;
char *outdir = NULL;
static char *default_dbpath = PODIR_DEFAULT_DB_PATH;
static char *default_net_id = PODIR_DEFAULT_NETWORK;
static char *fid = "main";

/* Parse and load command line arguments */

    if (argc < 2) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "db=", strlen("db=")) == 0) {
            dbpath = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "exclude=", strlen("exclude=")) == 0) {
            if (!BuildStationSelectionList(argv[i] + strlen("exclude="), PODIR_ACTION_EXCLUDE, TRUE)) help(argv[0]);
        } else if (strncmp(argv[i], "EXCLUDE=", strlen("EXCLUDE=")) == 0) {
            if (!BuildStationSelectionList(argv[i] + strlen("EXCLUDE="), PODIR_ACTION_EXCLUDE, FALSE)) help(argv[0]);
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            if (!BuildStationSelectionList(argv[i] + strlen("keep="), PODIR_ACTION_INCLUDE, TRUE)) help(argv[0]);
        } else if (strncmp(argv[i], "KEEP=", strlen("KEEP=")) == 0) {
            if (!BuildStationSelectionList(argv[i] + strlen("KEEP="), PODIR_ACTION_INCLUDE, FALSE)) help(argv[0]);
        } else if (strncasecmp(argv[i], "net=", strlen("net=")) == 0) {
            net_id = argv[i] + strlen("net=");
        } else if (strncasecmp(argv[i], "instance=", strlen("instance=")) == 0) {
            site_instance = atoi(argv[i] + strlen("instance="));
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            logthreshold = atoi(argv[i] + strlen("debug="));
            if (logthreshold > PODIR_DEFAULT_THRESHOLD) verbose = TRUE;
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "--h") == 0 || strcasecmp(argv[i], "--help") == 0) {
            help(argv[0]);
        } else if (outdir == NULL) {
            outdir = argv[i];
        } else {
            fprintf(stderr, "unrecognized '%s' argument\n", argv[i]);
            help(argv[0]);
        }
    }

    if (outdir == NULL) {
        fprintf(stderr, "ERROR: output directory path must be specified\n");
        exit(1);
    }

    if (outdir[0] == '-' || strcasecmp(outdir, ".") == 0 || strcasecmp(outdir, "..") == 0) {
        fprintf(stderr, "ERROR: '%s' is not a legal output directory name\n", outdir);
        exit(1);
    }
    getcwd(workdir, MAXPATHLEN);

    if (dbpath == NULL) dbpath = default_dbpath;
    if (net_id == NULL) net_id = default_net_id;

    if (strlen(net_id) > MSEED_NNAMLEN) {
        fprintf(stderr, "ERROR: illegal network code '%s' (too long)\n", net_id);
        exit(1);
    }

/* Create empty output directory */

    if (outdir[0] == '/' || outdir[0] == '.') {
        strcpy(dirpath, outdir);
    } else {
        sprintf(dirpath, "%s/%s", workdir, outdir);
    }

    if (utilDirectoryExists(dirpath)) {
        fprintf(stderr, "ERROR: output directory '%s' already exists\n", dirpath);
        exit(1);
    }

    if (util_mkpath(dirpath, 0775) != 0) {
        LogErr("%s: util_mkpath: ERROR: %s: %s\n", fid, dirpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }
    LogMsg(LogLevel, "output directory '%s' created OK\n", dirpath);

/* Start logging facility */

    LogLevel = verbose ? logthreshold : logthreshold + 1;
    lp = InitLogging(argv[0], NULL, logthreshold);
    LogRunParameters(argc, argv, dbpath, workdir);

/* Open the database */

    if ((db = dccdbOpenDatabase(dbpath, lp, verbose)) == NULL) GracefulExit(MY_MOD_ID + 4);
    LogMsg(LogLevel, "Database '%s' read OK\n", dbpath);

/* Create the POD.req file */

    if (!CreateReq(db, outdir, net_id)) {
        LogErr("%s: CreateReq failed\n", fid);
        GracefulExit(MY_MOD_ID + 7);
    }
    LogMsg(LogLevel, "%s/%s created OK\n", dirpath, PODIR_REQ_FILE_NAME);

/* Create the header dir */

    sprintf(headerdir, "%s/%s", dirpath, PODIR_HEADER_DIR_NAME);
    if (util_mkpath(headerdir, 0775) != 0) {
        LogErr("%s: util_mkpath: ERROR: %s: %s\n", fid, dirpath, strerror(errno));
        GracefulExit(MY_MOD_ID + 8);
    }
    LogMsg(LogLevel, "%s created OK\n", headerdir);

/* Create the H.A file */

    if (!CreateHA(db, outdir)) {
        LogErr("%s: CreateHA failed\n", fid);
        GracefulExit(MY_MOD_ID + 6);
    }
    LogMsg(LogLevel, "%s/%s created OK\n", dirpath, PODIR_HA_FILE_NAME);

/* Loop over all the stations */

    for (i = 0; i < db->nsite; i++) {
        if ((i > 0) && (strcmp(db->site[i-1].sta, db->site[i].sta) == 0)) {
            site_count++;
        } else {
            site_count = 1;
        }
        if ((site_count == site_instance) && ProcessThisSite(&db->site[i])) {
            LogMsg(2, "processing station '%s'\n", db->site[i].sta);
            ProcessSite(db, &db->site[i], net_id, headerdir);
        } else {
            LogMsg(2, "skipping station:instance '%s:%d'\n", db->site[i].sta, site_count);
            continue;
        }
    }

/* All done */

    GracefulExit(0);
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
 * Revision 1.5  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
