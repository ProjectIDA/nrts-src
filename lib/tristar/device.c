#pragma ident "$Id: device.c,v 1.1 2016/06/06 21:51:36 dechavez Exp $"
/*======================================================================
 *
 *  TS handle support
 *
 *====================================================================*/
#define TS_INCLUDE_STATIC_TS_DEFAULT_OPT
#include "tristar.h"
#include "private.h"
#include "util.h"

void _tristarClearModbusConext(TS *handle)
{
    if (handle->ctx == NULL) return; 

    modbus_close(handle->ctx);
    modbus_free(handle->ctx);
    handle->ctx = NULL; 
}

TS *tristarDestroyHandle(TS *handle)
{
    if (handle == NULL) return NULL;
    _tristarClearModbusConext(handle);
    free(handle);

    return NULL;
}

TS *tristarCreateHandle(char *string, int slave, TS_OPT *opt)
{
TS *handle = NULL;
char server[MAXPATHLEN+1];
int port = TS_DEFAULT_PORT;
static char *fid = "tristarHandle";

/* server and slave must be specified */

    if (string == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (!utilParseServer(string, server, &port)) return NULL;

    if (slave != TS_SLAVE_CHRG && slave != TS_SLAVE_LOAD) {
        errno = EINVAL;
        return NULL;
    }

/* create and initialize (most of) the handle (remainder is done in tristarConnect()) */

    if ((handle = (TS *) malloc(sizeof(TS))) == NULL) return NULL;

    memset(handle, 0, sizeof(TS));

    strncpy(handle->server, server, MAXPATHLEN); handle->server[MAXPATHLEN] = 0;
    handle->port = (port > 0) ? port : TS_DEFAULT_PORT;
    handle->slave = slave;
    handle->opt = (opt != NULL) ? *opt : TS_DEFAULT_OPT;
    handle->ctx = NULL;
    sprintf(handle->peerID, "%s:%d", handle->server, handle->port);

/* all done */

    return handle;
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
 * $Log: device.c,v $
 * Revision 1.1  2016/06/06 21:51:36  dechavez
 * initial release
 *
 */
