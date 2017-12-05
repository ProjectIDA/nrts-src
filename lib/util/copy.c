#pragma ident "$Id: copy.c,v 1.1 2015/03/06 23:04:34 dechavez Exp $"
/*======================================================================
 *
 *  Copy a file.
 *
 *====================================================================*/
#include "util.h"

#define DEFAULT_BUFLEN 1024

static BOOL CloseFiles(FILE *ifp, FILE *ofp, char *dst, BOOL status)
{
   fclose(ifp);
   fclose(ofp);
   if (status == FALSE) utilDeleteFile(dst);
   return status;
}

BOOL utilCopyfile(char *dst, char *src, int buflen)
{
int len;
FILE *ifp, *ofp;
char *buf;

    if ((ifp = fopen(src, "r")) == NULL) return FALSE;
    if ((ofp = fopen(dst, "w")) == NULL) return FALSE;

    if (buflen < 1) buflen = DEFAULT_BUFLEN;
    if ((buf = (char *) malloc(buflen)) == NULL) return FALSE;

    while ((len = fread(buf, sizeof(char), buflen, ifp)) > 0) {
        if (fwrite(buf, sizeof(char), len, ofp) != len) return CloseFiles(ifp, ofp, dst, FALSE);
    }

    return CloseFiles(ifp, ofp, dst, (ferror(ifp) || ferror(ofp)) ? FALSE : TRUE);
}

#ifdef DEBUG_TEST

int main(int argc, char **argv)
{
char *src, *dst;
int buflen;

    if (argc == 3) {
        src = argv[1];
        dst = argv[2];
        buflen = 0;
    } else if (argc == 4) {
        src = argv[1];
        dst = argv[2];
        buflen = atoi(argv[3]);
    } else {
        fprintf(stderr, "usage: %s src dst [ buflen ]\n", argv[0]);
        exit(1);
    }

    printf("utilCopyfile('%s', '%s', %d) = %s\n", dst, src, buflen, utilCopyfile(dst, src, buflen) ? "TRUE" : "FALSE");
}

#endif /* DEBUG_TEST */

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
 * $Log: copy.c,v $
 * Revision 1.1  2015/03/06 23:04:34  dechavez
 * initial release
 *
 */
