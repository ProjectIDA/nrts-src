#pragma ident "$Id: iir.c,v 1.2 2014/08/11 18:26:16 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.iir
 *
 *====================================================================*/
#include "protos.h"

static void ParseIirSection(MYXML_PARSER *xr, QDP_IIR_DATA *data)
{
int result;
char *mark, *tag, *value;

    data->type = QDP_IIR_LOPASS; /* maybe */

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            if ((tag = XmlCrntTag(xr)) == NULL) return;
            if ((value = XmlCrntValue(xr)) == NULL) return;
                 if (strcmp(tag, "cutratio") == 0) data->ratio = (REAL32) atof(value);
            else if (strcmp(tag, "poles"   ) == 0) data->npole =  (UINT8) atoi(value);
            else if (strcmp(tag, "high"    ) == 0) data->type  = atoi(value) ? QDP_IIR_HIPASS : QDP_IIR_LOPASS;
            else XmlWarn(xr);
            break;
          default:
            if (data->type == QDP_IIR_HIPASS) data->npole |= 0x1000;
            return;
        }
    }
}

static void set_value(MYXML_PARSER *xr, QDP_TOKEN_IIR *iir)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "name"    ) == 0) strlcpy(iir->name, value, QDP_PASCALSTRING_LEN+1);
    else if (strcmp(tag, "sections") == 0) iir->nsection =  (UINT8) atoi(value);
    else if (strcmp(tag, "gain"    ) == 0) iir->gain     = (REAL32) atof(value);
    else if (strcmp(tag, "reffreq" ) == 0) iir->refreq   = (REAL32) atof(value);
    else XmlWarn(xr);
}

int qdpXmlParse_tokensIir(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id)
{
int i, result;
QDP_TOKEN_IIR new;
char *mark, *tag, tst[] = "sectXXX + slop";

    memset(&new, 0, sizeof(QDP_TOKEN_IIR));
    new.id = id;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;
            for (i = 0; i < new.nsection; i++) {
                sprintf(tst, "sect%d", i);
                if (strcmp(tag, tst) == 0) ParseIirSection(xr, &new.data[i]);
            }
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &new);
            break;
          default:
            if (!listAppend(&token->iir, &new, sizeof(QDP_TOKEN_IIR))) return MYXML_ERROR;
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
 * $Log: iir.c,v $
 * Revision 1.2  2014/08/11 18:26:16  dechavez
 * made ParseIirSection() static and void
 *
 * Revision 1.1  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
