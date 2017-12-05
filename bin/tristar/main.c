#pragma ident "$Id: main.c,v 1.5 2016/09/01 18:02:44 dechavez Exp $"
/*======================================================================
 *
 *  Tristar TS-45 command and control, via Moxa gateway.
 *
 *====================================================================*/
#define TS_INCLUDE_STATIC_TS_DEFAULT_OPT
#define TS_INCLUDE_STATIC_CMD_MAP
#include "tristar.h"

BOOL DebugFlag = FALSE;

static void help(char *myname)
{

    fprintf(stderr, "usage: %s { charge[=cmd] | load[=cmd] } [options]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Required arguments:\n");
    fprintf(stderr, "charge[=cmd]  => charge controller command (default=soh)\n");
    fprintf(stderr, "load[=cmd]    => load controller command (default=soh)\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "spc=server[:port] => MODBUS/TCP server name or dot decmial IP address, with optional port number\n");
    fprintf(stderr, "bto=msec          => set byte timeout\n");
    fprintf(stderr, "rto=msec          => set response timeout\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Either charge or load must be specified.  If no cmd is given, 'soh' is assumed.\n");
    fprintf(stderr, "Specify charge=list or load=list to see a list of supported commands\n");
    fprintf(stderr, "Default spc=%s:%d\n", TS_DEFAULT_SERVER, TS_DEFAULT_PORT);
    exit(1);
}

static void ListSupportedCommands(int device) 
{
int i, maxlen; 
char format[MAXPATHLEN+1];

    for (maxlen = 0, i = 0; TS_CMD_MAP[i].code != TS_CMD_NONE; i++) {
        if (tristarSupportedCommand(device, TS_CMD_MAP[i].code)) {
            if (strlen(TS_CMD_MAP[i].name) > maxlen) maxlen = strlen(TS_CMD_MAP[i].name);
        }
    }
    sprintf(format, "%%%ds = %%s\n", maxlen);

    printf("Allowed commands for a %s\n", tristarDeviceString(device));
    for (i = 0; TS_CMD_MAP[i].code != TS_CMD_NONE; i++) {
        if (TS_CMD_MAP[i].code == TS_CMD_LIST) continue;
        if (TS_CMD_MAP[i].code == TS_CMD_DEBUG) continue;
        if (tristarSupportedCommand(device, TS_CMD_MAP[i].code)) printf(format, TS_CMD_MAP[i].name, TS_CMD_MAP[i].desc);
    }
}

int main(int argc, char **argv)
{
TS *ts;
TS_SOH soh;
int cmd = TS_CMD_NONE;
TS_OPT opt = TS_DEFAULT_OPT;
int i, status, slave = TS_SLAVE_NONE;
static char *default_server = TS_DEFAULT_SERVER;
char *server = NULL, *cmdstr = NULL;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else if (strncasecmp(argv[i], "spc=", strlen("spc=")) == 0) {
            server = argv[i] + strlen("spc=");
        } else if (strncasecmp(argv[i], "bto=", strlen("bto=")) == 0) {
            if ((opt.bto = atoi(argv[i])) <= 0) {
                fprintf(stderr, "ERROR: illegal bto '%s'\n", argv[i]);
                help(argv[0]);
            }
        } else if (strncasecmp(argv[i], "rto=", strlen("rto=")) == 0) {
            if ((opt.rto = atoi(argv[i])) <= 0) {
                fprintf(stderr, "ERROR: illegal rto '%s'\n", argv[i]);
                help(argv[0]);
            }
        } else if (strcasecmp(argv[i], "charge") == 00) {
            slave = TS_SLAVE_CHRG;
        } else if (strncasecmp(argv[i], "charge=", strlen("charge=")) == 0) {
            slave = TS_SLAVE_CHRG;
            cmdstr = argv[i] + strlen("charge=");
        } else if (strcasecmp(argv[i], "load") == 00) {
            slave = TS_SLAVE_LOAD;
        } else if (strncasecmp(argv[i], "load=", strlen("load=")) == 0) {
            slave = TS_SLAVE_LOAD;
            cmdstr = argv[i] + strlen("load=");
        } else if (strncmp(argv[i], "-debug", strlen("-debug")) == 0) {
            DebugFlag = TRUE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (server == NULL) server = strdup(default_server);

/* Check for required arguments */

    if (slave == TS_SLAVE_NONE) {
        fprintf(stderr, "ERROR: missing required slave (charge or load) argument\n");
        help(argv[0]);
    }

/* Verify command is recognized, or assign default if not specified */

    if (cmdstr == NULL) {
        cmd = TS_CMD_SOH;
    } else if ((cmd = tristarCommandCode(cmdstr)) <= 0) {
        fprintf(stderr, "ERROR: '%s' is not a valid TriStar command string\n", cmdstr);
        fprintf(stderr, "You may specify '%s=list' for a list of valid commands.\n", tristarSlaveString(slave));
        exit(1);
    }

/* Create the connection handle */

    if ((ts = tristarCreateHandle(server, slave, &opt)) == NULL) {
        perror("tristarCreateHandle");
        exit(1);
    }

/* Connect to gateway (and determine peer device type) */

    if ((status = tristarConnect(ts)) != TS_CONNECT_OK) {
        fprintf(stderr, "tristarConnect: %s\n", tristarConnectErrorString(status));
        exit(1);
    }

/* Verify command is appropriate for this device */

    if (!tristarSupportedCommand(ts->ident.device, cmd)) {
        fprintf(stderr, "ERROR: the '%s' command is not supported by this device (%s)\n", cmdstr, tristarDeviceString(ts->ident.device));
        fprintf(stderr, "You may specify '%s=list' for a list of valid commands.\n", tristarSlaveString(slave));
        exit(1);
    }

/* Execute command */

    switch (cmd) {
#ifdef notdef
      case TS_CMD_DEBUG:
        Test_MP_adc_vb_min(ts);
        break;
#endif
      case TS_CMD_LIST:
        ListSupportedCommands(ts->ident.device);
        break;
      case TS_CMD_SOH:
        tristarReadSOH(ts, &soh);
        tristarPrintSOH(stdout, &soh);
        break;
      default:
        if ((status = tristarSendCommand(ts, cmd)) != TS_COMMAND_OK) {
            fprintf(stderr, "ERROR: tristarSendCommand: %s\n", tristarCommandErrorString(status));
            exit(1);
        }
    }

    tristarDisconnect(ts);
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
 * $Log: main.c,v $
 * Revision 1.5  2016/09/01 18:02:44  dechavez
 * Fixed core dump bug when using default server
 *
 * Revision 1.4  2016/06/06 21:54:43  dechavez
 * Support for TS-MPPT devices added, simplified command line, replacing slave
 * numbers with "charge" and "load" strings, reworked report contents
 *
 * Revision 1.3  2012/05/02 18:26:04  dechavez
 * added bto (byte timeout) and rto (response timeout) options
 *
 * Revision 1.2  2012/04/25 21:07:53  dechavez
 * moved large chunks of code over to libtristar 1.0.0
 *
 */
