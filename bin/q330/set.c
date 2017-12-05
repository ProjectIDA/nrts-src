#pragma ident "$Id: set.c,v 1.12 2014/09/24 21:25:02 dechavez Exp $"
/*======================================================================
 *
 *  Add (or replace) a digitizer in the configuration file
 *
 *====================================================================*/
#include "q330cc.h"

static void SetHelp()
{
    printf("usage: set ident tag [ sensorA sensorB ] [ serialno ]\n");
    printf("\n");
    printf("   ident - digitizer identifier (eg, sta00)\n");
    printf("     tag - external property tag\n");
    printf("\n");
    printf("Optional:\n");
    printf(" sensorA - complex sensor name, input A\n");
    printf(" sensorB - complex sensor name, input B\n");
    printf("serialno - 16 digit hex internal serial number\n");
    printf("\n");
    printf("Notes: The 'ident' must resolve to an IP address (eg, have a /etc/hosts entry)\n");
    printf("       The serial number is required if `ident' is previously unknown\n");
    printf("\n");
}

void SetDigitizer(Q330 *q330, int argc, char **argv)
{
int i, errcode, errors = 0;
Q330_CFG *cfg;
Q330_AUTH auth;
Q330_ADDR addr, *test, *crnt;
BOOL HaveSensors, HaveSerialNo;
char *sensorA, *sensorB;
static char *NONE = "none";

    switch (argc) {
      case 4:
        HaveSensors = FALSE;
        HaveSerialNo = FALSE;
        break;

      case 5:
        HaveSensors = FALSE;
        HaveSerialNo = TRUE;
        break;

      case 6:
        HaveSensors = TRUE;
        HaveSerialNo = FALSE;
        break;

      case 7:
        HaveSensors = TRUE;
        HaveSerialNo = TRUE;
        break;

      default:
        SetHelp();
        return;
    }

    strlcpy(addr.connect.ident, argv[2], ISI_STALEN+1);
    addr.connect.type = QIO_UDP;
    if ((addr.connect.tag = (UINT32) atol(argv[3])) == 0) {
        printf("*** ERROR *** invalid tag number '%s'\n", argv[3]);
        return;
    }

    crnt = q330LookupAddr(addr.connect.ident, q330->cfg); /* in case we already have it */

    if (HaveSensors) {
        sensorA = (strncasecmp(argv[4], "a=", strlen("a=")) == 0) ? argv[4] + strlen("a=") : argv[4];
        if (!q330ParseComplexInputName(&addr.input.a, sensorA, &errcode)) {
            return;
        }
        sensorB = (strncasecmp(argv[5], "b=", strlen("b=")) == 0) ? argv[5] + strlen("b=") : argv[5];
        if (!q330ParseComplexInputName(&addr.input.b, sensorB, &errcode)) {
            printf("*** ERROR *** q330ParseComplexInputName('%s') failed\n", argv[5]);
            return;
        }
    } else {
        if (crnt != NULL) {
            addr.input.a = crnt->input.a;
            addr.input.b = crnt->input.b;
        } else {
            if (!q330ParseComplexInputName(&addr.input.a, NONE, &errcode)) {
                printf("*** ERROR *** q330ParseComplexInputName('%s') failed\n", NONE);
                return;
            }
            if (!q330ParseComplexInputName(&addr.input.b, NONE, &errcode)) {
                printf("*** ERROR *** q330ParseComplexInputName('%s') failed\n", NONE);
                return;
            }
        }
    }

    if (q330LookupSensor(addr.input.a.sensor.name, q330->cfg) == NULL) {
        printf("*** ERROR *** '%s' is not a recognized sensor\n", addr.input.a.sensor.name);
        printf("Enter the command `q330 sensor' to list valid sensor names\n");
        return;
    }

    if (q330LookupSensor(addr.input.b.sensor.name, q330->cfg) == NULL) {
        printf("*** ERROR *** '%s' is not a recognized sensor\n", addr.input.b.sensor.name);
        printf("Enter the command `q330 sensor' to list valid sensor names\n");
        return;
    }

    if (!q330AddAddr(q330->cfg, &addr)) {
        printf("*** ERROR *** Failed to add '%s' to address list\n", addr.connect.ident);
        return;
    }

    if (!q330SaveAddr(NULL, q330->cfg)) {
        printf("*** ERROR *** %s: %s\n", q330->cfg->path.addr, strerror(errno));
        return;
    } else {
        printf("%s updated\n", q330->cfg->path.addr);
    }

    if (HaveSerialNo) {
        auth.tag = addr.connect.tag;
        auth.serialno = (UINT64) strtoll(HaveSensors ? argv[6] : argv[4], NULL, 16);
        for (i = 0; i < QDP_NUM_PORT; i++) auth.code[i] = 0;
        if (!q330AddAuth(q330->cfg, &auth)) {
            printf("*** ERROR *** q330AddAuth failed: %s\n", strerror(errno));
            ++errors;
        }
        if (!q330SaveAuth(NULL, q330->cfg)) {
            printf("*** ERROR *** %s: %s\n", q330->cfg->path.auth, strerror(errno));
            return;
        } else {
            printf("%s updated\n", q330->cfg->path.auth);
        }
    }

    if ((cfg = q330ReadCfg(q330->cfg->root, &errcode)) == NULL) {
        q330PrintErrcode(stdout, "ERROR: cannot reload configuration: ", q330->cfg->root, errcode);
        return;
    }

    if ((test = q330LookupAddr(argv[2], cfg)) == NULL) {
        printf("*** ERROR *** Failed to recover '%s' from address list\n", argv[2]);
        return;
    }

    if (test->serialno == QDP_INVALID_SERIALNO) {
        printf("ERROR: no serial number found for digitizer `%d'.\n", test->connect.tag);
        printf("Edit '%s' or use \"q330 set\" and include the 16-digit (internal) serial number.\n", q330->cfg->path.auth);
        return;
    }

    q330PrintAddr(stdout, &addr);
    if (HaveSerialNo) q330PrintAuth(stdout, &auth);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: set.c,v $
 * Revision 1.12  2014/09/24 21:25:02  dechavez
 * added code to check for illegal tag fields and possible a= or b= prefixes to sensor names
 *
 * Revision 1.11  2014/09/08 18:35:29  dechavez
 * Improved messages when "q330 set" runs into problems
 *
 * Revision 1.10  2014/01/24 00:03:14  dechavez
 * don't exit in case of errors (causes subsequent I/O to hang for 5 minutes)
 *
 * Revision 1.9  2011/01/26 22:21:31  dechavez
 * fixed incomplete addr error
 *
 * Revision 1.8  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.7  2011/01/07 22:54:48  dechavez
 * Don't allow q330 set to insert unrecognized sensors into q330.cfg
 *
 * Revision 1.6  2011/01/07 16:37:35  dechavez
 * complain if tag number lacks matching serial number
 *
 * Revision 1.5  2010/12/21 20:03:12  dechavez
 * support auth updates, made sensors optional
 *
 * Revision 1.4  2010/12/17 19:44:27  dechavez
 * support libq330 2.0.0 Q330 configuration database and structure formats
 * Use property tag instead of internal serial number
 *
 * Revision 1.3  2010/11/24 19:19:32  dechavez
 * set sensors to "none"
 *
 * Revision 1.2  2009/07/25 17:37:18  dechavez
 * initial working release
 *
 * Revision 1.1  2009/07/23 20:30:17  dechavez
 * initial stub
 *
 */
