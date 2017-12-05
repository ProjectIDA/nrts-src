#pragma ident "$Id: main.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Convert old IDA database to new schema
 *
 *====================================================================*/
#include "convertdb.h"

#define MY_MOD_ID MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "usage: %s [ old=path new=path schema=name debug=level -insane ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Optional arguments:\n");
    fprintf(stderr, "        old=path => path to input database (default is %s)\n", DEFAULT_OLD_PATH);
    fprintf(stderr, "     schema=path => path to input schema (default is %s)\n", DEFAULT_NEW_SCHEMA);
    fprintf(stderr, "      new=prefix => prefix for new db output files (default is %s)\n", DEFAULT_NEW_PREFIX);
    fprintf(stderr, "     debug=level => verbosity (0=terse output, higher numbers increase verbosity)\n");
    fprintf(stderr, "         -insane => suppress sanity checks on input database\n");
    fprintf(stderr, "\n");
    exit(MY_MOD_ID + 1);
}

int main(int argc, char **argv)
{
FILE *fp;
LOGIO *lp;
OLD *old;
DCCDB *new;
int i, logthreshold = DEFAULT_LOGTHRESHOLD;
char *old_path = NULL;
char *new_schema = NULL;
char *new_prefix = NULL;
static char *default_old_path = DEFAULT_OLD_PATH;
static char *default_new_schema = DEFAULT_NEW_SCHEMA;
static char *default_new_prefix = DEFAULT_NEW_PREFIX;
static char *fid = "main";
char tstring1[MAXPATHLEN+1];
char tstring2[MAXPATHLEN+1];
char tstring3[MAXPATHLEN+1];
char path[MAXPATHLEN+1];
BOOL CheckSanity = TRUE;

/* Parse and load command line arguments */

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "old=", strlen("old=")) == 0) {
            old_path = argv[i] + strlen("old=");
        } else if (strncasecmp(argv[i], "new=", strlen("new=")) == 0) {
            new_prefix = argv[i] + strlen("new=");
        } else if (strncasecmp(argv[i], "schema=", strlen("schema=")) == 0) {
            new_schema = argv[i] + strlen("schema=");
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            logthreshold = atoi(argv[i] + strlen("debug="));
        } else if (strcasecmp(argv[i], "-insane") == 0) {
            CheckSanity = FALSE;
        } else if (strcasecmp(argv[i], "-h") == 0 || strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "--h") == 0 || strcasecmp(argv[i], "--help") == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr, "unrecognized '%s' argument\n", argv[i]);
            help(argv[0]);
        }
    }

    if (old_path   == NULL) old_path   = default_old_path;
    if (new_schema == NULL) new_schema = default_new_schema;
    if (new_prefix == NULL) new_prefix = default_new_prefix;

/* Start logging facility */

    lp = InitLogging(argv[0], NULL, logthreshold);
    LogRunParameters(1, argc, argv, old_path, new_schema, new_prefix);

/* Open the old database */

    if ((old = OpenOldDatabase(old_path, lp, CheckSanity)) == NULL) GracefulExit(MY_MOD_ID + 4);

/* Build the new database */

    if ((new = CreateNewDatabase(lp)) == NULL) GracefulExit(MY_MOD_ID + 5);
    LogMsg(1, "Output Database '%s' created OK\n", new_prefix);

    BuildAbbrev(old, new, new_prefix);
    BuildSeedloc(old, new, new_prefix);
    BuildSite(old, new, new_prefix);
    BuildUnits(old, new, new_prefix);
    BuildStage(old, new, new_prefix);
    BuildChan(old, new, new_prefix);

/* Create the descriptor file */

    strcpy(path, new_prefix);
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        GracefulExit(MY_MOD_ID + 6);
    }

    fprintf(fp, "%s\n", new_schema);
    fprintf(fp, "./{%s}\n", new_prefix);
    fclose(fp);
    LogMsg(1, "descriptor file '%s' created OK\n", path);

/* All done */

    GracefulExit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
