#pragma ident "$Id: cds.c,v 1.1 2009/10/20 22:08:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.cds
 *
 *====================================================================*/
#include "protos.h"

static void SetEquationEntry(QDP_DP_TOKEN *token, QDP_TOKEN_CDS *cds, char *value, UINT8 nib)
{
QDP_TOKEN_IDENT *entry;

    if (cds->nentry == QDP_MAX_CDS_ENTRY) return;
    cds->equation[cds->nentry].code = (nib << 6);
    strlcpy(cds->equation[cds->nentry].name, value, QDP_PASCALSTRING_LEN+1);
    ++cds->nentry;
}

static void set_value(MYXML_PARSER *xr, QDP_DP_TOKEN *token, QDP_TOKEN_CDS *cds)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "logmsg") == 0) cds->options |= ((UINT8) atoi(value));
    else if (strcmp(tag, "name"  ) == 0) strlcpy(cds->name, value, QDP_PASCALSTRING_LEN+1);
    else if (strcmp(tag, "comm"  ) == 0) SetEquationEntry(token, cds, value, QDP_CDS_NIB_COMM);
    else if (strcmp(tag, "det"   ) == 0) SetEquationEntry(token, cds, value, QDP_CDS_NIB_DET);
    else if (strcmp(tag, "cal"   ) == 0) SetEquationEntry(token, cds, value, QDP_CDS_NIB_CAL);
    else if (strcmp(tag, "op"    ) == 0) SetEquationEntry(token, cds, value, QDP_CDS_NIB_OP);
    else XmlWarn(xr);
}

int qdpXmlParse_tokensCds(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id)
{
int result;
QDP_TOKEN_CDS new;
char *mark;
       
    memset(&new, 0, sizeof(QDP_TOKEN_CDS));

    new.id = id;
    new.nentry = 0;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, token, &new);
            break;
          default:
            if (!listAppend(&token->cds, &new, sizeof(QDP_TOKEN_CDS))) return MYXML_ERROR;
            return result;
        }
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: cds.c,v $
 * Revision 1.1  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
