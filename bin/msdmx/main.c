#pragma ident "$Id: main.c,v 1.2 2015/10/02 15:48:06 dechavez Exp $"
/*======================================================================
 *
 *  Program to demultiplex and decompress miniSeed into CSS 3.0
 *  Same behavior as i10dmx except the sta= option is dropped.
 *
 *====================================================================*/
#include "msdmx.h"

#define MY_MOD_ID MSDMX_MOD_MAIN

static void PrintVersionBanner(FILE *fp, char *myname)
{
    fprintf(fp, "%s - %s (build: %s %s)\n", myname, VersionIdentString, __DATE__, __TIME__);
}

static void help(char *myname)
{
    PrintVersionBanner(stderr, myname);
    fprintf(stderr, "usage: %s [outdir=output] < MiniSEED\n", myname);
    exit(0);
}

int main(int argc, char **argv)
{
int i;
MSEED_RECORD record;
char *outdir = NULL;
char dirpath[MAXPATHLEN+1], workdir[MAXPATHLEN+1];
static char *default_outdir = MSDMX_DEFAULT_OUTDIR;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "outdir=", strlen("outdir=")) == 0) {
            outdir = argv[i] + strlen("outdir=");
        } else if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else {
            fprintf(stderr,"Unrecognized argument '%s'\n",argv[i]);
            help(argv[0]);
        }
    }

    PrintVersionBanner(stdout, argv[0]);

    if (outdir == NULL) outdir = default_outdir;

    CreateOutputDirectory(outdir);
    InitLogging(argv[0]);

    while (mseedReadRecord(stdin, &record)) ProcessRecord(&record);

    if (!feof(stdin)) {
        fprintf(stderr, "mseedReadRecord: %s\n", strerror(errno));
        exit(1);
    }

    GracefulExit(0);
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
 * $Log: main.c,v $
 * Revision 1.2  2015/10/02 15:48:06  dechavez
 * removed duplicate version banners for -h option
 *
 * Revision 1.1  2015/10/02 15:46:39  dechavez
 * initial production release
 *
 */
