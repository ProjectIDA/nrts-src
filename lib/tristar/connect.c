#pragma ident "$Id: connect.c,v 1.1 2016/06/06 21:51:36 dechavez Exp $"
/*======================================================================
 *
 *  Connect to a MODBUS TCP server and do the library handshake
 *
 *====================================================================*/
#define TS_INCLUDE_STATIC_TS_DESCRIPTOR
#include "tristar.h"
#include "private.h"
#include "util.h"

#define NUM_SN_ADDR 4

static BOOL ReadSerialNo(TS *handle, TS_DESC *desc, char *serialno)
{
int i;
char *ptr;
TS_ITEM item;

    ptr = serialno;
    for (i = 0; i < NUM_SN_ADDR; i++) {
        if (tristarReadItem(handle, &desc[i], &item) != TS_READ_OK) return FALSE;
        *ptr++ = (item.raw & 0x000000ff);
        *ptr++ = (item.raw & 0x0000ff00) >> 8;
    }
    serialno[TS_SERIALNO_LEN] = 0;

    return TRUE;
}

static REAL64 ComputeScaleFactor(INT32 hi, INT32 lo)
{
REAL64 whole, fraction, result;

    whole    = (REAL64) hi;
    fraction = (REAL64) lo / (REAL64) 65536;
    result   = whole + fraction;

    return result;
}

static BOOL SetScaleFactors(TS *handle)
{
TS_ITEM hi, lo;
static char *fid = "tristarConnect:SetScaleFactors";

    if (tristarReadItem(handle, &TS_DESCRIPTOR.MP_V_PU_hi, &hi) != TS_READ_OK) return FALSE;
    if (tristarReadItem(handle, &TS_DESCRIPTOR.MP_V_PU_lo, &lo) != TS_READ_OK) return FALSE;
    handle->V_PU = ComputeScaleFactor(hi.raw, lo.raw);

    if (tristarReadItem(handle, &TS_DESCRIPTOR.MP_I_PU_hi, &hi) != TS_READ_OK) return FALSE;
    if (tristarReadItem(handle, &TS_DESCRIPTOR.MP_I_PU_lo, &lo) != TS_READ_OK) return FALSE;
    handle->I_PU = ComputeScaleFactor(hi.raw, lo.raw);

    return TRUE;
}

static int ModelHandshake(TS *handle)
{
char serialno[TS_SERIALNO_LEN+1];
static TS_DESC TS_serialno_desc[NUM_SN_ADDR] = {TS_DESC_TS_serial01, TS_DESC_TS_serial23, TS_DESC_TS_serial45, TS_DESC_TS_serial67};
static TS_DESC MP_serialno_desc[NUM_SN_ADDR] = {TS_DESC_MP_serial01, TS_DESC_MP_serial23, TS_DESC_MP_serial45, TS_DESC_MP_serial67};
static char *fid = "tristarConnect:ModelHandshake";

    if (ReadSerialNo(handle, TS_serialno_desc, serialno)) {
        handle->ident.model = TS_MODEL_TS;
        handle->ident.device = (handle->slave == TS_SLAVE_CHRG) ? TS_DEVICE_TSCH : TS_DEVICE_TSLD;
        handle->V_PU = handle->I_PU = 1.0;
    } else if (ReadSerialNo(handle, MP_serialno_desc, serialno)) {
        handle->ident.model = TS_MODEL_MP;
        if (handle->slave != TS_SLAVE_CHRG) return TS_CONNECT_MISWIRE;
        handle->ident.device = TS_DEVICE_MPPT;
        if (!SetScaleFactors(handle)) return TS_CONNECT_HANDSHAKE_FAIL;
    } else {
        return TS_CONNECT_UNKOWN_DEVICE;
    }

    if (strlen(handle->ident.serialno) != 0 && strcmp(handle->ident.serialno, serialno) != 0) {
        tristarLog(handle, "%s: NOTICE: %s serial number changed from '%s' to '%s'\n", fid, handle->peerID, handle->ident.serialno, serialno);
    }

    strcpy(handle->ident.serialno, serialno);

    return TS_CONNECT_OK;
}

int tristarConnect(TS *handle)
{
struct timeval timeout;
static char *fid = "tristarConnect";

/* argument and sanity checks */

    if (handle == NULL) {
        errno = EINVAL;
        return TS_CONNECT_EINVAL;
    }

    switch (handle->slave) {
      case TS_SLAVE_CHRG: break;
      case TS_SLAVE_LOAD: break;
      default:
        return TS_CONNECT_ILLEGAL_SLAVE;
    }

/* dispose of any previous MODBUS context */

    _tristarClearModbusConext(handle);
    handle->ident.model = TS_MODEL_UNKNOWN;
    handle->ident.device = TS_DEVICE_NONE;

/* connect to the MODBUS server */

    if ((handle->ctx = modbus_new_tcp(handle->server, handle->port)) == NULL) return TS_CONNECT_MODBUS_NEW_ERROR;

/* set the modbus timeouts and debug options, if desired */

    if (handle->opt.bto > 0) {
        utilMsecToTimeval(&timeout, handle->opt.bto);
        modbus_set_byte_timeout(handle->ctx, &timeout);
    }

    if (handle->opt.rto > 0) {
        utilMsecToTimeval(&timeout, handle->opt.rto);
        modbus_set_response_timeout(handle->ctx, &timeout);
    }

    if (handle->opt.debug) modbus_set_debug(handle->ctx, TRUE);

/* connect to the server and set the slave device */

    if (modbus_connect(handle->ctx) == -1) return TS_CONNECT_FAIL;
    modbus_set_slave(handle->ctx, handle->slave);

/* figure out the peer device based on the model and slave */

    return ModelHandshake(handle);
}

void tristarDisconnect(TS *handle)
{
    _tristarClearModbusConext(handle);
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
 * $Log: connect.c,v $
 * Revision 1.1  2016/06/06 21:51:36  dechavez
 * initial release
 *
 */
