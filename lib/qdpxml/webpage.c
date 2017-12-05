#pragma ident "$Id: webpage.c,v 1.4 2009/11/05 18:40:10 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.web_page
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int i, j;
#define MAXLINELEN 8192
char line[MAXLINELEN+1];
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

    if (strcmp(tag, "web_page") == 0) {
        line[0] = 0;
        for (i = 0, j = 0; i < strlen(value); i++) {
            if (value[i] == 0x0d) continue; /* ignore carriage returns */
            if (j == 0 && isspace(value[i])) continue; /* ignore leading whitespace */
            if (value[i] != 0x0a) {  /* use new lines to terminate current line of text */
                line[j++] = value[i];
            } else {
                line[j] = 0;
                if (strncmp(line, "<crc>", strlen("<crc>")) != 0) listAppend(&config->webpage, line, strlen(line)+1);
                j = 0;
            }
        }
    } else {
        XmlWarn(xr);
    }
}

int qdpXmlParse_webpage(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
char *mark;
int result;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, config);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_WEBPAGE;
            if (!listSetArrayView(&config->webpage)) return MYXML_ERROR;
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
 * $Log: webpage.c,v $
 * Revision 1.4  2009/11/05 18:40:10  dechavez
 * use isspace() instead of isblank()
 *
 * Revision 1.3  2009/10/29 17:20:56  dechavez
 * removed tabs
 *
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
