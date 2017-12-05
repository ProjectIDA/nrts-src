#pragma ident "$Id: errno.h,v 1.9 2016/07/20 16:07:56 dechavez Exp $"
/*======================================================================
 *
 * QDP library error codes
 *
 *====================================================================*/
#ifndef qdp_errno_h_included
#define qdp_errno_h_included

#define QDP_ERR_NO_ERROR       0
#define QDP_ERR_INVALID        1
#define QDP_ERR_NOSUCH         2
#define QDP_ERR_NOMEM          3
#define QDP_ERR_BUSY           4
#define QDP_ERR_UDPIO          5
#define QDP_ERR_TTYIO          6
#define QDP_ERR_DPORT_DISABLED 7
#define QDP_ERR_REGERR         8
#define QDP_ERR_WATCHDOG       9
#define QDP_ERR_BADAUTH       10
#define QDP_ERR_NORESPONSE    11

#endif /* qdp_errno_h_included */

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
 * $Log: errno.h,v $
 * Revision 1.9  2016/07/20 16:07:56  dechavez
 * removed QDP_ERR_NOREPLY_SRVRQ, QDP_ERR_NOREPLY_CHRSP, QDP_ERR_REJECTED, QDP_ERR_NODPMEM
 * added QDP_ERR_DPORT_DISABLED, QDP_ERR_REGERR, QDP_ERR_WATCHDOG. QDP_ERR_BADAUTH, QDP_ERR_NORESPONSE
 *
 * Revision 1.8  2016/06/23 19:51:45  dechavez
 * added QDP_ERR_NODPMEM
 *
 * Revision 1.7  2016/06/16 15:31:14  dechavez
 * added QDP_ERR_REJECTED
 *
 * Revision 1.6  2016/02/11 18:44:43  dechavez
 * changed QDP_ERR_NOREPLY to QDP_ERR_NOREPLY_SRVRQ, introduced QDP_ERR_NOREPLY_CHRSP
 *
 * Revision 1.5  2016/02/05 16:43:08  dechavez
 * added QDP_ERR_NOREPLY
 *
 * Revision 1.4  2015/12/23 20:26:15  dechavez
 * added QDP_ERR_TTYIO
 *
 * Revision 1.3  2010/04/02 18:13:16  dechavez
 * added QDP_ERR_UDPIO
 *
 * Revision 1.2  2010/03/22 21:29:17  dechavez
 * added QDP_ERR_INVALID, QDP_ERR_NOMEM, QDP_ERR_BUSY
 *
 * Revision 1.1  2009/05/14 18:18:02  dechavez
 * initial release
 *
 */
