#pragma ident "$Id: chnloc.c,v 1.1 2017/11/21 18:05:31 dechavez Exp $"
/*======================================================================
 *
 *  Given a SEED channel name and location code, generate an IDA style
 *  CHNLOC name while dealing with spaces in both input strings.
 *
 *====================================================================*/
#include "mseed.h"
#include "util.h"

char *mseedChnLocToChnloc(char *chnid, char *locid, char *chnloc)
{

    if (chnid == NULL || locid == NULL || chnloc == NULL) {
        errno = EINVAL;
        return NULL;
    }

    strcpy(chnid, chnid); utilTrimString(chnid); /* removes any spaces */
    strcpy(locid, locid); utilTrimString(locid); /*        ditto       */
    sprintf(chnloc, "%s%s", chnid, locid);

    return chnloc;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
 * $Log: chnloc.c,v $
 * Revision 1.1  2017/11/21 18:05:31  dechavez
 * initial release
 *
 */
