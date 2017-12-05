#pragma ident "$Id: config.c,v 1.16 2016/01/19 23:13:50 dechavez Exp $"
/*======================================================================
 *
 *  Configure Q330
 *
 *====================================================================*/
#include "q330cc.h"

void ConfigHelp(char *name, int argc, char **argv)
{
    printf("usage: %s sta=sname [ options ] configfile.xml\n", name);
    printf("   or: %s sta=sname [ options ] < configfile.xml\n", name);
    printf("\n");
    printf("Required\n");
    printf("sta=sname => set station name to `sname'\n");
    printf("\n");
    printf("Options\n");
    printf("sta=sname => override XML station name with `sname'\n");
    printf("net=nname => override XML network name with `nname'\n");
    printf("ip=addr   => set the ethernet IP address to dot decimal address `addr' (console only)\n");
    printf("a=inputA  => set sensor A input to complex name `inputA'\n");
    printf("b=inputB  => set sensor B input to complex name `inputA'\n");
    printf("-save     => save to EEPROM after updating configuration\n");
    printf("-boot     => save to EEPROM and reboot Q330 after updating configuration\n");
    printf("-q        => work quietly without commentary\n");
    printf("\n");
    printf("With no arguments the program expects to read XML from standard input.\n");
    printf("If not using the console, this command will NOT change the ethernet IP address.\n");
    printf("To change the IP address via the network, use the `ifconfig' command.\n");
    printf("\n");
    printf("Examples:\n");
    printf("\n");
    printf("1) q330 new00 config sta=new generic00-20090626.xml -boot\n");
    printf("   Configures digitizer \"new00\" with generic00-20090626.xml programming, setting station name to \"NEW\".\n");
    printf("   Prints a running commentary of its actions, saves to EEPROM and reboots when done.\n");
    printf("\n");
    printf("2) q330 new00 config deployed.xml -q\n");
    printf("   Configures digitizer \"new00\" to be identical to that which produced the deployed.xml file.\n");
    printf("   Works quietly and does not save or reboot when done.\n");
// This next example won't work because the src00 instance of q330 grabs UDP port 6330... need to figure
// out an easy (and transparent) way to specify an alternate return port when the first is busy
//    printf("\n");
//    printf("3) To directly clone \"new00\" from an on-line digitizer \"src00\", without intermediate XML files\n");
//    printf("   q330 src00 xml | q330 new00 config sta=new\n");
}

BOOL VerifyConfig(Q330 *q330)
{
int i;
char *arg;
BOOL console;
char *sta = NULL;

    q330->cmd.code = Q330_CMD_CONFIG;
    q330->cmd.config.sname = NULL;
    q330->cmd.config.nname = NULL;
    q330->cmd.config.xname = NULL;
    q330->cmd.config.save = FALSE;
    q330->cmd.config.warn = FALSE;
    q330->cmd.config.boot = FALSE;
    q330->cmd.config.verbose = TRUE;
    q330->cmd.config.sensor.a = NULL;
    q330->cmd.config.sensor.b = NULL;
    q330->cmd.config.ip = 0;

    console = (q330->addr.connect.type == QIO_UDP) ? FALSE : TRUE;

    for (i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];
        if (strncmp(arg, "sta=", strlen("sta=")) == 0) {
            sta = arg + strlen("sta=");
            util_ucase(sta);
        } else if (strncmp(arg, "net=", strlen("net=")) == 0) {
            q330->cmd.config.nname = arg + strlen("net=");
        } else if (strncmp(arg, "b=", strlen("b=")) == 0) {
            q330->cmd.config.sensor.b = arg + strlen("b=");
        } else if (strncmp(arg, "a=", strlen("a=")) == 0) {
            q330->cmd.config.sensor.a = arg + strlen("a=");
        } else if (console && strncmp(arg, "ip=", strlen("ip=")) == 0) {
            q330->cmd.config.ip = htonl(inet_addr(arg + strlen("ip=")));
        } else if (strcmp(arg, "-save") == 0) {
            q330->cmd.config.save = TRUE;
        } else if (strcmp(arg, "-warn") == 0) {
            q330->cmd.config.warn = TRUE;
        } else if (strcmp(arg, "-boot") == 0) {
            q330->cmd.config.save = TRUE;
            q330->cmd.config.boot = TRUE;
        } else if (strcmp(arg, "-q") == 0) {
            q330->cmd.config.verbose = FALSE;
        } else if (q330->cmd.config.xname == NULL) {
            q330->cmd.config.xname = arg;
        } else {
            return UnexpectedArg(q330, arg);
        }
    }

    if (sta == NULL) {
        printf("ERROR: station name must be specified with the sta=name argument\n");
        return FALSE;
    }

    q330->cmd.config.sname = sta;
    return TRUE;
}

static Q330_SENSOR *LookupSensor(char *complex, Q330_CFG *cfg)
{
int errcode;
Q330_INPUT input;

    if (!q330ParseComplexInputName(&input, complex, &errcode)) return NULL;
    return q330LookupSensor(input.sensor.name, cfg);
}

void config(Q330 *q330)
{
int i;
QDP_TYPE_FULL_CONFIG config;
#define TEST_SNAME "TST"
struct {
    Q330_SENSOR *a;
    Q330_SENSOR *b;
} sensor;

    if (q330->cmd.config.sensor.a == NULL) {
        sensor.a = &q330->addr.input.a.sensor;
    } else if ((sensor.a = LookupSensor(q330->cmd.config.sensor.a, q330->cfg)) == NULL) {
        printf("ERROR: sensor A `%s' is not recognized\n", q330->cmd.config.sensor.a);
    }

    if (q330->cmd.config.sensor.b == NULL) {
        sensor.b = &q330->addr.input.b.sensor;
    } else if ((sensor.b = LookupSensor(q330->cmd.config.sensor.b, q330->cfg)) == NULL) {
        printf("ERROR: sensor B `%s' is not recognized\n", q330->cmd.config.sensor.b);
    }

    if (sensor.a == NULL || sensor.b == NULL) {
        printf("Invalid sensor name(2).\n");
        GracefulExit(q330, 1);
    }

    if (!qdpXmlConfigFromFile(q330->cmd.config.xname, &config, stdout)) {
        perror("Unable to read XML configuration");
        GracefulExit(q330, 1);
    }

    if (!qdp_C1_RQFIX(q330->qdp, &config.fix)) {
        perror("qdp_C1_RQFIX failed");
        GracefulExit(q330, 1);
    }

    if (q330->cmd.config.sname != NULL) {
        for (i = 0; i < QDP_NLP; i++) {
            if (strncasecmp(config.dport[i].token.site.sname, TEST_SNAME, strlen(TEST_SNAME)) != 0) {
                strlcpy(config.dport[i].token.site.sname, q330->cmd.config.sname, QDP_SNAME_LEN);
                config.dport[i].token.site.valid = TRUE;
            }
        }
    }

    if (q330->cmd.config.nname != NULL) {
        for (i = 0; i < QDP_NLP; i++) {
            strlcpy(config.dport[i].token.site.nname, q330->cmd.config.nname, QDP_NNAME_LEN);
            config.dport[i].token.site.valid = TRUE;
        }
    }

    config.phy.ethernet.ip = q330->cmd.config.ip;

    if (q330->cmd.config.verbose) printf("Reconfiguring %s (KMI tag %u, S/N %016llX)\n", q330->addr.connect.ident, config.fix.proper_tag, q330->addr.serialno);

    if (!qdpSetConfig(q330->qdp, &config, q330->cmd.config.verbose)) {
        perror("qdpSetConfig");
        GracefulExit(q330, 1);
    }
    if (q330->cmd.config.verbose) printf("Setting sensor control lines for a=%s, b=%s\n", sensor.a->name, sensor.b->name);
    if (!ConfigSensorControl(q330, sensor.a, sensor.b)) return;

    if (q330->cmd.config.save) {
        if (q330->cmd.config.verbose) printf("Saving configuration to EEPROM\n");
        sleep(10);
        if (!qdpCtrl(q330->qdp, QDP_CTRL_SAVE, TRUE)) {
            printf("save to eeprom command failed\n");
            GracefulExit(q330, 1);
        }
    } else if (q330->cmd.config.warn) {
        printf("\n");
        printf("************************************************************\n");
        printf("*** configure will not persist across reboot until saved ***\n");
        printf("************************************************************\n");
    }

    if (q330->cmd.config.boot) {
        if (q330->cmd.config.verbose) printf("Rebooting digitizer\n");
        sleep(10);
        boot(q330);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
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
 * $Log: config.c,v $
 * Revision 1.16  2016/01/19 23:13:50  dechavez
 * added a blank line somewhere up there for anal purposes
 *
 * Revision 1.15  2015/12/05 00:26:56  dechavez
 * fixed benign return FALSE instead of NULL bug in LookupSensor()
 *
 * Revision 1.14  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.13  2012/08/31 18:30:37  dechavez
 * added -warn option to print unsaved config warning (ie, suppress warning)
 *
 * Revision 1.12  2012/06/24 18:25:46  dechavez
 * use new configuration dport field instead of decoupled log and token fields
 *
 * Revision 1.11  2011/12/20 22:28:52  dechavez
 * fixed bug setting station name
 *
 * Revision 1.10  2011/10/13 18:21:12  dechavez
 * no longer save and boot when finished, unless -save and -boot arguments explictly given
 *
 * Revision 1.9  2011/02/04 20:04:18  dechavez
 * added options to override sensor A and B control maps
 *
 * Revision 1.8  2011/02/01 20:26:04  dechavez
 * allow override of ethernet IP address (console connections only)
 *
 * Revision 1.7  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.6  2010/12/23 21:55:47  dechavez
 * use QDP_NLP intstead of QDP_MAX_PORT which was causing overflow problems
 * following reorg of qdp.h symbolic names
 *
 * Revision 1.5  2010/12/17 19:45:00  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 *
 * Revision 1.4  2010/06/14 19:26:51  dechavez
 * removed unused variables
 *
 * Revision 1.3  2009/11/13 00:42:21  dechavez
 * Don't override token station name if it is TST.
 *
 * Revision 1.2  2009/11/05 18:49:17  dechavez
 * improved help message, added options to override site info and to automatically save and reboot
 *
 * Revision 1.1  2009/10/29 19:37:11  dechavez
 * initial release
 *
 */
