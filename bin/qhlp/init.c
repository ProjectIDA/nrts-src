#pragma ident "$Id: init.c,v 1.27 2015/12/09 18:42:28 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#define INCLUDE_QHLP_STATIC_DEFAULTS
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_INIT

typedef struct {
    BOOL pkt;
    BOOL ttag;
    BOOL bwd;
    BOOL dl;
    BOOL lock;
} DEBUG_FLAGS;

/* Open input and output disk loops */

static ISI_DL *OpenDiskLoop(char *dbspec, char *myname, char *site, LOGIO *lp, int perm, DEBUG_FLAGS *dbg, BOOL laxnrts)
{
ISI_DL *dl;
static ISI_GLOB glob;
static UINT32 options = ISI_OPTION_GENERATE_SEQNO | ISI_OPTION_INSERT_32BIT_SEQNO | ISI_OPTION_INSERT_32BIT_TSTAMP;
static char *fid = "OpenDiskLoop";

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }

    if (dbg != NULL) {
        if (dbg->pkt) glob.debug.pkt = LOG_INFO;
        if (dbg->ttag) glob.debug.ttag = LOG_INFO;
        if (dbg->bwd) glob.debug.bwd = LOG_INFO;
        if (dbg->dl) glob.debug.dl = LOG_INFO;
        if (dbg->lock) glob.debug.lock = LOG_INFO;
    }

    if ((dl = isidlOpenDiskLoop(&glob, site, lp, perm, options)) == NULL) {
        fprintf(stderr, "%s: isidlOpenDiskLoop failed: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 2);
    }

    if (dl->nrts != NULL) {
        dl->nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM;
        dl->options.flags |= ISI_OPTION_REJECT_COMPLETE_OVERLAPS;
    }
    if (laxnrts)  dl->nrts->flags |= NRTS_DL_FLAGS_LAX_TIME;

    return dl;
}

static void help(char *myname)
{
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ options ] qdp_site hlp_site\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "db=spec         => set database to `spec'\n");
    fprintf(stderr, "log=name        => set log file name\n");
    fprintf(stderr, "net=port        => accept data packets at TCP/IP port 'port'\n");
    fprintf(stderr, "-ida10          => generate IDA10 packets\n");
    fprintf(stderr, "-lcase          => use lower case channel names\n");
    fprintf(stderr, "-laxnrts        => relax NRTS disk loop time stamp sanity requirements\n");
    fprintf(stderr, "-laxtokens      => do not require LCQ token entries for all streams\n");
    fprintf(stderr, "-continuous     => all channels continous (no event trigger)\n");
    fprintf(stderr, "seedlink=cfgstr => set SeedLink configuration (where cfgstr is srv:port:len:depth:net)\n");
    fprintf(stderr, "-noseedlink     => disable SeedLink support\n");
    fprintf(stderr, "slinkdebug=int  => set SeedLink debug level\n");
#ifdef unix
    fprintf(stderr, "-bd             => run in the background\n");
#endif /* unix */
    fprintf(stderr, "\n");
    exit(MY_MOD_ID + 3);
}

QHLP_PAR *init(char *myname, int argc, char **argv)
{
int i, errcode;
char *log = NULL;
LOGIO *lp = NULL;
char *dbspec = DEFAULT_DB;
int format = DEFAULT_FORMAT;
BOOL complete;
BOOL laxnrts = FALSE;
BOOL debug = DEFAULT_DEBUG;
BOOL daemon = DEFAULT_DAEMON;
UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
QHLP_DL_FUNC save = NULL;
char *root = NULL;
static char *seedlink = ISI_DL_DEFAULT_SLINK_CFGSTR;
Q330_CFG *cfg = NULL;
char *watchdir = NULL;
UINT32 trigflags = QHLP_TRIG_DEFAULT;
int net = 0;
static char *dbgdir = DEFAULT_DEBUG_DIR;
static char *user = DEFAULT_USER;
static DEBUG_FLAGS dbg = { FALSE, FALSE, FALSE, FALSE, FALSE };
static QHLP_PAR qhlp;
int slinkdebug = ISI_DL_DEFAULT_SLINK_DEBUG;

    qhlp.input.site = qhlp.output.site = NULL;
    qhlp.SyncInterval = DEFAULT_SYNC_INTERVAL;
    strncpy(qhlp.output.netid, DEFAULT_NETID, IDA105_NNAME_LEN);

    for (complete = FALSE, i = 1; i < argc; i++) {
        if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncmp(argv[i], "dbgdir=", strlen("dbgdir=")) == 0) {
            dbgdir = argv[i] + strlen("dbgdir=");
        } else if (strncmp(argv[i], "netid=", strlen("netid=")) == 0) {
            strncpy(qhlp.output.netid, argv[i] + strlen("netid="), IDA105_NNAME_LEN);
        } else if (strncmp(argv[i], "sync=", strlen("sync=")) == 0) {
            qhlp.SyncInterval = (UINT64) atoi(argv[i] + strlen("sync="));
        } else if (strncmp(argv[i], "root=", strlen("root=")) == 0) {
            root = argv[i] + strlen("root=");
        } else if (strncmp(argv[i], "watchdir=", strlen("watchdir=")) == 0) {
            watchdir = argv[i] + strlen("watchdir=");
        } else if (strcmp(argv[i], "-laxnrts") == 0) {
            laxnrts = TRUE;
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            if (net != 0) {
                fprintf(stderr, "%s: multiple `net' servers not allowed\n", argv[0]);
                help(argv[0]);
            }
            net = atoi(argv[i] + strlen("net="));
            if (net < 1) {
                fprintf(stderr, "%s: illegal parameter `%s'\n", argv[0], argv[i]);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "seedlink=", strlen("seedlink=")) == 0) {
            seedlink = argv[i] + strlen("seedlink=");
        } else if (strcmp(argv[i], "-noseedlink") == 0) {
            seedlink = NULL;
        } else if (strncmp(argv[i], "slinkdebug=", strlen("slinkdebug=")) == 0) {
            slinkdebug = atoi(argv[i] + strlen("slinkdebug="));
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-ida10") == 0) {
            format = FORMAT_IDA10;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-lcase") == 0) {
            flags |= QDP_HLP_RULE_FLAG_LCASE;
        } else if (strcmp(argv[i], "-laxtokens") == 0) {
            flags &= ~QDP_HLP_RULE_FLAG_STRICT;
        } else if (strcmp(argv[i], "-dbgpkt") == 0) {
            dbg.pkt = TRUE;
        } else if (strcmp(argv[i], "-dbgttag") == 0) {
            dbg.ttag = TRUE;
        } else if (strcmp(argv[i], "-dbgbwd") == 0) {
            dbg.bwd = TRUE;
        } else if (strcmp(argv[i], "-dbgdl") == 0) {
            dbg.dl = TRUE;
        } else if (strcmp(argv[i], "-dbglock") == 0) {
            dbg.lock = TRUE;
        } else if (strcmp(argv[i], "-continuous") == 0) {
            trigflags |= QHLP_TRIG_CONTINUOUS;
        } else if (qhlp.input.site == NULL) {
            qhlp.input.site = argv[i];
        } else if (qhlp.output.site == NULL) {
            qhlp.output.site = argv[i];
            complete = TRUE;
        } else {
            fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (!complete) {
        fprintf(stderr, "%s: incomplete command line\n", argv[0]);
        help(argv[0]);
    }

    sprintf(qhlp.dbginfo, "%s/%s", dbgdir, utilBasename(argv[0]));

/* Switch to runtime user */

    utilSetIdentity(user);

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(myname, log, qhlp.output.site, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID + 5);
    }
    par.lcq.lp = lp;

/* Initialize watchdog, if any */

    if (watchdir != NULL) Watch = logioInitWatchdog(watchdir, argv[0]);

/* Read the Q330 config file */

    if ((cfg = q330ReadCfg(root, &errcode)) == NULL) {
        q330PrintErrcode(stderr, "q330ReadCfg: ", root, errcode);
        exit(MY_MOD_ID + 6);
    } else {
        LogMsg("Q330 config path = %s/%s", cfg->root, Q330_NAME_SUBDIR);
    }

/* Open disk loops */

    LogMsg("Opening %s disk loop for QDP source", qhlp.input.site);
    qhlp.input.dl = OpenDiskLoop(dbspec, argv[0], qhlp.input.site, lp, ISI_RDONLY, NULL, FALSE);
    if (!isiInitRawPacket(&qhlp.input.raw, NULL, qhlp.input.dl->sys->maxlen)) {
        fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
        exit(MY_MOD_ID + 7);
    }

    LogMsg("Opening %s disk loop for HLP destination", qhlp.output.site);
    qhlp.output.dl = OpenDiskLoop(dbspec, argv[0], qhlp.output.site, lp, ISI_RDWR, &dbg, laxnrts);
    if (!isiInitRawPacket(&qhlp.output.raw, NULL, qhlp.output.dl->sys->maxlen)) {
        fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
        exit(MY_MOD_ID + 8);
    }

    strlcpy(qhlp.output.raw.hdr.site, qhlp.output.dl->sys->site, ISI_SITELEN+1);
    qhlp.output.raw.hdr.len.used = qhlp.output.dl->sys->maxlen;
    qhlp.output.raw.hdr.len.native = qhlp.output.dl->sys->maxlen;

/* Configure SeedLink feeder, now that we have log pointer and output disk loop set */

    if (seedlink != NULL && !isidlSetSeedLinkOption(qhlp.output.dl, seedlink, argv[0], slinkdebug)) help(argv[0]);
    isidlLogSeedLinkOption(lp, LOG_INFO, qhlp.output.dl);

/* Determine and initialize the HLP converter */

    switch (format) {
      case FORMAT_IDA10:
        if (!InitIDA10(&par.lcq.rules, &qhlp.output, flags)) {
            fprintf(stderr, "ERROR: failed to initialize IDA10 packet formatter!\n");
            exit(MY_MOD_ID + 9);
        }
        save = SaveIDA10;
        LogMsg("Packet converter configured for IDA10");
        break;
      default:
        fprintf(stderr, "%s: unsupported high level packet format\n", argv[0]);
        exit(MY_MOD_ID + 10);
    }

/* Initialize the HLP callback */

    if (!InitPacketProcessor(cfg, qhlp.output.dl, qhlp.input.dl->path.meta, save, trigflags)) {
        fprintf(stderr, "ERROR: failed to initialize HLP callback: %s\n", strerror(errno));
        exit(MY_MOD_ID + 11);
    }

/* Create QDPLUS packet processor handle */

    par.site = qhlp.input.site;
    if (!(qhlp.input.dl->flags & ISI_DL_FLAGS_HAVE_META)) {
        fprintf(stderr, "ERROR: no meta data found for site = %s\n", par.site);
        exit(MY_MOD_ID + 12);
    }
    par.path.meta = qhlp.input.dl->path.meta;
    par.path.state = qhlp.input.dl->path.qdpstate;

    if ((qhlp.qdplus = qdplusCreateHandle(&par)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("qdplusCreateHandle");
        exit(MY_MOD_ID + 13);
    }

    LogMsg("QDP metadata directory = %s", par.path.meta);
    LogMsg("HLP state file = %s", par.path.state);
    switch (qhlp.qdplus->state) {
      case QDPLUS_STATE_OK:
      case QDPLUS_STATE_EMPTY:
        break;
      case QDPLUS_STATE_DIRTY:
        LogMsg("WARNING: HLP state file %s is dirty", par.path.state);
        break;
      default:
        LogMsg("ERROR: unable to load HLP state file %s (state=%d)", par.path.state, qhlp.qdplus->state);
        fprintf(stderr, "ERROR: unable to load HLP state file %s\n", par.path.state);
        exit(MY_MOD_ID + 14);
    }

/* Go into the background, if applicable */

    if (daemon && !BackGround(qhlp.output.dl)) {
        perror("BackGround");
        exit(MY_MOD_ID + 15);
    }

/* Initialize exit handler */

    InitExit(&qhlp);

/* Start signal handling thread */

    StartSignalHandler(&qhlp);

/* Launch packet server, if configured */

    if (net != 0 && !isidlPacketServer(qhlp.output.dl, net, lp, WritePacketToDisk)) {
        LogMsg("init: isidlPacketServer: %s", strerror(errno));
        Exit(MY_MOD_ID + 16);
    }

/* Note trigger data */

    LogTriggerData();

/* Recover state, if any */

    RecoverState(&qhlp);

/* All done, return handle to run time parameters */

    return &qhlp;

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
 * Revision 1.27  2015/12/09 18:42:28  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.26  2014/08/28 21:38:57  dechavez
 * support move of ISI_DL_OPTIONS into ISI_DL handle and use of WritePacketToDisk() with isiPushServer()
 *
 * Revision 1.25  2014/08/19 18:16:19  dechavez
 * Added slinkdebug option
 *
 * Revision 1.24  2013/03/08 18:43:17  dechavez
 * use new (libisidl 1.22.0) isidlSetSeedLinkOption() and isidlLogSeedLinkOption() args,
 * moved to after output dl has been opened (since slink now resides in ISI_DL handle)
 *
 * Revision 1.23  2013/01/23 23:37:39  dechavez
 * include LOGIO in isidlLogSeedLinkOption() call
 *
 * Revision 1.22  2013/01/16 23:03:11  dechavez
 * Enabled SeedLink support by default, added -noseedlink option to suppress
 *
 * Revision 1.21  2013/01/14 22:40:09  dechavez
 * SeedLink support introduced
 *
 * Revision 1.20  2011/11/07 17:42:22  dechavez
 * added bud= option, moved accomodated move of mseed from DISK_LOOP_DATA to ISI_DL_OPTIONS
 *
 * Revision 1.19  2011/11/03 17:53:23  dechavez
 * Generate equivalent MiniSEED equivalent, when possible
 *
 * Revision 1.18  2011/10/12 17:40:26  dechavez
 * added support for net=port argument to invoke ISI_PUSH server option
 *
 * Revision 1.17  2011/04/07 23:00:03  dechavez
 * added netid option to override default
 *
 * Revision 1.16  2010/12/21 20:14:09  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 *
 * Revision 1.15  2010/12/17 19:39:08  dechavez
 * changed Q330 config option to specify root of Q330 configuration database
 *
 * Revision 1.14  2010/11/24 22:06:15  dechavez
 * q330ReadCfg with error code
 *
 * Revision 1.13  2009/07/25 17:42:35  dechavez
 * added -lax option
 *
 * Revision 1.12  2009/07/02 23:10:06  dechavez
 * added -continuous and -nocaltrig options
 *
 * Revision 1.11  2009/05/14 18:12:44  dechavez
 * added debug verbosity toggling via /usr/nrts/etc/debug
 *
 * Revision 1.10  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.9  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.8  2008/12/17 02:24:39  dechavez
 * note state when aborting on bad qhlp state file
 *
 * Revision 1.7  2008/12/15 23:29:36  dechavez
 * support corrupt or empty state files
 *
 * Revision 1.6  2007/09/22 02:34:55  dechavez
 * use Q330 config file to get detector parameters
 *
 * Revision 1.5  2007/09/17 23:27:51  dechavez
 * checkpoint - reworking threshold trigger (now with specific list of triggered channels)
 *
 * Revision 1.4  2007/09/14 19:53:48  dechavez
 * added various dbg options, trigger stuff
 *
 * Revision 1.3  2007/09/07 20:29:42  dechavez
 * set runtime identity to "nrts" or that specified via user= argument
 *
 * Revision 1.2  2007/09/07 19:57:44  dechavez
 * Fixed missing state file bug.  Set NRTS disk loop "station system" flag.
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
