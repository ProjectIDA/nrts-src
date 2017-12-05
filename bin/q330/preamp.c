#pragma ident "$Id: preamp.c,v 1.1 2017/09/13 23:42:37 dauerbach Exp $"
/*======================================================================
 *
 *  preamp control
 *
 *====================================================================*/
#include "q330cc.h"

#define MUXA    1
#define MUXB    2
#define MUXNONE 3

int *chans = NULL, value;
UINT16 mask = 0;

void PreampHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ {a|b}={on|off} ]\n", name);
}

static BOOL BadArg(char *arg)
{
    printf("unrecognized or malformed argument or value: %s\n\n", arg);
    return FALSE;
}

static BOOL ParsePreamp(char *arg, UINT16 *gain_map)
{
UINT16 mux;
int i, shift, ntoken;
static int a[3] = { 1, 2, 3 };
static int b[3] = { 4, 5, 6 };
#define MAXTOKEN 2
char *token[MAXTOKEN];
static char delimiters[] = "=";
#define AMASK 0x003F /* 000000000 00111111 */
#define BMASK 0x0FC0 /* 000001111 11000000 */

    /* check for bad number of args, or badly formed args
     *
     * If args valid, sets chans to be changed, mask to use when setting bitmask,
     * and value to set each channel to.
     *
     */
    if ((ntoken = utilParse(arg, token, delimiters, MAXTOKEN, 0)) != 2) return BadArg(arg);

    if (strcasecmp(token[0], "a") == 0) {
        chans = a;
        mask = ~AMASK;
    } else if (strcasecmp(token[0], "b") == 0) {
        chans = b;
        mask = ~BMASK;
    } else {
        return BadArg(arg);
    }

    if (strcasecmp(token[1], "disabled") == 0) {
        value = QDP_PREAMP_CHANDISABLED;
    } else if (strcasecmp(token[1], "on") == 0) {
        value = QDP_PREAMP_ON;
    } else if (strcasecmp(token[1], "off") == 0) {
        value = QDP_PREAMP_OFF;
    } else {
        return BadArg(arg);
    }

    return TRUE;
}

static BOOL SetPreamp(Q330 *q330, char *arg)
{

    /* help requested */

    if (strcasecmp(arg, "help") == 0) {
        PreampHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    /* parse args */

    if (!ParsePreamp(arg, &q330->cmd.gain_map)) {
        PreampHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    q330->cmd.update = TRUE;
    return TRUE;
}

BOOL VerifyPreamp(Q330 *q330)
{
    q330->cmd.update = FALSE;
    q330->cmd.gain_map = 0;
    q330->cmd.code = Q330_CMD_PREAMP;

    switch (q330->cmd.arglst->count) {
      case 0:
        return TRUE;
      case 1:
        return SetPreamp(q330, (char *) q330->cmd.arglst->array[0]);
    }

    PreampHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void preamp(Q330 *q330)
{
int i, chan, shift;
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;
UINT16 new_map = 0;

/* Get global data (gain_map lives there) */

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

/* Print current values */

    if (q330->cmd.update) printf("was: ");
    qdpPrintPreampBitmap(stdout, glob.gain_map);

/* All done if we aren't changing anything */

    if (!q330->cmd.update) return;

/* construct the new bitmask */

    for (i = 0; i < 3; i++) {
        chan = chans[i];
        shift = (chan - 1) * 2;
        new_map |= (value << shift);
    }
    /* use mask to clear out channels to be set */
    /* then OR with new values for those cahnnels */
    glob.gain_map = (glob.gain_map & mask) | new_map;

/* Change gain gain_map */

    qdpEncode_C1_GLOB(&pkt, &glob);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Global Programming command failed\n");
        return;
    }

/* Retrieve the freshly installed values, and print them out */

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

    printf("now: ");
    qdpPrintPreampBitmap(stdout, glob.gain_map);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: preamp.c,v $
 * Revision 1.1  2017/09/13 23:42:37  dauerbach
 * Implement q330 preamp command to turn 'on' or 'off' preamp for sensor 'a' or 'b'
 *
 */
