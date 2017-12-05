#pragma ident "$Id: detect.c,v 1.2 2011/01/14 00:31:27 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.lcq.detect
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_LCQ_DETECT *det)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "name") == 0) qdpSetTokenIdent(&det->base, QDP_UNDEFINED_ID, value);
    else if (strcmp(tag, "run" ) == 0) det->options |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "log" ) == 0) det->options |= ((UINT8) atoi(value) << 1);
    else if (strcmp(tag, "msg" ) == 0) det->options |= ((UINT8) atoi(value) << 3);
    else XmlWarn(xr);
}

int qdpXmlParse_tokenLcqDetect(MYXML_PARSER *xr, QDP_TOKEN_LCQ *lcq)
{
int i, result;
QDP_LCQ_DETECT *det = NULL;
char *mark;

    for (i = 0; det == NULL && i < QDP_LCQ_NUM_DETECT; i++) if (!lcq->detect[i].set) det = &lcq->detect[i];
    if (det == NULL) if (xr->warn != NULL) fprintf(stderr, "WARNING: no empty lcq->detect slots!\n");
       
    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            if (det != NULL) set_value(xr, det);
            break;
          default:
            if (det != NULL) det->set = TRUE;
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
 * $Log: detect.c,v $
 * Revision 1.2  2011/01/14 00:31:27  dechavez
 * qdpSetIdent() to qdpSetTokenIdent()
 *
 * Revision 1.1  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
