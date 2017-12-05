#pragma ident "$Id: preamp.c,v 1.1 2017/09/20 19:30:23 dauerbach Exp $"
/*======================================================================
 *
 *  Manage preamp settings
 *
 *====================================================================*/
#include "qmon.h"

static int *chans = NULL, value;
static UINT16 mask = 0;
static UINT16 save = 0;

static void HelpMesg(QMON *qmon)
{
    sprintf(qmon->poptxt, "usage: preamp {a|b}={on|off}\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "currently:\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpPreampBitmapString(save, NULL));

    PopupMessage(qmon, qmon->poptxt);
}

static BOOL ParsePreamp(QMON *qmon, char *arg, UINT16 *bitmap)
{
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
    if ((ntoken = utilParse(arg, token, delimiters, MAXTOKEN, 0)) != 2) {
        HelpMesg(qmon);
        return FALSE;
    }

    if (strcasecmp(token[0], "a") == 0) {
        chans = a;
        mask = ~AMASK;
    } else if (strcasecmp(token[0], "b") == 0) {
        chans = b;
        mask = ~BMASK;
    } else {
        HelpMesg(qmon);
        return FALSE;
    }

    if (strcasecmp(token[1], "disabled") == 0) {
        value = QDP_PREAMP_CHANDISABLED;
    } else if (strcasecmp(token[1], "on") == 0) {
        value = QDP_PREAMP_ON;
    } else if (strcasecmp(token[1], "off") == 0) {
        value = QDP_PREAMP_OFF;
    } else {
        HelpMesg(qmon);
        return FALSE;
    }

    return TRUE;
}

static BOOL SetPreamp(QMON *qmon, char *arg, UINT16 *bitmap)
{

    if (strcasecmp(arg, "help") == 0) {
        HelpMesg(qmon);
        return FALSE;
    }

    if (!ParsePreamp(qmon, arg, bitmap)) return FALSE;

    return TRUE;
}

void ExecutePreamp(QMON *qmon, int argc, char **argv)
{
int i, chan, shift;
int port;
UINT16 new = 0;
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;
QDP_TYPE_MONITOR *mon;

    save = qmon->active->mon.glob.gain_map;

    switch (argc) {
      case 2:
        if (!SetPreamp(qmon, argv[1], &new)) return;
        break;
      default:
        HelpMesg(qmon);
        return;
    }

    if (new == save) {
        sprintf(qmon->poptxt, "preamps unchanged\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "----------------------\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpPreampBitmapString(save, NULL));
        PopupMessage(qmon, qmon->poptxt);
        return;
    }

    /* construct the new bitmask */

    for (i = 0; i < 3; i++) {
        chan = chans[i];
        shift = (chan - 1) * 2;
        new |= (value << shift);
    }

    /* use mask to clear out channels to be set */
    /* then OR with new values for those cahnnels */

    qmon->active->mon.glob.gain_map = (qmon->active->mon.glob.gain_map & mask) | new;

    qdpEncode_C1_GLOB(&pkt, &qmon->active->mon.glob);
    if (!qdpPostCmd(qmon->qdp, &pkt, TRUE)) {
        PopupMessage(qmon, "ERROR: unable to set global programming");
        return;
    }

    if (!qdp_C1_RQGLOB(qmon->qdp, &qmon->active->mon.glob)) {
        PopupMessage(qmon, "ERROR: unable to retrieve global programming");
        return;
    }

    if (new != 0) {
        sprintf(qmon->poptxt,                      "preamp modified\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "---------------\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt),"was:\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpPreampBitmapString(save, NULL));
        sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "now:\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpPreampBitmapString(qmon->active->mon.glob.gain_map, NULL));
    }
    PopupMessage(qmon, qmon->poptxt);
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
 * Revision 1.1  2017/09/20 19:30:23  dauerbach
 * initial release
 *
 */
