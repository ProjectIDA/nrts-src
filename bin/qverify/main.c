#pragma ident "$Id: main.c,v 1.11 2016/08/19 16:20:12 dechavez Exp $"
/*======================================================================
 *
 *  Register with a Q330 and disconnect.  Used to verify that the I/O
 *  database is complete and correct for use by other applications.
 *
 *====================================================================*/
#include "q330.h"

typedef struct {
    Q330_CFG *cfg;
    Q330_ADDR addr;
    int port;
    int debug;
    LOGIO *lp;
    QDP *qdp;
} Q330;

extern char *VersionIdentString;

void help(char *myname, int status)
{
    fprintf(stderr, "usage: %s [ -q watchdog=secs root=path debug={0|1|2|3|4} ] NameOrDotDecimalIP { c | s | 1 | 2 | 3 | 4 }\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "         -q => don't print ident string\n");
    fprintf(stderr, "  root=path => path to root of Q330 configuration directory (default=%s)\n", Q330_DEFAULT_ROOT);
    fprintf(stderr, "debug=value => set debug verbosity\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "where the final argument specifies the connection port:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    c => configuration port\n");
    fprintf(stderr, "    s => special functions port\n");
    fprintf(stderr, "    1 => data port 1\n");
    fprintf(stderr, "    2 => data port 2\n");
    fprintf(stderr, "    3 => data port 3\n");
    fprintf(stderr, "    4 => data port 4\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "eg: %s watchdog=%d root=%s xxx00 4\n", myname, Q330_DEFAULT_WATCHDOG / MSEC_PER_SEC, Q330_DEFAULT_ROOT);
    fprintf(stderr, "\n");
    exit(status);
}

static void InitQ330(Q330 *q330)
{
static char *fid = "InitQ330";

    q330->cfg = NULL;
    q330->debug = QDP_TERSE;
    q330->lp = NULL;
    q330->qdp = NULL;
    q330->port = QDP_CFG_PORT;
}

static void PrintId(Q330 *q330)
{
int i;

    printf(" %6s", q330->addr.connect.ident);
    printf(" %4u", q330->qdp->c1_fix.proper_tag);
    printf(" %016llX", q330->qdp->c1_fix.sys_num);
    printf(" %d.%03d", q330->qdp->c1_fix.sys_ver.major, q330->qdp->c1_fix.sys_ver.minor);
    printf(" %d.%02d", q330->qdp->c1_fix.sp_ver.major, q330->qdp->c1_fix.sp_ver.minor);
    switch (q330->port) {
      case QDP_LOGICAL_PORT_1:
      case QDP_LOGICAL_PORT_2:
      case QDP_LOGICAL_PORT_3:
      case QDP_LOGICAL_PORT_4:
        printf(" Active Channels:");
        for (i = 0; i < QDP_NCHAN; i++) if ((q330->qdp->meta.combo.glob.gain_map >> i*2) & 0x03) printf(" %d", i+1);
        break;
    }
    printf("\n");
}

int main(int argc, char **argv)
{
Q330 q330;
BOOL verbose = TRUE;
int i, errors = 0, errcode, suberr, watchdog = Q330_DEFAULT_WATCHDOG;
char *root = NULL, *ident = NULL, *port = NULL;
struct sockaddr_in peer;
static char *fid = "init";

    InitQ330(&q330);

    if (argc < 3) help(argv[0], 1);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "root=", strlen("root=")) == 0) {
            root = argv[i] + strlen("root=");
        } else if (strncmp(argv[i], "watchdog=", strlen("watchdog=")) == 0) {
            watchdog = (atoi(argv[i] + strlen("watchdog="))) * MSEC_PER_SEC;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
            help(argv[0], 0);
        } else if (strcmp(argv[i], "-q") == 0) {
            verbose = FALSE;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "debug=0") == 0) {
            q330.debug = QDP_TERSE;
        } else if (strcmp(argv[i], "debug=1") == 0) {
            q330.debug = QDP_INFO;
        } else if (strcmp(argv[i], "debug=2") == 0) {
            q330.debug = QDP_DEBUG;
        } else if (strcmp(argv[i], "debug=3") == 0) {
            q330.debug = QDP_DEBUG2;
        } else if (strcmp(argv[i], "debug=4") == 0) {
            q330.debug = QDP_DEBUG3;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            fprintf(stderr, "illegal debug option\n");
            help(argv[0], 1);
            exit(1);
        } else if (ident == NULL) {
            ident = argv[i];
        } else if (port == NULL) {
            port = argv[i];
        } else {
            fprintf(stderr, "unrecognized argument '%s'\n", argv[i]);
            help(argv[0], 1);
        }
    }

/* Ensure required arguments are present */

    errors = 0;

    if (ident == NULL) {
        printf("ERROR: no digitizer specfied\n");
        ++errors;
    } else if (port == NULL) {
        printf("ERROR: no connection port specfied\n");
        ++errors;
    }
    if (errors) help(argv[0], 1);

/* Ensure the connection port is legal */

    if (strcasecmp(port, "c") == 0) {
        q330.port = QDP_CFG_PORT;
    } else if (strcasecmp(port, "c") == 0) {
        q330.port = QDP_CFG_PORT;
    } else if (strcasecmp(port, "s") == 0) {
        q330.port = QDP_SFN_PORT;
    } else if (strcasecmp(port, "1") == 0) {
        q330.port = QDP_LOGICAL_PORT_1;
    } else if (strcasecmp(port, "2") == 0) {
        q330.port = QDP_LOGICAL_PORT_2;
    } else if (strcasecmp(port, "3") == 0) {
        q330.port = QDP_LOGICAL_PORT_3;
    } else if (strcasecmp(port, "4") == 0) {
        q330.port = QDP_LOGICAL_PORT_4;
    } else {
        fprintf(stderr, "ERROR: illegal port code '%s'\n", port);
        help(argv[0], 1);
    }

/* Read the config file (required) */

    if ((q330.cfg = q330ReadCfg(root, &errcode)) == NULL) {
        q330PrintErrcode(stdout, "ERROR: q330ReadCfg failed: ", root, errcode);
        exit(1);
    }

/* Ensure the digitizer is in the config file */

    if (q330.cfg != NULL && !q330GetAddr(ident, q330.cfg, &q330.addr)) {
        printf("ERROR: no entry for digitizer `%s' in configuration file `%s'\n", ident, q330.cfg->path.addr);
        ++errors;
    } else if (q330.addr.serialno == QDP_INVALID_SERIALNO) {
        printf("ERROR: no serial number found for digitizer `%d'.\n", q330.addr.connect.tag);
        printf("       Edit '%s' or use the \"q330 set\" command.\n", q330.cfg->path.auth);
        ++errors;
    }

/* Abort if everything isn't perfect at this point */

    if (errors) exit(1);

/* Register with the device */

    q330.addr.watchdog = watchdog;
    if ((q330.qdp = q330Register(&q330.addr, q330.port, q330.debug, q330.lp, &errcode, &suberr)) == NULL) {
        fprintf(stderr, "registration with %s %s failed: %s\n", q330.addr.connect.ident, qdpPortString(q330.port), qdpErrcodeString(errcode));
        switch (errcode) {
          case QDP_ERR_REGERR:
            fprintf(stderr, "Q330 responds with error code %d (%s)\n", suberr, qdpErrString(suberr));
            break;
          case QDP_ERR_WATCHDOG:
            fprintf(stderr, "automaton state '%s'\n", qdpStateString(suberr));
            break;
        }
        exit(1);
    }

    if (verbose) PrintId(&q330);

    if (!qdpShutdown(q330.qdp)) {
        fprintf(stderr, "WARNING - qdShutdown() failed\n");
        exit(0);
    }

    exit(0);
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
 * Revision 1.11  2016/08/19 16:20:12  dechavez
 * made -v (verbose) the default and added -q (quiet) option to suppress
 *
 * Revision 1.10  2016/08/15 22:56:58  dechavez
 * use qdpShutdown() instead of q330Deregister(), default watchdog to Q330_DEFAULT_WATCHDOG
 *
 * Revision 1.9  2016/08/09 15:21:06  dechavez
 * changed default watchdog registration timeout to 10 seconds
 *
 * Revision 1.8  2016/07/20 22:28:18  dechavez
 * libqdp 3.13.0, libq330 3.7.0 compatibility mods
 * Replaced maxtry command line option with watchdog option (default 30 sec)
 * Cleaned up error reporting to reflect the saner registration error handling
 * provided by libqdp 3.13.0
 *
 * Revision 1.7  2016/06/23 20:11:55  dechavez
 * added QDP_ERR_REJECTED and QDP_ERR_NODPMEM failure modes
 *
 * Revision 1.6  2016/06/16 15:46:55  dechavez
 * Added support for debug options 4 and 5 (raw packet logging)
 *
 * Revision 1.5  2016/05/10 20:34:16  dechavez
 * print enabled channel list when verifying a data port
 *
 * Revision 1.4  2016/02/12 18:01:44  dechavez
 * Changed verbose option to print digitizer "ident" string
 *
 * Revision 1.3  2016/02/12 00:22:00  dechavez
 * removed Willard conflict suggestion since not releavant for data port connections
 *
 * Revision 1.2  2016/02/12 00:17:27  dechavez
 * changed default maxtry to 2 (5 was taking too damn long)
 *
 * Revision 1.1  2016/02/11 23:56:08  dechavez
 * initial release
 *
 */
