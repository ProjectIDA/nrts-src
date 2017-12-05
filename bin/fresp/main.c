#pragma ident "$Id: main.c,v 1.1 2015/09/11 16:13:36 dechavez Exp $"
/*======================================================================
 *
 *  Compute the response of a FILTER at specified frequency
 *
 *====================================================================*/
#include "filter.h"

static void help(char *myname)
{
    fprintf(stderr, "usage: %s freq [srate] < std_filter_file\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
REAL64 a0, freq, srate = 0.0;
FILTER filter;

    if (argc != 2 && argc != 3) help(argv[0]);

    if ((freq = (REAL64) atof(argv[1])) < 0.0) {
        fprintf(stderr, "illegal freq '%s'\n", argv[1]);
        exit(1);
    }

    if (argc == 3 && (srate = (REAL64) atof(argv[2])) < 0.0) {
        fprintf(stderr, "illegal srate '%s'\n", argv[2]);
        exit(1);
    }

    if (!filterRead(stdin, &filter)) {
        perror("filterRead");
        exit(1);
    }

    if (!filterA0(&filter, freq, srate, &a0)) {
        perror("filterA0");
        exit(1);
    }

    printf("%11.7lg (%11.7lg)\n", a0, 1.0 / a0);
    exit(0);
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
 * Revision 1.1  2015/09/11 16:13:36  dechavez
 * initial release
 *
 */
