#pragma ident "$Id: e300.h,v 1.2 2015/12/07 19:18:12 dechavez Exp $"
/*======================================================================
 *
 *  E300 utilities
 *
 *====================================================================*/
#ifndef e300_h_included
#define e300_h_included

#include "platform.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#define E300_DIGI_PASSTHRU_PORT 2001

/* function prototypes */

/* util.c */
BOOL e300Extcal(char *e300);
BOOL e300Safe(char *e300);
BOOL e300Extcal(char *e300);
BOOL e300Ping(char *e300);

/* version.c */
char *e300VersionString(void);
VERSION *e300Version(void);

#ifdef __cplusplus
}
#endif

#endif /* e300_h_included */

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
 * $Log: e300.h,v $
 * Revision 1.2  2015/12/07 19:18:12  dechavez
 * added missing e300Extcal() prototype
 *
 * Revision 1.1  2010/09/29 21:19:08  dechavez
 * initial release
 *
 */
