#pragma ident "$Id: main.c,v 1.24 2016/09/01 17:12:35 dechavez Exp $"
/*======================================================================
 *
 *  Initiate a Q330 calibration and record the output
 *
 *====================================================================*/
#include "qcal.h"

#define MY_MOD_ID QCAL_MOD_MAIN

BOOL DebugFlag = FALSE;

static BOOL ParseDigitizer(char *digitizer, QCAL *qcal)
{
#define DELIMITER ":"
int port;
char *portstr;
#define MAXTOKEN 5
char *copy, *token[MAXTOKEN];
int ntoken;

    if ((copy = strdup(digitizer)) == NULL) {
        perror("strdup");
        return FALSE;
    }

    switch (utilParse(copy, token, DELIMITER, MAXTOKEN, 0)) {
      case 5: qcal->dport.net    = token[4]; /* no break, fall through */
      case 4: qcal->dport.sta    = token[3]; /* no break, fall through */
      case 3: qcal->dport.tokens = token[2]; /* no break, fall through */
      case 2:                                /* no break, fall through */
        if ((qcal->port = qdpDataPortNumber(atoi(token[1]))) == QDP_UNDEFINED_PORT) {
            fprintf(stderr, "illegal data port '%s'\n", token[1]);
            return FALSE;
        }
      case 1: qcal->digitizer = token[0];
        return TRUE;
    }

    fprintf(stderr, "bad digitizer argument '%s'\n", digitizer);
    return FALSE;
}

int main(int argc, char **argv)
{
QCAL qcal;
QDP *ConfigPortConnection = NULL;
int i, errcode, suberr, err, interval;
INT32 status;
UINT32 options;
char *root, *caltag;
Q330_CALIB calib;
BOOL verbose, verify, HaveE300Control, strict;
struct {
    UINT16 cal;
    UINT16 mon;
} bitmap;

    if (argc < 3) help(argv[0]);

/*  Get command line arguments  */

    qcal.debug = DEFAULT_DEBUG;
    qcal.port  = DEFAULT_PORT;
    qcal.qdp   = NULL;
    qcal.name.prefix[0] = 0;
    qcal.digitizer = qcal.dport.tokens = qcal.dport.sta = qcal.dport.net = NULL;
    bitmap.cal = bitmap.mon = 0;
    verbose = TRUE;
    verify = FALSE;
    root = caltag = NULL;
    options = DEFAULT_OPTIONS;
    strict = TRUE;
    qcal.output = 0; /* begin with no output format(s) selected */

    for (i = 1; i < argc; i++) {

    /* check for optional arguments first */

        if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else if (strncmp(argv[i], "root=", strlen("root=")) == 0) {
            root = argv[i] + strlen("root=");
        } else if (strncmp(argv[i], "prefix=", strlen("prefix=")) == 0) {
            strcpy(qcal.name.prefix, argv[i] + strlen("prefix="));
        } else if (strcmp(argv[i], "-q") == 0) {
            verbose = FALSE;
        } else if (strcmp(argv[i], "-lax") == 0) {
            strict = FALSE;
        } else if (strcmp(argv[i], "-ida10") == 0) {
            qcal.output |= OUTPUT_IDA10;
        } else if (strcmp(argv[i], "-qdp") == 0) {
            qcal.output |= OUTPUT_QDP;
        } else if (strcmp(argv[i], "-mseed") == 0) {
            qcal.output |= OUTPUT_MSEED;
        } else if (strcmp(argv[i], "-V") == 0) {
            verify = TRUE;
        } else if (strncmp(argv[i], "cal=", strlen("cal=")) == 0) {
            if ((bitmap.cal = q330ChannelBitmap(argv[i] + strlen("cal="))) == 0) {
                fprintf(stderr, "illegal %s option\n", argv[i]);
                help(argv[0]);
            }
            if (bitmap.mon == 0) {
                if (bitmap.cal == Q330_SENSOR_A_BITMAP) {
                    bitmap.mon = Q330_SENSOR_A_CALMON;
                } else if (bitmap.cal == Q330_SENSOR_B_BITMAP) {
                    bitmap.mon = Q330_SENSOR_B_CALMON;
                }
            }
        } else if (strncmp(argv[i], "mon=", strlen("mon=")) == 0) {
            if ((bitmap.mon = q330ChannelBitmap(argv[i] + strlen("mon="))) == 0) {
                fprintf(stderr, "illegal %s option\n", argv[i]);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "wdsec=", strlen("wdsec=")) == 0) {
            interval = atoi(argv[i]+strlen("wdsec="));
            SetWatchdogExitDelay(interval * MSEC_PER_SEC);
        } else if (strcmp(argv[i], "debug=0") == 0) {
            qcal.debug = QDP_TERSE;
        } else if (strcmp(argv[i], "debug=1") == 0) {
            qcal.debug = QDP_INFO;
        } else if (strcmp(argv[i], "debug=2") == 0) {
            qcal.debug = QDP_DEBUG;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            fprintf(stderr, "illegal debug option\n");
            help(argv[0]);

    /* then required arguments in expected order */

        } else if (qcal.digitizer == NULL) {
            if (!ParseDigitizer(argv[i], &qcal)) help(argv[0]);
        } else if (caltag == NULL) {
            caltag = argv[i];
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (strict) options |= QDP_HLP_RULE_FLAG_STRICT;

/* Fall back to the default output if none was specified on the command line */

    if (qcal.output == 0) qcal.output = DEFAULT_OUTPUT;

/* Check for required arguments */

    if (qcal.digitizer == NULL) {
        fprintf(stderr,"%s: missing digitizer identifier\n", argv[0]);
        help(argv[0]);
    }
    if (caltag == NULL) {
        fprintf(stderr,"%s: missing calibration type identifier\n", argv[0]);
        help(argv[0]);
    }
    if (verify) {
        printf("qcal.digitizer = %s\n", qcal.digitizer);
        printf("          port = %d\n", qcal.port + 1);
        printf("  dport.tokens = %s\n", qcal.dport.tokens);
        printf("     dport.sta = %s\n", qcal.dport.sta);
        printf("     dport.net = %s\n", qcal.dport.net);
        printf("        caltag = %s\n", caltag);
        printf("    bitmap.cal = 0x%02x\n", bitmap.cal);
        printf("    bitmap.mon = 0x%02x\n", bitmap.mon);
    }

/* Read the configuration file */

    if ((qcal.cfg = q330ReadCfg(root, &errcode)) == NULL) {
        q330PrintErrcode(stderr, "q330ReadCfg: ", root, errcode);
        exit(MY_MOD_ID + 1);
    }

/* Make sure digitizer and calibration tag are in the configuration file */

    if (!q330GetAddr(qcal.digitizer, qcal.cfg, &qcal.addr)) {
        fprintf(stderr, "ERROR: no entry for digitizer `%s' in `%s'\n", qcal.digitizer, qcal.cfg->path.addr);
        exit(MY_MOD_ID + 2);
    }
    if (qcal.addr.serialno == 0) {
        fprintf(stderr, "ERROR: no serial number for digitizer `%d' in `%s'\n", qcal.addr.connect.tag, qcal.cfg->path.auth);
        exit(MY_MOD_ID + 3);
    }

    if (bitmap.cal == Q330_SENSOR_B_BITMAP) {
        if (!q330GetCalibB(caltag, &qcal.addr, qcal.cfg, &calib)) {
            fprintf(stderr, "ERROR: can't load Sensor B '%s' calibration parameters\n", caltag);
            exit(MY_MOD_ID + 4);
        }
    } else if (!q330GetCalib(caltag, &qcal.addr, qcal.cfg, &calib)) {
        fprintf(stderr, "ERROR: can't load Sensor A '%s' calibration parameters\n", caltag);
        exit(MY_MOD_ID + 5);
    }
    qcal.c1_qcal = calib.qcal;
    if (bitmap.cal != 0) qcal.c1_qcal.chans = bitmap.cal;
    if (bitmap.mon != 0) qcal.c1_qcal.monitor = bitmap.mon;

    if (verify) {
        PrintCalib(stdout, &qcal.c1_qcal);
        exit(0);
    }

/* If this calibration involves an E300, enable external calibration */

    HaveE300Control = E300Control(&calib);

/* Generate the output and log names and ensure they are unique */

    BuildFileNames(&qcal, qcal.digitizer, caltag);
    err = 0;
    if ((qcal.output & OUTPUT_QDP) && utilFileExists(qcal.name.qdp)) {
        fprintf(stderr, "ERROR: %s already exists\n", qcal.name.qdp);
        ++err;
    }
    if ((qcal.output & OUTPUT_IDA10) && utilFileExists(qcal.name.ida)) {
        fprintf(stderr, "ERROR: %s already exists\n", qcal.name.ida);
        ++err;
    }
    if ((qcal.output & OUTPUT_MSEED) && utilFileExists(qcal.name.mseed)) {
        fprintf(stderr, "ERROR: %s already exists\n", qcal.name.mseed);
        ++err;
    }
    if (utilFileExists(qcal.name.log)) {
        fprintf(stderr, "ERROR: %s already exists\n", qcal.name.log);
        ++err;
    }
    if (err) exit(MY_MOD_ID + 6);

/* Start logging facility */

    if ((qcal.lp = InitLogging(argv[i], qcal.name.log)) == NULL) exit(MY_MOD_ID + 7);

/* Register with the configuration port */

    if ((ConfigPortConnection = RegisterWithConfigPort(&qcal)) == NULL) exit(MY_MOD_ID + 7);

/* Initialize the exit handler */

    InitExit(&qcal);

/* Configure the data port tokens */

    if (!ConfigureDataPort(ConfigPortConnection, &qcal)) Exit(MY_MOD_ID + 8);

/* Start signal handling thread */

    StartSignalHandler(&qcal);

/* Prep the output */

    if (!OpenQDP(&qcal)) {
        SetDeleteFlag();
        Exit(MY_MOD_ID + 9);
    }
    if (!OpenIDA(&qcal, options)) {
        SetDeleteFlag();
        Exit(MY_MOD_ID + 10);
    }
    if (!OpenMSEED(&qcal)) {
        SetDeleteFlag();
        Exit(MY_MOD_ID + 11);
    }

/* Start pulling data */

    if (!StartDataThread(&qcal)) {
        SetDeleteFlag();
        Exit(MY_MOD_ID + 12);
    }

/* Launch the calibration */

    if (!LaunchCal(ConfigPortConnection, &qcal, HaveE300Control)) {
        SetDeleteFlag();
        Exit(MY_MOD_ID + 13);
    }
    qdpShutdown(ConfigPortConnection); /* won't need this until the calibration completes */

    if (verbose) {
        printf("\n");
        if ((qcal.output & OUTPUT_QDP))   printf("QDP data will be written to %s\n", qcal.name.qdp);
        if ((qcal.output & OUTPUT_IDA10)) printf("IDA data will be written to %s\n", qcal.name.ida);
        if ((qcal.output & OUTPUT_MSEED)) printf("MiniSEED data will be written to %s\n", qcal.name.mseed);
        printf("Log data will be written to %s\n", qcal.name.log);
    } else {
        printf("%s\n", qcal.name.prefix);
    }

/* Wait for calibration to finish... data thread or signal handler will bring us down */

    while (1) {
        if ((status = ExitStatus()) == QCAL_STATUS_RUNNING) {
            sleep(1);
        } else {
            if (calib.input.e300.present) e300Safe(calib.input.e300.name);
            Exit(status);
        }
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * Revision 1.24  2016/09/01 17:12:35  dechavez
 * introduced RegisterWithConfigPort() as part of initialization
 *
 * Revision 1.23  2016/02/05 20:10:00  dechavez
 * fixed problem with the root= option
 *
 * Revision 1.22  2015/12/07 19:01:26  dechavez
 * added MiniSEED and QCAL "output" field support
 *
 * Revision 1.21  2014/10/21 21:55:10  dechavez
 * moved all E300 interaction off to e300.c
 *
 * Revision 1.20  2014/06/12 20:19:20  dechavez
 * added -lax and -qdp options
 *
 * Revision 1.19  2013/06/19 22:24:37  dechavez
 * fixed bug where sensor B calibrations were getting the sensor A cal parameters
 * added cal=A and cal=B support instead of actual channel numbers for bitmap
 *
 * Revision 1.18  2012/06/24 18:28:56  dechavez
 * Modified to use libqdp 3.4.0 where data port and tokens are bound together
 * and main channel output frequencies are forced to agree with token set
 *
 * Revision 1.17  2012/01/11 19:22:08  dechavez
 * added wdsec option
 *
 * Revision 1.16  2011/12/15 21:51:59  dechavez
 * added -V (verify) option
 *
 * Revision 1.15  2011/04/16 04:52:56  dechavez
 * added cal and mon options to override default calibration and monitor channels
 *
 * Revision 1.14  2011/04/14 19:21:42  dechavez
 * added QCAL.token support
 *
 * Revision 1.13  2011/01/25 18:47:20  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.12  2010/12/23 21:56:29  dechavez
 * cleaned up presentation
 *
 * Revision 1.11  2010/12/23 00:07:42  dechavez
 * use qdpDataPortNumber() to assign data port parameter
 *
 * Revision 1.10  2010/12/21 20:13:49  dechavez
 * use q330PrintErrcode() for more informative q330ReadCfg() failures
 * add check for missing serial number
 *
 * Revision 1.9  2010/12/17 20:04:42  dechavez
 * removed reference to obsolete Q330_DEFAULT_CFG_NAME
 *
 * Revision 1.8  2010/12/17 19:41:45  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.7  2010/11/24 22:09:47  dechavez
 * q330ReadCfg with error code, get E330 stuff from Q330_SENSOR data
 *
 * Revision 1.6  2010/09/30 17:54:52  dechavez
 * improved error message when failing to configure E300
 *
 * Revision 1.5  2010/09/30 17:27:23  dechavez
 * Automatically control external calibration lines for E300 systems
 *
 * Revision 1.4  2010/04/12 21:09:08  dechavez
 * removed some console verbosity, moved things around in a failed effort to
 * cleaning background the whole affair
 *
 * Revision 1.3  2010/04/05 21:27:32  dechavez
 * changed structure of exit poll loop to permit debug messages to get printed
 *
 * Revision 1.2  2010/04/01 20:27:18  dechavez
 * instead of pemanent pause(), poll for exit flag and Exit() when it is set
 *
 * Revision 1.1  2010/03/31 19:46:38  dechavez
 * initial release
 *
 */
