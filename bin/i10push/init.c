#pragma ident "$Id: init.c,v 1.2 2011/10/12 17:49:03 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "push.h"

#define MY_MOD_ID PUSH_MOD_INIT

#define DEFAULT_DEPTH 5

static void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s server:port [options] < IDA10_packet_stream\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "required arguments\n");
    fprintf(stderr, "server => name or dot decimal IP address of server\n");
    fprintf(stderr, "port   => TCP/IP port to connect to\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "optional arguments\n");
    fprintf(stderr,"log=name     => set log file name\n");
    fprintf(stderr,"tcpbuf=value => set TCP/IP send and receive buffer length\n");
    fprintf(stderr, "\n");

    exit(0);
}

static void ParseServerPort(char *myname, char **server, int *port, char *arg)
{
#define MAX_TOKEN 2
char *token[MAX_TOKEN];
#define DELIMITERS ":"
int ntoken;

    if ((ntoken = utilParse(arg, token, DELIMITERS, MAX_TOKEN, 0)) != MAX_TOKEN) {
        fprintf(stderr, "%s: bad server:port string '%s'\n", myname, arg);
        help(myname);
    }

    if ((*server = strdup(token[0])) == NULL) {
        perror("strdup");
        exit(1);
    }

    if ((*port = atoi(token[1])) <= 0) {
        fprintf(stderr, "%s: illegal port number from '%s'\n", myname, arg);
        help(myname);
    }
}

ISI_PUSH *init(char *myname, int argc, char **argv)
{
int i, port, depth = DEFAULT_DEPTH;
char *log = NULL;
IACP_ATTR attr = IACP_DEFAULT_ATTR;
ISI_PUSH *ph;
char *server;
static LOGIO *lp;


    if (argc < 2) help(myname);

    if (!utilNetworkInit()) {
        perror("utilNetworkInit");
        exit(MY_MOD_ID);
    }

/*  Get command line arguments  */

    server = NULL;
    port   = 0;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "tcpbuf=", strlen("tcpbuf=")) == 0) {
            attr.at_sndbuf = attr.at_rcvbuf = atoi(argv[i] + strlen("tcpbuf="));
        } else if (server == NULL) {
            ParseServerPort(myname, &server, &port, argv[i]);
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", myname, argv[i]);
            help(myname);
        }
    }

    if (server == NULL) {
        fprintf(stderr, "%s: missing server:port argument\n", myname);
        help(myname);
    }

/* Start logging facility */

    lp = InitLogging(myname, log);

/* Initialize exit handler */

    InitExit();

/* Start signal handler */

    StartSignalHandler();    

/* Start push facility */

    if ((ph = isiPushInit(server, port, &attr, lp, LOG_INFO, IDA10_MAXRECLEN, depth, TRUE)) == NULL) {
        perror("isiPushInit");
        exit(1);
    }

    LogMsg(LOG_DEBUG, "initialization complete");

    return ph;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: init.c,v $
 * Revision 1.2  2011/10/12 17:49:03  dechavez
 * added (hardcoded) block and queue depth options (required by libisi 2.12.0 version of isiPushInit())
 *
 * Revision 1.1  2011/08/04 22:07:22  dechavez
 * initial release
 *
 */
