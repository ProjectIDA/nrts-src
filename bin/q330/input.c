#pragma ident "$Id: input.c,v 1.3 2014/08/11 19:20:26 dechavez Exp $"
/*======================================================================
 *
 *  Input control
 *
 *====================================================================*/
#include "q330cc.h"

#define MUXA    1
#define MUXB    2
#define MUXNONE 3

static int WorkingMux = 0;

void InputHelp(char *name, int argc, char **argv)
{
int i;
       
    printf("usage: %s {123}=signal {456}=signal | normal\n", name);
    printf("signal should be one of:");
    printf(" %s", qdpInputMuxString2(QDP_INPUT_MUX_REFERENCE));
    printf(" %s", qdpInputMuxString2(QDP_INPUT_MUX_GROUNDED));
    printf(" %s", qdpInputMuxString2(QDP_INPUT_MUX_1PPS));
    printf(" %s", qdpInputMuxString2(QDP_INPUT_MUX_CALIBRATOR));
    printf("\n");
}

static BOOL SetBitmapChan(UINT16 *bitmap, UINT16 bit, int mux)
{
    if (WorkingMux == 0 || WorkingMux == mux) {
        *bitmap |= bit;
        WorkingMux = mux;
        return TRUE;
    }

    if (WorkingMux == MUXNONE) {
        printf("ERROR: can't mix 'normal' with other inputs\n");
    } else {
        printf("ERROR: can't mix A (123) and B (456) inputs\n");
    }
    return FALSE;
}

static BOOL BadArg(char *arg)
{
    printf("unrecognized or malformed argument: %s\n", arg);
    return FALSE;
}

static BOOL ParseInput(char *arg, UINT16 *bitmap)
{
UINT16 mux;
int i, shift;

    if (strcmp(arg, "normal") == 0) {
        if (*bitmap != 0) {
            printf("ERROR: can't mix 'normal' with other inputs\n");
            return FALSE;
        }
        WorkingMux = MUXNONE;
        bitmap = 0;
        return TRUE;
    } else {
        WorkingMux = 0;
    }

    for (i = 0; i < strlen(arg); i++) {
        switch (arg[i]) {
          case '1': if (!SetBitmapChan(bitmap, 1 << 0,  MUXA)) return BadArg(arg);
            break;
          case '2': if (!SetBitmapChan(bitmap, 1 << 1,  MUXA)) return BadArg(arg);
            break;
          case '3': if (!SetBitmapChan(bitmap, 1 << 2,  MUXA)) return BadArg(arg);
            break;
          case '4': if (!SetBitmapChan(bitmap, 1 << 3,  MUXB)) return BadArg(arg);
            break;
          case '5': if (!SetBitmapChan(bitmap, 1 << 4,  MUXB)) return BadArg(arg);
            break;
          case '6': if (!SetBitmapChan(bitmap, 1 << 5,  MUXB)) return BadArg(arg);
            break;
          case '=':
            if ((mux = qdpInputMuxCode(arg+i+1)) == QDP_INPUT_MUX_UNDEFINED) return BadArg(arg);
            shift = (WorkingMux == MUXA) ? QDP_INPUT_MUX_A_SHIFT : QDP_INPUT_MUX_B_SHIFT;
            *bitmap |= (mux << shift);
            return TRUE;
          default:
            return BadArg(arg);
        }
    }
    return BadArg(arg);
}

static BOOL SetInput(Q330 *q330, char *arg)
{
int type;
char *want;

    if (strcasecmp(arg, "help") == 0) {
        InputHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }

    if (!ParseInput(arg, &q330->cmd.input_map)) return FALSE;

    q330->cmd.update = TRUE;
    return TRUE;
}

BOOL VerifyInput(Q330 *q330)
{
    q330->cmd.update = FALSE;
    q330->cmd.input_map = 0;
    q330->cmd.code = Q330_CMD_INPUT;

    switch (q330->cmd.arglst->count) {
      case 0:
        return TRUE;
      case 1:
        return SetInput(q330, (char *) q330->cmd.arglst->array[0]);
      case 2:
        if (SetInput(q330, (char *) q330->cmd.arglst->array[0])) {
            return SetInput(q330, (char *) q330->cmd.arglst->array[1]);
        }
        break;
    }

    InputHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void input(Q330 *q330)
{
int i;
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;

/* Get global data (input bitmap lives there) */

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

/* Print current values */

    if (q330->cmd.update) printf("was: ");
    qdpPrintInputBitmap(stdout, glob.input_map);

/* All done if we aren't changing anything */

    if (!q330->cmd.update) return;

/* Change input bitmap */

    glob.input_map = q330->cmd.input_map;
    qdpEncode_C1_GLOB(&pkt, &glob);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Global Programming command failed\n");
        return;
    }

/* Retrieve the freshly installed values, and print them out */

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

    printf("now: ");
    qdpPrintInputBitmap(stdout, glob.input_map);
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
 * $Log: input.c,v $
 * Revision 1.3  2014/08/11 19:20:26  dechavez
 * changed WorkingMux variable from nothing (how did that work) to int
 *
 * Revision 1.2  2011/01/03 21:29:19  dechavez
 * moved some input bitmap related stuff to libqdp as qdpInputMuxCode(), qdpInputMuxString2()
 *
 * Revision 1.1  2010/04/07 19:49:51  dechavez
 * created
 *
 */
