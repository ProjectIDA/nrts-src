#pragma ident "$Id: murdock.c,v 1.2 2011/01/14 00:31:27 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.murdock
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TOKEN_DET *det)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "filter"  ) == 0) qdpSetTokenIdent(&det->detf, QDP_UNDEFINED_ID, value);
    else if (strcmp(tag, "filhi"   ) == 0) det->fhi = (UINT32) atoi(value);
    else if (strcmp(tag, "fillo"   ) == 0) det->flo = (UINT32) atoi(value);
    else if (strcmp(tag, "iwin"    ) == 0) det->iw  =  (UINT8) atoi(value);
    else if (strcmp(tag, "n_hits"  ) == 0) det->nht =  (UINT8) atoi(value);
    else if (strcmp(tag, "xth1"    ) == 0) det->x1  = (UINT16) atoi(value) / 2;
    else if (strcmp(tag, "xth2"    ) == 0) det->x2  = (UINT16) atoi(value) / 2;
    else if (strcmp(tag, "xth3"    ) == 0) det->x3  = (UINT16) atoi(value) / 2;
    else if (strcmp(tag, "xthx"    ) == 0) det->xx  =  (UINT8) atoi(value);
    else if (strcmp(tag, "def_tc"  ) == 0) det->tc  = (UINT16) atoi(value);
    else if (strcmp(tag, "wait_blk") == 0) det->wa  = (UINT16) atoi(value);
    else if (strcmp(tag, "val_avg" ) == 0) det->av  = (UINT16) atoi(value);
    else if (strcmp(tag, "name"    ) == 0) strlcpy(det->name, value, QDP_PASCALSTRING_LEN+1);
    else XmlWarn(xr);
}

int qdpXmlParse_tokensMurdock(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id)
{
int result;
QDP_TOKEN_DET new;
char *mark;

    qdpInitTokenDet(&new);
    new.type = QDP_TOKEN_TYPE_MHD;
    new.id = id;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &new);
            break;
          default:
            if (!listAppend(&token->det, &new, sizeof(QDP_TOKEN_DET))) return MYXML_ERROR;
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
 * $Log: murdock.c,v $
 * Revision 1.2  2011/01/14 00:31:27  dechavez
 * qdpSetIdent() to qdpSetTokenIdent()
 *
 * Revision 1.1  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
