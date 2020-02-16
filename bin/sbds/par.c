#pragma ident "$Id: par.c,v 1.3 2013/07/19 17:59:29 dechavez Exp $"
/*======================================================================
 *
 *  Get run time parameters. 
 *
 *====================================================================*/
#define INCLUDE_SBD_DEFAULT_ATTR
#include "sbds.h"

#define MY_MOD_ID SBDS_MOD_LOADINI

#define DELIMITERS " =\t"
#define MAX_TOKEN    32
#define PBLEN       256
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

static void PrintHelp(CHAR *myname)
{
#ifndef WIN32_SERVICE
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [options] site\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "db=spec         => set database to `spec'\n");
#ifdef WIN32
    fprintf(stderr, "priority=string => process priority class\n");
#endif /* WIN32 */
    fprintf(stderr, "user=nrts       => ");
    fprintf(stderr, "run as specified user\n");
    fprintf(stderr, "port=value      => ");
    fprintf(stderr, "port number for client connections\n");
    fprintf(stderr, "maxclient=value => ");
    fprintf(stderr, "maximum number of simultaneous clients allowd\n");
    fprintf(stderr, "log=name        => ");
    fprintf(stderr, "log specifier\n");
#ifdef unix
    fprintf(stderr, "-noseedlink     => disable SeedLink support\n");
    fprintf(stderr, "-bd             => ");
    fprintf(stderr, "run as a daemon (in the background)\n");
#endif
    fprintf(stderr, "-debug=         => ");
    fprintf(stderr, "turn on debug messages\n");
    fprintf(stderr, "\n");
#endif /* WIN32_SERVICE */

    exit(1);
}

PARAM *LoadPar(char *prog, int argc, char **argv)
{
int i;
char *dbspec = NULL;
static PARAM par;
static CHAR *default_user = DEFAULT_USER;
static CHAR *fid = "LoadPar";

/* Set default parameters */

    par.daemon      = DEFAULT_DAEMON;
    par.port        = DEFAULT_PORT;
    par.maxclient   = DEFAULT_MAXCLIENT;
    par.priority    = DEFAULT_PROCESS_PRIORITY;
    par.log         = NULL;
    par.user        = default_user;
    par.buflen.send = DEFAULT_SENDBUFLEN;
    par.buflen.recv = DEFAULT_RECVBUFLEN;
    par.attr        = SBD_DEFAULT_ATTR;
    par.debug       = DEFAULT_DEBUG;
    par.site        = NULL;
    par.seedlink    = DEFAULT_SEEDLINK;

/* Now go through the command line looking for explicit overrides */
    
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "port=stdin") == 0) {
            par.port = STDIN_PORT;
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
             par.port = (UINT16) atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "user=", strlen("user=")) == 0) {
            if ((par.user = strdup(argv[i] + strlen("user="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                exit(1);
            }

#ifdef unix
        } else if (strcasecmp(argv[i], "-bd") == 0) {
            par.daemon = TRUE;
        } else if (strcasecmp(argv[i], "-noseedlink") == 0) {
            par.seedlink = FALSE;
#endif
        
        } else if (strncmp(argv[i], "maxclient=", strlen("maxclient=")) == 0) {
            par.maxclient = atoi(argv[i] + strlen("maxclient="));

        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            if ((par.log = strdup(argv[i] + strlen("log="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                exit(1);
            }

        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            par.attr.at_dbgpath = argv[i] + strlen("dbgpath=");

        } else if (strcmp(argv[i], "-debug") == 0) {
            par.debug = TRUE;

        } else if (strcmp(argv[i], "-nolock") == 0) {
            par.glob.flags |= ISI_DL_FLAGS_IGNORE_LOCKS;

        } else if (strncasecmp(argv[i], "priority=", strlen("priority=")) == 0) {
            BOOL ok;
            par.priority = utilStringToProcessPriority(argv[i] + strlen("priority="), &ok);
            if (!ok) {
                fprintf(stderr, "%s: illegal priority '%s'\n", prog, argv[i] + strlen("priority=") );
                PrintHelp(prog);
            }

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strcasecmp(argv[i], "-h") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "-help") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "--help") == 0) {
            PrintHelp(prog);
        } else if (par.site == NULL) {
            if ((par.site = (char *) strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                exit(1);
            }
        } else {
            fprintf(stderr, "\n%s: unrecognized argument '%s'\n\n", prog, argv[i]);
            PrintHelp(prog);
        }
    }

/* ISI "site" name is required */

    if (par.site == NULL) {
        fprintf(stderr, "missing ISI disk loop \"site\" name\n");
        PrintHelp(prog);
    }

/*  Load the global initialization */

    if (!isidlSetGlobalParameters(dbspec, prog, &par.glob)) {
        fprintf(stderr, "%s: isiSetGlobalParameters: %s\n", prog, strerror(errno));
        exit(1);
    }

/* If logging not explicity specified, set default depending on status */

    if (par.log == NULL) par.log = par.daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;

/* All done, return pointer to parameters */

    return &par;
}

/* Log parameters */

void LogPar(PARAM *par)
{
    LogMsg(LOG_INFO, "Database    %s",  par->glob.db->dbid);
    LogMsg(LOG_INFO, "Home Dir    %s",  par->glob.root);
    if (par->port != STDIN_PORT) {
        LogMsg(LOG_INFO, "SBD Port    %hu", par->port);
    } else {
        LogMsg(LOG_INFO, "SBD Port    stdin");
    }
    LogMsg(LOG_INFO, "MaxClient   %lu", par->maxclient);
    LogMsg(LOG_INFO, "RecvBuflen  %lu", par->buflen.recv);
    LogMsg(LOG_INFO, "SendBuflen  %lu", par->buflen.send);
#ifdef WIN32
    LogMsg(LOG_INFO, "Priority    %s",  utilProcessPriorityToString(par->priority));
#endif /* WIN32 */
}

/* Revision History
 *
 * $Log: par.c,v $
 * Revision 1.3  2013/07/19 17:59:29  dechavez
 * fixed missing newline in help message
 *
 * Revision 1.2  2013/05/11 23:08:07  dechavez
 * added port=stdin, plus ADDOSS/IDA10 support
 *
 * Revision 1.1  2013/03/11 23:04:16  dechavez
 * initial release
 *
 */
