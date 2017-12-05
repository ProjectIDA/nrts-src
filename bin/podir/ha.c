#pragma ident "$Id: ha.c,v 1.3 2015/09/30 20:25:45 dechavez Exp $"
/*======================================================================
 *
 *  Create the H.A file
 *
 *====================================================================*/
#include "podir.h"

BOOL CreateHA(DCCDB *db, char *outdir)
{
int i;
FILE *fp;
static char path[MAXPATHLEN+1];
static char *fid = "CreateHA";

    sprintf(path, "%s/%s", outdir, PODIR_HA_FILE_NAME);
    if ((fp = fopen(path, "w")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, path, strerror(errno));
        return FALSE;
    }

    for (i = 0; i < db->nb30; i++) mseedWriteBlockette30(fp, &db->b30[i]);
    for (i = 0; i < db->nb33; i++) mseedWriteBlockette33(fp, &db->b33[i]);
    for (i = 0; i < db->nb34; i++) mseedWriteBlockette34(fp, &db->b34[i]);

    fclose(fp);

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: ha.c,v $
 * Revision 1.3  2015/09/30 20:25:45  dechavez
 * initial production release
 *
 */
