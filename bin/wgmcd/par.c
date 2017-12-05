#pragma ident "$Id: par.c,v 1.1 2012/07/03 16:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Get run time parameters. 
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "wgmcd.h"

#define MY_MOD_ID WGMCD_MOD_LOADINI

#define DELIMITERS " =\t"
#define MAX_TOKEN    32
#define PBLEN       256
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

static VOID PrintHelp(CHAR *myname)
{
#ifndef WIN32_SERVICE
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [options]\n", myname);
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
    fprintf(stderr, "status=value    => ");
    fprintf(stderr, "port number for status reports\n");
    fprintf(stderr, "bs=value        => ");
    fprintf(stderr, "maximum size of both incoming and outgoing messages\n");
    fprintf(stderr, "ibs=value       => ");
    fprintf(stderr, "maximum size of incoming messages\n");
    fprintf(stderr, "obs=value       => ");
    fprintf(stderr, "maximum size of outgoing messages\n");
    fprintf(stderr, "maxclient=value => ");
    fprintf(stderr, "maximum number of simultaneous clients allowd\n");
    fprintf(stderr, "log=name        => ");
    fprintf(stderr, "log specifier\n");
    fprintf(stderr, "echo=value      => ");
    fprintf(stderr, "port number for echo service\n");
#ifdef unix
    fprintf(stderr, "-bd             => ");
    fprintf(stderr, "run as a daemon (in the background)\n");
#endif
    fprintf(stderr, "-debug=         => ");
    fprintf(stderr, "turn on debug messages\n");
    fprintf(stderr, "-logtt=         => ");
    fprintf(stderr, "log time tears\n");
    fprintf(stderr, "\n");
#endif /* WIN32_SERVICE */

    exit(1);
}

static VOID Fail(char *fid, FILE *fp, char *path, char *token, UINT32 lineno)
{
    fprintf(stderr, "FATAL ERROR: %s: syntax error at line ", fid);
    fprintf(stderr, "%lu, file `%s', token `%s'\n", lineno, path, token);
    fclose(fp);
    exit(MY_MOD_ID + 1);
}

PARAM *LoadPar(char *prog, int argc, char **argv)
{
int i;
char *path;
char *dbspec = NULL;
static char *bearg[2];
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
    par.attr        = IACP_DEFAULT_ATTR;
    par.debug       = DEFAULT_DEBUG;

/* Scan the command line looking for explict ini file specification */

    path = (char *) NULL;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strcasecmp(argv[i], "-h") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "-help") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "--help") == 0) {
            PrintHelp(prog);
        }
    }

/*  Load the global initialization */

    if (!isidlSetGlobalParameters(dbspec, prog, &par.glob)) {
        fprintf(stderr, "%s: isiSetGlobalParameters: %s\n", prog, strerror(errno));
        exit(1);
    }

/* Now go through the command line looking for explicit overrides */
    
    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            par.port = (UINT16) atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "user=", strlen("user=")) == 0) {
            if ((par.user = strdup(argv[i] + strlen("user="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                return NULL;
            }

#ifdef unix
        } else if (strcasecmp(argv[i], "-bd") == 0) {
            par.daemon = TRUE;
#endif
        
        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            par.buflen.send = par.buflen.recv = atoi(argv[i] + strlen("bs="));

        } else if (strncmp(argv[i], "ibs=", strlen("ibs=")) == 0) {
            par.buflen.recv = atoi(argv[i] + strlen("ibs="));

        } else if (strncmp(argv[i], "obs=", strlen("obs=")) == 0) {
            par.buflen.send = atoi(argv[i] + strlen("obs="));

        } else if (strncmp(argv[i], "maxclient=", strlen("maxclient=")) == 0) {
            par.maxclient = atoi(argv[i] + strlen("maxclient="));

        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            if ((par.log = strdup(argv[i] + strlen("log="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                return NULL;
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
                fprintf(stderr, "%s: illegal priority '%s'\n", 
                    prog, argv[i] + strlen("priority=")
                );
                PrintHelp(prog);
            }

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            ;
        } else if (strcasecmp(argv[i], "-h") == 0) {
            ;
        } else if (strcasecmp(argv[i], "-help") == 0) {
            ;
        } else if (strcasecmp(argv[i], "--help") == 0) {
            ;
        } else if (strncmp(argv[i], "echo=", strlen("echo=")) == 0) {
            par.echo = atoi(argv[i] + strlen("echo="));
        } else {
            fprintf(stderr, "\n%s: unrecognized argument '%s'\n\n", prog, argv[i]);
            PrintHelp(prog);
        }
    }

/* If logging not explicity specified, set default depending on status */

    if (par.log == NULL) par.log = par.daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;

/* All done, return pointer to parameters */

    return &par;
}

/* Log parameters */

VOID LogPar(PARAM *par)
{
    LogMsg(LOG_INFO, "Database    %s",  par->glob.db->dbid);
    LogMsg(LOG_INFO, "Home Dir    %s",  par->glob.root);
    LogMsg(LOG_INFO, "IACP Port   %hu", par->port);
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
 * Revision 1.1  2012/07/03 16:15:15  dechavez
 * initial (barely working) release
 *
 */
