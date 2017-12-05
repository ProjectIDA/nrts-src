#pragma ident "$Id: read.c,v 1.1 2015/07/15 17:29:40 dechavez Exp $"
/*======================================================================
 *
 *  Read a filter file.  
 *  Both old-style and new-style file formats are supported.
 *
 *====================================================================*/
#include "filter.h"

#define MAX_TOKEN  32
#define MAXLINELEN 256
#define COMMENT    '#'
#define DELIMITERS " \t"
#define QUOTE      '"'     

/* Read a single integer */

static int ReadInt(FILE *fp, int *dest)
{
int ntoken;
char *token[MAX_TOKEN];
char input[MAXLINELEN+1];

    while (utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL) == 0) {
        ntoken = utilParse(input, token, DELIMITERS, MAX_TOKEN, QUOTE);
        if (ntoken == 1) {
            *dest = strtol(token[0], NULL, 0);
            return 1;
        } else if (token > 0) {
            return -1;
        }
    }

    return -1;
}

/* Read a single complex number */

static int ReadComplex(FILE *fp, UTIL_COMPLEX *dest)
{
int ntoken;
char *token[MAX_TOKEN];
char input[MAXLINELEN+1];

    while (utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL) == 0) {
        ntoken = utilParse(input, token, DELIMITERS, MAX_TOKEN, QUOTE);
        if (ntoken == 2) {
            dest->r = atof(token[0]);
            dest->i = atof(token[1]);
            return 1;
        } else if (token > 0) {
            return -1;
        }
    }

    return -1;
}

/* Read a single real number */

static int ReadReal(FILE *fp, REAL64 *dest)
{
int ntoken;
char *token[MAX_TOKEN];
char input[MAXLINELEN+1];

    while (utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL) == 0) {
        ntoken = utilParse(input, token, DELIMITERS, MAX_TOKEN, QUOTE);
        if (ntoken == 1) {
            *dest = atof(token[0]);
            return 1;
        } else if (ntoken != 0) {
            return -1;
        }
    }

    return -1;
}

/* Read one or two real numbers */

static int ReadRealReal(FILE *fp, REAL64 *dest)
{
int ntoken;
char *token[MAX_TOKEN];
char input[MAXLINELEN+1];

    while (utilGetLine(fp, input, MAXLINELEN, COMMENT, NULL) == 0) {
        ntoken = utilParse(input, token, DELIMITERS, MAX_TOKEN, QUOTE);
        if (ntoken == 1) {
            *dest = atof(token[0]);
            return 1;
        } else if (ntoken == 2) {
            *dest++ = atof(token[0]);
            *dest   = atof(token[1]);
            return 2;
        } else if (ntoken != 0) {
            return -1;
        }
    }

    return -1;
}

static BOOL LoadOldPandZ(FILE *fp, FILTER_PANDZ *dest)
{
int i;
REAL64 rdummy;
int idummy;

    if (ReadInt( fp, &dest->nzero) != 1) return FALSE;
    if (ReadInt( fp, &dest->npole) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;
    if (ReadInt( fp, &idummy     ) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;

    if (dest->nzero > FILTER_MAX_PZ || dest->npole > FILTER_MAX_PZ) {
        errno = E2BIG;
        return FALSE;
    }

    for (i = 0; i < dest->nzero; i++) if (!ReadComplex(fp, &dest->zero[i])) return FALSE;
    for (i = 0; i < dest->npole; i++) if (!ReadComplex(fp, &dest->pole[i])) return FALSE;

    return TRUE;
}

static BOOL LoadOldCoeff(FILE *fp, FILTER_COEFF *dest, BOOL HasDelay)
{
int i, idummy, ncoef, nread;
REAL64 rdummy;

    if (ReadInt( fp, &dest->ncoef) != 1) return FALSE;
    if (ReadInt( fp, &idummy     ) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;
    if (ReadInt( fp, &idummy     ) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;
    if (ReadReal(fp, &rdummy     ) != 1) return FALSE;

    if (HasDelay) {
        if (ReadReal(fp, &dest->delay) != 1) return FALSE;
    } else {
        dest->delay = 0.0;
    }

    if (dest->ncoef > FILTER_MAX_COEF) {
        errno = E2BIG;
        return FALSE;
    }
    for (i = 0, ncoef = 0; ncoef < dest->ncoef; i++) {
        if ((nread = ReadRealReal(fp, &dest->coef[ncoef])) < 0) return FALSE;
        ncoef += nread;
    }

    return TRUE;
}

static BOOL LoadPandZ(FILE *fp, FILTER_PANDZ *dest)
{
int i;

    if (ReadInt( fp, &dest->nzero) != 1) return FALSE;
    if (ReadInt( fp, &dest->npole) != 1) return FALSE;

    if (dest->nzero > FILTER_MAX_PZ || dest->npole > FILTER_MAX_PZ) {
        errno = E2BIG;
        return FALSE;
    }

    for (i = 0; i < dest->nzero; i++) if (!ReadComplex(fp, &dest->zero[i])) return FALSE;
    for (i = 0; i < dest->npole; i++) if (!ReadComplex(fp, &dest->pole[i])) return FALSE;

    return TRUE;
}

static BOOL LoadCoeff(FILE *fp, FILTER_COEFF *dest)
{
int i, ncoef, nread;

    if (ReadInt( fp, &dest->ncoef) != 1) return FALSE;
    if (ReadReal(fp, &dest->delay) != 1) return FALSE;

    if (dest->ncoef > FILTER_MAX_COEF) {
        errno = E2BIG;
        return FALSE;
    }

    ncoef = 0;
    while (ncoef < dest->ncoef) {
        if ((nread = ReadRealReal(fp, &dest->coef[ncoef])) < 0) return FALSE;
        ncoef += nread;
    }

    return TRUE;
}

BOOL filterRead(FILE *fp, FILTER *dest)
{
    if (fp == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Filter type is the first field */

    if (!ReadInt(fp, &dest->type)) return FALSE;

    if (dest->type & FILTER_TYPE_PANDZ) {
        return LoadPandZ(fp, &dest->data.pz);
    } else if (dest->type & FILTER_TYPE_COEFF) {
        return LoadCoeff(fp, &dest->data.cf);
    } else switch (dest->type) {
      case FILTER_TYPE_OLD_ANALOG:
        dest->type = FILTER_TYPE_ANALOG;
        return LoadOldPandZ(fp, &dest->data.pz);
      case FILTER_TYPE_OLD_IIR_PZ:
        dest->type = FILTER_TYPE_IIR_PZ;
        return LoadOldPandZ(fp, &dest->data.pz);
      case FILTER_TYPE_OLD_LAPLACE:
        dest->type = FILTER_TYPE_LAPLACE;
        return LoadOldPandZ(fp, &dest->data.pz);
      case FILTER_TYPE_OLD_COMB:
        dest->type = FILTER_TYPE_COMB;
        return LoadOldCoeff(fp, &dest->data.cf, FALSE); 
      case FILTER_TYPE_OLD_FIR_SYM:
        dest->type = FILTER_TYPE_FIR_SYM;
        return LoadOldCoeff(fp, &dest->data.cf, FALSE); 
      case FILTER_TYPE_OLD_FIR_ASYM:
        dest->type = FILTER_TYPE_FIR_ASYM;
        return LoadOldCoeff(fp, &dest->data.cf, TRUE);
    }

    errno = ENOTSUP;
    dest->type = FILTER_TYPE_NULL;
    return FALSE;
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
 * $Log: read.c,v $
 * Revision 1.1  2015/07/15 17:29:40  dechavez
 * created
 *
 */
