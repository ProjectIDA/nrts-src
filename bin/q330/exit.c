#pragma ident "$Id: exit.c,v 1.1 2011/10/13 18:22:02 dechavez Exp $"
/*======================================================================
 *
 *  Make sure we deregister before all exits
 *
 *====================================================================*/
#include "q330cc.h"

void GracefulExit(Q330 *q330, int status)
{
    if (q330 != NULL && q330->qdp != NULL) qdpDeregister(q330->qdp, TRUE);
    exit(status);
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
 * $Log: exit.c,v $
 * Revision 1.1  2011/10/13 18:22:02  dechavez
 * created
 *
 */
