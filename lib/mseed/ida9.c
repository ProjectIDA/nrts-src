#pragma ident "$Id: ida9.c,v 1.1 2014/08/11 18:18:01 dechavez Exp $"
/*======================================================================
 *
 *  Conversion from IDA9 (pre-IDA10) to MSEED_RECORD
 *
 *====================================================================*/
#include "mseed.h"
#include "ida.h"
#include "isi.h"
#include "dmc.h"

BOOL mseedConvertIDA9(MSEED_HANDLE *handle, MSEED_RECORD *dest, UINT8 *src)
{
static char *fid = "mseedConvertIDA9";

    mseedLog(handle, -1, "%s: IDA9 support not implemented\n", fid);
    errno = ENOTSUP;
    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: ida9.c,v $
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
