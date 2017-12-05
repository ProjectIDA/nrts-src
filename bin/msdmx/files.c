#pragma ident "$Id: files.c,v 1.1 2015/10/02 15:46:39 dechavez Exp $"
/*======================================================================
 *
 *  File and directory utilities
 *
 *====================================================================*/
#include "msdmx.h"

#define MY_MOD_ID MSDMX_MOD_FILES

static char base[MAXPATHLEN+1];

FILE *OpenWfdiscFile(void)
{
FILE *fp;
char path[MAXPATHLEN+1];
static BOOL failed = FALSE; /* because GracefulExit() will call this */
static char *fid = "OpenWfdiscFile";

    if (failed) return NULL; /* only will happen when called from GracefulExit() */

    sprintf(path, "%s/%s", base, MSDMX_WFDISC_FILE_NAME);
    if ((fp = fopen(path, "a")) == NULL) {
        LogErr("%s: fopen: %s: %s\n", fid, path, strerror(errno));
        failed = TRUE;
        GracefulExit(MY_MOD_ID + 1);
    }

    return fp;
}

char *LogPathName(void)
{
static char path[MAXPATHLEN+1];

    snprintf(path, MAXPATHLEN+1, "%s/%s", base, MSDMX_LOG_FILE_NAME);
    return path;
}

char *BaseDir(void)
{
    return base;
}

void CreateOutputDirectory(char *outdir)
{
int i;
char crntdir[MAXPATHLEN+1], datadir[MAXPATHLEN+1];
static char *fid = "CreateOutputDirectory";

    if (outdir[0] == '-' || strcasecmp(outdir, ".") == 0 || strcasecmp(outdir, "..") == 0) {
        fprintf(stderr, "ERROR: '%s' is not a legal output directory name\n", outdir);
        exit(1);
    }
    getcwd(crntdir, MAXPATHLEN);

    if (outdir[0] == '/' || outdir[0] == '.') {
        strcpy(base, outdir);
    } else {
        sprintf(base, "%s/%s", crntdir, outdir);
    }

    if (utilDirectoryExists(base)) {
        fprintf(stderr, "ERROR: output directory '%s' already exists\n", outdir);
        exit(1);
    }

    snprintf(datadir, MAXPATHLEN+1, "%s/%s", base, MSDMX_DATA_DIR_NAME);
    if (util_mkpath(datadir, 0775) != 0) { 
        fprintf(stderr, "ERROR: util_mkpath: %s: %s\n", datadir, strerror(errno));
        exit(1);
    }
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
 * $Log: files.c,v $
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
