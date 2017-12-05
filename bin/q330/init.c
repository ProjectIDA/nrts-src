#pragma ident "$Id: init.c,v 1.23 2016/07/20 22:32:59 dechavez Exp $"
/*======================================================================
 *
 *  Initialzation routine
 *
 *====================================================================*/
#include "q330cc.h"

extern char *VersionIdentString;

void PrintCommandLineHelp(char *myname)
{
    printf("usage: %s [ watchdog=secs root=path debug={0|1|2|3|4} ] NameOrDotDecimalIP command [ arguments ]\n", myname);
}

static void InitQ330(Q330 *q330)
{
static char *fid = "InitQ330";

    q330->cfg = NULL;
    q330->cmd.name = NULL;
    q330->cmd.code = Q330_CMD_UNDEFINED;
    if ((q330->cmd.arglst = listCreate()) == NULL) {
        printf("%s: listCreate: %s\n", fid, strerror(errno));
        exit(1);
    }
    q330->debug = QDP_TERSE;
    q330->lp = NULL;
    q330->qdp = NULL;
    q330->port = QDP_CFG_PORT;
}

void init(int argc, char **argv, Q330 *q330)
{
int i, errors = 0, errcode, suberr, watchdog = Q330_DEFAULT_WATCHDOG;
char *root = NULL, *ident = NULL;
struct sockaddr_in peer;
static char *fid = "init";

    if (q330 == NULL) {
        printf("%s: NULL input(s)\n", argv[0]);
        exit(1);
    }

    InitQ330(q330);
    InitCommandMap();

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "root=", strlen("root=")) == 0) {
            root = argv[i] + strlen("root=");
        } else if (strncmp(argv[i], "watchdog=", strlen("watchdog=")) == 0) {
            watchdog = (atoi(argv[i] + strlen("watchdog="))) * MSEC_PER_SEC;
        } else if (strcmp(argv[i], "debug=0") == 0) {
            q330->debug = QDP_TERSE;
        } else if (strcmp(argv[i], "debug=1") == 0) {
            q330->debug = QDP_INFO;
        } else if (strcmp(argv[i], "debug=2") == 0) {
            q330->debug = QDP_DEBUG;
        } else if (strcmp(argv[i], "debug=3") == 0) {
            q330->debug = QDP_DEBUG2;
        } else if (strcmp(argv[i], "debug=4") == 0) {
            q330->debug = QDP_DEBUG3;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            printf("illegal debug option\n");
            PrintCommandLineHelp(argv[0]);
            exit(1);
        } else if (ident == NULL) {
            ident = argv[i];
        } else if (q330->cmd.name == NULL) {
            q330->cmd.name = argv[i];
        } else {
            if (!listAppend(q330->cmd.arglst, argv[i], strlen(argv[i]) + 1)) {
                printf("%s: listAppend: %s\n", argv[0], strerror(errno));
                exit(1);
            }
        }
    }

/* help */

    if (argc < 2 || strcasecmp(ident, "help") == 0) help(q330->cfg, argc, argv);

/* Read the config file (required) */

    if ((q330->cfg = q330ReadCfg(root, &errcode)) == NULL) {
        q330PrintErrcode(stdout, "ERROR: q330ReadCfg failed: ", root, errcode);
        exit(1);
    }

/* Check for database dump requests */

    if (strcasecmp(ident, "list") == 0 || strcasecmp(ident, "lst") == 0) {
        ListDigitizers(q330);
        exit(0);
    } else if (strcasecmp(ident, "cfg") == 0) {
        q330WriteAddr(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "addr") == 0) {
        q330WriteAddr(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "auth") == 0) {
        q330WriteAuth(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "sensor") == 0) {
        q330WriteSensor(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "calib") == 0) {
        q330WriteCalib(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "detector") == 0) {
        q330WriteDetector(stdout, q330->cfg);
        exit(0);
    } else if (strcasecmp(ident, "lcq") == 0) {
        q330WriteLcq(stdout, q330->cfg);
        exit(0);
    }

/* The set command is to modify q330.cfg (and possibly q330/auth) */

    if (strcasecmp(ident, "set") == 0) {
        SetDigitizer(q330, argc, argv);
        exit(0);
    }

/* Everything from here on is a digitizer command.
 * Make sure the digitizer is in the config file.
 */

    if (ident == NULL) {
        printf("ERROR: no digitizer specfied\n");
        ++errors;
    } else if (q330->cfg != NULL && !q330GetAddr(ident, q330->cfg, &q330->addr)) {
        printf("ERROR: no entry for digitizer `%s' in configuration file `%s'\n", ident, q330->cfg->path.addr);
        ++errors;
    } else if (q330->addr.serialno == QDP_INVALID_SERIALNO) {
        printf("ERROR: no serial number found for digitizer `%d'.\n", q330->addr.connect.tag);
        printf("       Edit '%s' or use the \"q330 set\" command.\n", q330->cfg->path.auth);
        ++errors;
    }

/* Make sure the command is recognized */

    if (!VerifyQ330Cmd(q330)) ++errors;

/* Abort if everything isn't perfect at this point */

    if (errors) exit(1);

/* Register with the device */

    q330->addr.watchdog = watchdog;
    if ((q330->qdp = q330Register(&q330->addr, q330->port, q330->debug, q330->lp, &errcode, &suberr)) == NULL) {
        fprintf(stderr, "registration with %s %s failed: %s\n", q330->addr.connect.ident, qdpPortString(q330->port), qdpErrcodeString(errcode));
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
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * Revision 1.23  2016/07/20 22:32:59  dechavez
 * libqdp 3.13.0, libq330 3.7.0 compatibility mods
 * Replaced maxtry command line option with watchdog option (default 30 sec)
 * Cleaned up error reporting to reflect the saner registration error handling
 * provided by libqdp 3.13.0
 *
 * Revision 1.22  2016/06/23 20:31:05  dechavez
 * added debug options 3 and 4, "combo" support
 *
 * Revision 1.21  2016/02/11 19:13:24  dechavez
 * changed QDP_PAR maxsrq to maxtry and QDP nsrq to ntry to follow libqdp 3.11.3 namingg
 * Changed QDP_ERR_NOREPLY to QDP_ERR_NOREPLY_SRVRQ (libqdp 3.11.3) and added test for
 * failed registration due to QDP_ERR_NOREPLY_CHRSP and issue warning as to probable cause
 *
 * Revision 1.20  2016/02/05 16:57:28  dechavez
 * added maxsrq option
 *
 * Revision 1.19  2016/02/04 00:33:51  dechavez
 * restored help command which was inadvertently removed from the previous build
 *
 * Revision 1.18  2016/02/03 17:40:20  dechavez
 * changed usage() to take just the app name as its single argument
 *
 * Revision 1.17  2014/01/24 00:17:03  dechavez
 * cleaned up database dump/set section, allowing the removal of the NeedRegistration flag
 *
 * Revision 1.16  2011/04/12 21:01:37  dechavez
 * Added "lcq" command to list the lcq database.
 *
 * Revision 1.15  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.14  2011/01/07 22:55:19  dechavez
 * Changed command to list dbfiles from "q330 cfg dbfile" to just "q330 dbfile"
 *
 * Revision 1.13  2010/12/21 20:04:10  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 *
 * Revision 1.12  2010/12/17 19:45:00  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.11  2010/11/24 19:23:48  dechavez
 * Slightly more useful error messages when config file fails to load
 *
 * Revision 1.10  2010/03/22 21:58:20  dechavez
 * abort startup if q330 configuration port already in use
 *
 * Revision 1.9  2010/03/12 00:43:30  dechavez
 * Fixed fall through bug when unable to load configuration
 *
 * Revision 1.8  2009/10/02 19:25:41  dechavez
 * fixed comment typo
 *
 * Revision 1.7  2009/07/28 18:15:40  dechavez
 * moved help test to after reading of configuration (in order for calib help to be able to list shortcuts)
 *
 * Revision 1.6  2009/07/25 17:39:25  dechavez
 * initial production support of set command
 *
 * Revision 1.5  2009/07/23 20:31:03  dechavez
 * improved help support
 *
 * Revision 1.4  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
