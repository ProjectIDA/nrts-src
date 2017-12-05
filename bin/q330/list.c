#pragma ident "$Id: list.c,v 1.5 2014/01/24 00:04:24 dechavez Exp $"
/*======================================================================
 *
 *  List all digitizers in the configuration file
 *
 *====================================================================*/
#include "q330cc.h"

void ListDigitizers(Q330 *q330)
{
Q330_ADDR *addr;
LNKLST_NODE *crnt;

    crnt = listFirstNode(q330->cfg->addr);
    crnt = listNextNode(crnt); /* skip over built in console entry */
    while (crnt != NULL) {
        addr = (Q330_ADDR *) crnt->payload;
        q330PrintAddr(stdout, addr);
        crnt = listNextNode(crnt);
    }
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
 * $Log: list.c,v $
 * Revision 1.5  2014/01/24 00:04:24  dechavez
 * ListDigitizers changed to return (not exit) on completion
 *
 * Revision 1.4  2011/02/01 20:23:28  dechavez
 * skip over built-int console entry in Q330_ADDR list
 *
 * Revision 1.3  2010/11/24 19:23:05  dechavez
 * use new q330PrintAddr() function
 *
 * Revision 1.2  2009/07/25 17:38:49  dechavez
 * reworked for linked list instead of array
 *
 * Revision 1.1  2009/07/23 20:29:53  dechavez
 * added
 *
 */
