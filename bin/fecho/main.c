#pragma ident "$Id: main.c,v 1.1 2015/09/11 16:09:59 dechavez Exp $"
/*======================================================================
 *
 *  Read a filter from stdin and echo to stdout.  Conversion from old
 *  to new format takes place automatically.
 *
 *====================================================================*/
#include "filter.h"

static void CheckCoeff(FILTER_COEFF *coeff)
{
int i;
REAL64 sum = 0.0;

    for (i = 0; i < coeff->ncoef; i++) sum += coeff->coef[i];
	printf("Sum of %d coefficients = %lf\n", coeff->ncoef, sum);
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s < filter_file\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
FILTER filter;

    if (argc != 1) help(argv[0]);

    if (!filterRead(stdin, &filter)) {
        perror("filterRead");
        exit(1);
    }

    filterPrint(stdout, &filter);

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
 * Revision 1.1  2015/09/11 16:09:59  dechavez
 * initial release
 *
 */
