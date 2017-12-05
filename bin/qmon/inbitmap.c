#pragma ident "$Id: inbitmap.c,v 1.4 2017/07/20 17:26:55 dechavez Exp $"
/*======================================================================
 *
 *  Manage input bitmap (channel and pre-amp enable/disable)
 *
 *====================================================================*/
#include "qmon.h"

#define MUXA    1
#define MUXB    2
#define MUXNONE 3
     
static int WorkingMux = 0;
static UINT16 save;

static void HelpMesg(QMON *qmon)
{
    sprintf(qmon->poptxt, "usage: input  {123}=signal {456}=signal | normal\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "valid values for `signal' are\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "      %s - 20 sec square wave synced to UTC\n", qdpInputMuxString2(QDP_INPUT_MUX_1PPS));
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "      %s - Reference Input\n",   qdpInputMuxString2(QDP_INPUT_MUX_REFERENCE));
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "      %s - Calibrator Output\n", qdpInputMuxString2(QDP_INPUT_MUX_CALIBRATOR));
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "      %s - Grounded Input\n",    qdpInputMuxString2(QDP_INPUT_MUX_GROUNDED));
    sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "currently:\n");
    sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpInputBitmapString(save, NULL));

    PopupMessage(qmon, qmon->poptxt);
}

static BOOL SetBitmapChan(QMON *qmon, UINT16 *bitmap, UINT16 bit, int mux)
{
    if (WorkingMux == 0 || WorkingMux == mux) {
        *bitmap |= bit;
        WorkingMux = mux;
        return TRUE;
    }

    if (WorkingMux == MUXNONE) {
        PopupMessage(qmon, "ERROR: can't mix 'normal' with other inputs");
    } else {
        PopupMessage(qmon, "ERROR: can't mix A (123) and B (456) inputs");
    }
    return FALSE;
}

static BOOL ParseInput(QMON *qmon, char *arg, UINT16 *bitmap)
{
UINT16 mux;
int i, shift;

    if (strcmp(arg, "normal") == 0) {
        if (*bitmap != 0) {
            PopupMessage(qmon, "ERROR: can't mix 'normal' with other inputs");
            return FALSE;
        }
        WorkingMux = MUXNONE;
        return TRUE;
    } else {
        WorkingMux = 0;
    }

    for (i = 0; i < strlen(arg); i++) {
        switch (arg[i]) {
          case '1': if (!SetBitmapChan(qmon, bitmap, 1 << 0,  MUXA)) return FALSE;
            break;
          case '2': if (!SetBitmapChan(qmon, bitmap, 1 << 1,  MUXA)) return FALSE;
            break;
          case '3': if (!SetBitmapChan(qmon, bitmap, 1 << 2,  MUXA)) return FALSE;
            break;
          case '4': if (!SetBitmapChan(qmon, bitmap, 1 << 3,  MUXB)) return FALSE;
            break;
          case '5': if (!SetBitmapChan(qmon, bitmap, 1 << 4,  MUXB)) return FALSE;
            break;
          case '6': if (!SetBitmapChan(qmon, bitmap, 1 << 5,  MUXB)) return FALSE;
            break;
          case '=':
            if ((mux = qdpInputMuxCode(arg+i+1)) == QDP_INPUT_MUX_UNDEFINED) {
                HelpMesg(qmon);
                return FALSE;
            }
            shift = (WorkingMux == MUXA) ? QDP_INPUT_MUX_A_SHIFT : QDP_INPUT_MUX_B_SHIFT;
            *bitmap |= (mux << shift);
            return TRUE;
          default:
            HelpMesg(qmon);
            return FALSE;
        }
    }

    HelpMesg(qmon);
    return FALSE;
}

static BOOL SetInput(QMON *qmon, char *arg, UINT16 *bitmap)
{
int type;
char *want;

    if (strcasecmp(arg, "help") == 0) {
        HelpMesg(qmon);
        return FALSE;
    }

    if (!ParseInput(qmon, arg, bitmap)) return FALSE;
     
    return TRUE;
}

void ExecuteInputBitmap(QMON *qmon, int argc, char **argv)
{
int port;
UINT16 new = 0;
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;
QDP_TYPE_MONITOR *mon;

    save = qmon->active->mon.glob.input_map;

    switch (argc) {
      case 2:
        if (!SetInput(qmon, argv[1], &new)) return;
        break;
      case 3:
        if (!SetInput(qmon, argv[1], &new)) return;
        if (!SetInput(qmon, argv[2], &new)) return;
        break;
      default:
        HelpMesg(qmon);
        return;
    }

    if (new == save) {
        sprintf(qmon->poptxt, "input bitmap unchanged\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "----------------------\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpInputBitmapString(save, NULL));
        PopupMessage(qmon, qmon->poptxt);
        return;
    }

    qmon->active->mon.glob.input_map = new;
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
        sprintf(qmon->poptxt,                      "input bitmap modified (settings will auto-clear in 30 minutes)\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "--------------------------------------------------------------\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt),"was:\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpInputBitmapString(save, NULL));
        sprintf(qmon->poptxt+strlen(qmon->poptxt), " \n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "now:\n");
        sprintf(qmon->poptxt+strlen(qmon->poptxt), "%s\n", qdpInputBitmapString(qmon->active->mon.glob.input_map, NULL));
    } else {
        sprintf(qmon->poptxt, "input bitmap reset to normal\n");
    }
    PopupMessage(qmon, qmon->poptxt);
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
 * $Log: inbitmap.c,v $
 * Revision 1.4  2017/07/20 17:26:55  dechavez
 * Changed descriptive comment to reflect what this module actually does
 *
 * Revision 1.3  2015/12/07 19:21:52  dechavez
 * explicity typed WorkingMux to int (benign oversight)
 *
 * Revision 1.2  2011/07/27 23:50:02  dechavez
 * don't set save/reboot flags on bitmap changes, print auto-clear reminder
 *
 * Revision 1.1  2011/01/03 21:31:05  dechavez
 * created
 *
 */
