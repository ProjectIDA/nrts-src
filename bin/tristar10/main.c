#pragma ident "$Id: main.c,v 1.4 2012/07/25 18:14:05 dechavez Exp $"
/*======================================================================
 *
 *  Generate IDA10 packets from a Tristar TS-45 and write to disk loop
 *
 *====================================================================*/
#include "tristar10.h"

#define MY_MOD_ID TRISTAR10_MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [moxa=dot.dec.ima.lip[:port]] dl=server:port log=name sta [-bd]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "default: moxa=%s\n", DEFAULT_MOXA_IP);
    exit(1);
}

static void GetTimeStamp(IDA10_GENTAG *gentag, int sint)
{
time_t now;

    now = time(NULL) - SAN_EPOCH_TO_1970_EPOCH;
    gentag->tstamp = (now - (now % sint)) * NANOSEC_PER_SEC;
    gentag->status.receiver = 0;
    gentag->status.generic = IDA10_GENTAG_LOCKED;
}

int main(int argc, char **argv)
{
int i, sint = DEFAULT_SAMPLE_INTERVAL;
UINT64 nsint;
modbus_t *ctx;
char *log = NULL;
char *sname = NULL;
char *nname = NULL;
static char *default_nname = DEFAULT_NETID;
static char *user = DEFAULT_USER;
SERVER dl, moxa;
BOOL daemon = DEFAULT_DAEMON, debug = DEFAULT_DEBUG;
LOGIO *lp;
int depth = DEFAULT_PACKET_QUEUE_DEPTH;
IDA10_GENTAG prev, crnt;
SCAN *scan;
UINT32 msec;

/*  Get command line arguments  */

    sprintf(moxa.server, DEFAULT_MOXA_IP);
    moxa.port = DEFAULT_MOXA_PORT;

    dl.server[0] = 0;
    dl.port = -1;
    moxa.bto.tv_sec = moxa.bto.tv_usec = moxa.rto.tv_sec = moxa.rto.tv_usec = 0;

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncasecmp(argv[i], "moxa=", strlen("moxa=")) == 0) {
            if (!utilParseServer(argv[i]+strlen("moxa="), moxa.server, &moxa.port)) {
                fprintf(stderr, "error parsing moxa argument: %s\n", strerror(errno));
                exit(1);
            }
        } else if (strncasecmp(argv[i], "bto=", strlen("bto=")) == 0) {
            utilMsecToTimeval(&moxa.bto, (msec = atoi(argv[i] + strlen("bto="))));
        } else if (strncasecmp(argv[i], "rto=", strlen("rto=")) == 0) {
            utilMsecToTimeval(&moxa.rto, (msec = atoi(argv[i] + strlen("rto="))));
        } else if (strncasecmp(argv[i], "dl=", strlen("dl=")) == 0) {
            if (!utilParseServer(argv[i]+strlen("dl="), dl.server, &dl.port)) {
                fprintf(stderr, "error parsing dl argument: %s\n", strerror(errno));
                exit(1);
            }
        } else if (strncasecmp(argv[i], "sint=", strlen("sint=")) == 0) {
            if ((sint = atoi(argv[i] + strlen("sint="))) < MIN_SAMPLE_INTERVAL) {
                fprintf(stderr, "illegal %s\n", argv[i]);
                help(argv[0]);
            }
        } else if (strncasecmp(argv[i], "net=", strlen("net=")) == 0) {
            nname = argv[i] + strlen("net=");
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "-debug", strlen("-debug")) == 0) {
            debug = TRUE;
        } else if (strncmp(argv[i], "-bd", strlen("-bd")) == 0) {
            daemon = TRUE;
        } else if (sname == NULL) {
            sname = argv[i];
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (nname == NULL) nname = default_nname;

/* Run as the specified user */

    utilSetIdentity(user);

/* Check for required arguments */

    if (sname == NULL) {
        fprintf(stderr, "ERROR: missing station name\n");
        help(argv[0]);
    }

    if (strlen(dl.server) == 0 || dl.port < 0) {
        fprintf(stderr, "ERROR: missing or partial dl=server:port argument\n");
        help(argv[0]);
    }

    if (moxa.port < 0) {
        fprintf(stderr, "ERROR: bad moxa port\n");
        help(argv[0]);
    }

    nsint = sint * NANOSEC_PER_SEC;

/* Go into the background, if applicable */

    BackGround(daemon);

/* Initialize the exit handler */

    InitExit();

/* Start logging */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    lp = InitLogging(argv[0], log, sname, debug);

/* Start signal handler */

    StartSignalHandler();

/* Start the packet pusher */

    StartPacketPusher(&dl, lp, depth, sname, nname, nsint);

/* Start polling the device */

    StartMoxaThread(&moxa);

/* Fill packets at the desired sample interval */

    GetTimeStamp(&prev, sint);

    while (1) {
        QuitOnShutdown(0);
        if (FlushFlagSet()) FlushAllPackets();
        GetTimeStamp(&crnt, sint);
        if (crnt.tstamp > prev.tstamp) {
            scan = LockScan();
                StuffScan(&crnt, scan, nsint);
            UnlockScan();
            prev = crnt;
        }
        sleep(1);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2012 Regents of the University of California            |
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
 * Revision 1.4  2012/07/25 18:14:05  dechavez
 * added user=name option, default DEFAULT_USER, run as same
 *
 * Revision 1.3  2012/05/02 18:30:48  dechavez
 * *** initial production release ***
 *
 * Revision 1.2  2012/04/26 17:38:56  dechavez
 * added bto and rto options and don't start stuffing samples until GetSample
 * gets its first live reading
 *
 * Revision 1.1  2012/04/25 21:21:04  dechavez
 * initial release
 *
 */
