#pragma ident "$Id: close.c,v 1.3 2014/08/11 18:01:19 dechavez Exp $"
/*======================================================================
 * 
 * Close a QIO handle
 *
 *====================================================================*/
#include "qio.h"

void qioClose(QIO *qio)
{
    if (qio == NULL) return;
    switch (qio->type) {
      case QIO_UDP:
        shutdown(qio->method.udp.sd, 2);
        utilCloseSocket(qio->method.udp.sd);
        break;
      case QIO_TTY:
        ttyioClose(qio->method.tty);
        break;
    }
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
 * $Log: close.c,v $
 * Revision 1.3  2014/08/11 18:01:19  dechavez
 *  MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.2  2011/02/03 17:59:55  dechavez
 * close UDP sockets
 *
 * Revision 1.1  2011/01/25 18:34:00  dechavez
 * initial release
 *
 */
