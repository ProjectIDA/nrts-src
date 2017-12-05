#pragma ident "$Id: main.c,v 1.3 2015/10/30 22:31:39 dechavez Exp $"
/*======================================================================
 *
 *  Filter and decimate 1 sps LH data into VH.  This version is really
 *  stupid, and works by reading in the entire time series into memory
 *  before applying the filter, only understands INT32 data and assumes
 *  everything named LH? is to be filtered.  Done this way soley in the
 *  interest of expediency.
 *
 *====================================================================*/
#include "vhfilt.h"

#define MY_MOD_ID VHFILT_MOD_MAIN

static void help(char *myname, int exitcode)
{
    fprintf(stderr, "usage: %s [ -v ] < MiniSeedStdinStream > MiniSeedStdoutStream\n", myname);
    exit(exitcode);
}

int main(int argc, char **argv)
{
int i, count = 0;
LOGIO *lp = NULL;
MSEED_RECORD record;
BOOL verbose = FALSE;
VHFILT filter;
LNKLST *list;
LNKLST_NODE *crnt;

    for (i = 0; i < VHFILT_NPASS; i++) filter.pass[i].path = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0], 0);
        } else {
            fprintf(stderr, "unrecognized argument `%s'\n", argv[i]);
            help(argv[0], 1);
        }
    }

/* Start logging facility */

    lp = InitLogging(argv[0], verbose);

/* Load the filters */

    LoadFilters(&filter);
    LogMsg("total group delay = %.3lf sec\n", (REAL64) (filter.delay / NANOSEC_PER_SEC));
    LogMsg("minimum input len = %ld samples\n", filter.minsamp);

/* Read and demultiplex the input into a list of contiguous TIME_SERIES */

    list = ReadInput();

/* Process each TIME_SERIES */

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        Process((TIME_SERIES *) crnt->payload, &filter, stdout);
        ++count;
        crnt = listNextNode(crnt);
    }

    LogMsg("%d traces processed\n", count);
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
 * Revision 1.3  2015/10/30 22:31:39  dechavez
 * initial production release
 *
 */
