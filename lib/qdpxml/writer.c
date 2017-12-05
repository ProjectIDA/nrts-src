#pragma ident "$Id: writer.c,v 1.2 2009/10/20 22:08:35 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.writer
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_XML_WRITER *writer)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "proto_ver") == 0) strlcpy(writer->proto_ver, value, QDP_XML_WRITER_MAXSTRLEN+1);
    else if (strcmp(tag, "name"     ) == 0) strlcpy(writer->name, value, QDP_XML_WRITER_MAXSTRLEN+1);
    else if (strcmp(tag, "soft_ver" ) == 0) strlcpy(writer->soft_ver, value, QDP_XML_WRITER_MAXSTRLEN+1);
    else if (strcmp(tag, "created"  ) == 0) strlcpy(writer->created, value, QDP_XML_WRITER_MAXSTRLEN+1);
    else if (strcmp(tag, "updated"  ) == 0) strlcpy(writer->updated, value, QDP_XML_WRITER_MAXSTRLEN+1);
    else XmlWarn(xr);
}

int qdpXmlParse_writer(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int result;
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &config->writer);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_WRITER;
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
 * $Log: writer.c,v $
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
