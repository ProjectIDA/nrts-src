#pragma ident "$Id: chncmp.c,v 1.1 2015/03/06 23:04:34 dechavez Exp $"
/*======================================================================
 *
 *  Comparison function for use in qsort to arrange channel names into
 *  the more natural "seismologist" order (ie, bhz, bhn, bhe).
 *
 *====================================================================*/
#include "util.h"

int utilChnCompare(char *a, char *b)
{
int result;

    if (strcmp(a, b) == 0) return 0;
    if (strlen(a) != 3 || strlen(b) != 3) return strcmp(a, b);
    if (strncmp(a, b, 2) != 0) return strcmp(a, b);

    switch (a[2]) {

      case 'z':
        switch (b[2]) {
          case 'n': return 1;
          case 'e': return 1;
          case '1': return 1;
          case '2': return 1;
          default:
            return strcmp(a, b);
        }
        break;

      case 'n':
        switch (b[2]) {
          case 'z': return  1;
          case 'e': return -1;
          case '1': return -1;
          case '2': return -1;
          default:
            return strcmp(a, b);
        }
        break;

      case 'e':
        switch (b[2]) {
          case 'z': return  1;
          case 'n': return  1;
          case '1': return -1;
          case '2': return -1;
          default:
            return strcmp(a, b);
        }
        break;

      case '1':
        switch (b[2]) {
          case 'z': return  1;
          case 'n': return  1;
          case 'e': return  1;
          case '2': return -1;
          default:
            return strcmp(a, b);
        }
        break;

      case '2':
        switch (b[2]) {
          case 'z': return  1;
          case 'n': return  1;
          case 'e': return  1;
          case '1': return  1;
          default:
            return strcmp(a, b);
        }
        break;
    }

    return strcmp(a, b);
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
 * $Log: chncmp.c,v $
 * Revision 1.1  2015/03/06 23:04:34  dechavez
 * initial release
 *
 */
