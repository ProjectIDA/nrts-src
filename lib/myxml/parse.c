#pragma ident "$Id: parse.c,v 1.4 2015/12/04 23:34:09 dechavez Exp $"
/*======================================================================
 *
 * Convenience functions for reading simple XML files without having
 * to actually learn anything about XML.  Tailored to work with the
 * Q330 configuration files, so only sufficient support for the types
 * of records found in those files is included.
 *
 *====================================================================*/
#include "myxml.h"
#include <libxml/xmlreader.h>

static MYXML_PARSER *CreateParser()
{
MYXML_PARSER *xr;

    if ((xr = (MYXML_PARSER *) malloc(sizeof(MYXML_PARSER))) == NULL) return NULL;
    if (!listInit(&xr->taglist)) {
        free(xr);
        return NULL;
    }

    xr->reader = NULL;
    xr->warn = NULL;
    xr->crnt = NULL;
    xr->name = NULL;
    xr->value = NULL;

    return xr;
}

MYXML_PARSER *XmlDestroyParser(MYXML_PARSER *xr)
{
    if (xr != NULL) {
        if (xr->reader != NULL) {
            xmlFreeTextReader(xr->reader);
            xmlCleanupParser();
            xmlMemoryDump();
        }
        listDestroy(&xr->taglist);
        free(xr);
    }

    return NULL;
}

MYXML_PARSER *XmlOpenParserForFile(char *fname, FILE *warn)
{
MYXML_PARSER *xr;

    if ((xr = CreateParser()) == NULL) return NULL;
    if ((xr->reader = xmlReaderForFile(fname, NULL, 0)) == NULL) return XmlDestroyParser(xr);
    xr->warn = warn;

    return xr;
}

MYXML_PARSER *XmlOpenParserForFILE(FILE *fp, FILE *warn)
{
MYXML_PARSER *xr;
static char empty[] = "";

    if ((xr = CreateParser()) == NULL) return NULL;
    if ((xr->reader = xmlReaderForFd(fileno(fp), empty, NULL, 0)) == NULL) return XmlDestroyParser(xr);
    xr->warn = warn;

    return xr;
}

char *XmlLastTag(MYXML_PARSER *xr)
{
LNKLST_NODE *last;

    if (xr == NULL) return NULL;
    if ((last = listLastNode(&xr->taglist)) == NULL) return NULL;
    return (char *) last->payload;
}

char *XmlCrntPath(MYXML_PARSER *xr)
{
LNKLST_NODE *crnt;

    if (xr == NULL) return NULL;

    crnt = listFirstNode(&xr->taglist);
    xr->path[0] = 0;
    while (crnt != NULL) {
        strcat(xr->path, "<"); strcat(xr->path, (char *) crnt->payload); strcat(xr->path, ">");
        crnt = listNextNode(crnt);
    }
    return xr->path;
}

char *XmlCrntValue(MYXML_PARSER *xr)
{
    if (xr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return xr->value;
}

char *XmlCrntTag(MYXML_PARSER *xr)
{
    if (xr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return xr->crnt;
}

int XmlNextRecord(MYXML_PARSER *xr, char *mark)
{
int result, type;
char *temp;

    if (xr == NULL) {
        errno = EINVAL;
        return MYXML_ERROR;
    }

    while ((result = xmlTextReaderRead(xr->reader)) == 1) {
        xr->name = (char *) xmlTextReaderConstName(xr->reader);
        switch ((type = xmlTextReaderNodeType(xr->reader))) {
          case XML_READER_TYPE_TEXT:
          case XML_READER_TYPE_CDATA:
            if ((xr->value = (char *) xmlTextReaderConstValue(xr->reader)) == NULL) return MYXML_ERROR;
            temp = xr->value;
            if (temp[0] == '"' && temp[strlen(temp)-1] == '"') {
                temp[strlen(temp)-1] = 0;
                ++temp;
            }
            xr->value = temp;
            return MYXML_VALUE;
          case XML_READER_TYPE_ELEMENT:
            if (!listAppend(&xr->taglist, xr->name, strlen(xr->name)+1)) return MYXML_ERROR;
            xr->crnt = XmlLastTag(xr);
            return MYXML_RECORD_BEG;
          case XML_READER_TYPE_END_ELEMENT:
            listRemoveNode(&xr->taglist, listLastNode(&xr->taglist));
            xr->crnt = XmlLastTag(xr);
            if (mark != NULL && strcmp(xr->name, mark) == 0) {
                return MYXML_COMPLETE;
            } else {
                return MYXML_RECORD_END;
            }
        }
    }

    return (result == 0) ? MYXML_EOF : MYXML_ERROR;
}

int XmlSkipRecord(MYXML_PARSER *xr)
{
char *name;
int result;

    do {
        result = XmlNextRecord(xr, XmlCrntTag(xr));
    } while (result != MYXML_COMPLETE && result != MYXML_EOF && result != MYXML_ERROR);
    
    return result;
}

void XmlWarn(MYXML_PARSER *xr)
{
    if (xr == NULL) return;
    if (xr->warn == NULL) return;
    fprintf(xr->warn, "UNEXPECTED record %s ignored\n", XmlCrntPath(xr));
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
 * $Log: parse.c,v $
 * Revision 1.4  2015/12/04 23:34:09  dechavez
 * XmlOpenParserForFile() and XmlOpenParserForFILE() return NULL instead of FALSE, to calm OS X compiles
 *
 * Revision 1.3  2009/10/20 21:50:21  dechavez
 * reworked parser to use improved handle
 *
 * Revision 1.2  2009/10/10 00:31:56  dechavez
 * reworked to move sequentially through file instead of pre-read linked list
 *
 * Revision 1.1  2009/10/07 17:32:20  dechavez
 * created
 *
 */
