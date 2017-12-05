#pragma "$Id: cmd.c,v 1.1 2016/06/06 21:51:36 dechavez Exp $"
/*======================================================================
 *
 * Command utilities
 *
 *====================================================================*/
#define TS_INCLUDE_STATIC_CMD_MAP
#define TS_INCLUDE_STATIC_COIL_MAPS
#include "tristar.h"

static UINT16 LookupCoil(int cmd, TS_COIL_MAP *map)
{
int i;

    for (i = 0; map[i].cmd != TS_CMD_NONE; i++) if (cmd == map[i].cmd) return map[i].coil;
    return TS_COIL_UNSUPPORTED;
}

int tristarSendCommand(TS *handle, int cmd)
{
UINT16 coil;
TS_COIL_MAP *tsch = TS_COIL_MAP_TSCH;
TS_COIL_MAP *tsld = TS_COIL_MAP_TSLD;
TS_COIL_MAP *mppt = TS_COIL_MAP_MPPT;

    if (handle == NULL) return TS_COMMAND_EINVAL;

    switch (handle->ident.device) {
      case TS_DEVICE_TSCH: coil = LookupCoil(cmd, tsch); break;
      case TS_DEVICE_TSLD: coil = LookupCoil(cmd, tsld); break;
      case TS_DEVICE_MPPT: coil = LookupCoil(cmd, mppt); break;
      default:             return TS_COMMAND_UNKNOWN_DEVICE;
    }

    if (coil == TS_COIL_UNSUPPORTED) return TS_COMMAND_UNSUPPORTED;

    if (modbus_write_bit(handle->ctx, coil, TRUE) != 1) return TS_COMMAND_FAIL;

    return TS_COMMAND_OK;
}

int tristarCommandCode(char *string)
{
int i;

    if (string == NULL) return TS_DEFAULT_CMD;
    for (i = 0; TS_CMD_MAP[i].code != TS_CMD_NONE; i++) if (strcasecmp(string, TS_CMD_MAP[i].name) == 0) return TS_CMD_MAP[i].code;

    return TS_CMD_ILLEGAL;
}

BOOL tristarSupportedCommand(int device, int cmd)
{
int i;

    for (i = 0; TS_CMD_MAP[i].code != TS_CMD_NONE; i++) if (TS_CMD_MAP[i].code == cmd) return (TS_CMD_MAP[i].device & device) ? TRUE : FALSE;

    return FALSE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: cmd.c,v $
 * Revision 1.1  2016/06/06 21:51:36  dechavez
 * initial release
 *
 */
