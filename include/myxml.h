#pragma ident "$Id: myxml.h,v 1.4 2016/06/15 21:23:51 dechavez Exp $"
/*======================================================================
 *
 * Simple output routines for writing stuff using XML without my having
 * to spend time learning the right way to do it.
 *
 *====================================================================*/
#include "list.h"
#ifndef myxml_h_defined
#define myxml_h_defined

#include "platform.h"
#include <libxml/xmlreader.h>

/* Handle for buffering XML strings for CRC calculation */

#define MYXML_STRLEN 4095
#define MYXML_BUFLEN 131071

typedef struct {
    FILE *fp;
    char str[MYXML_STRLEN+1];
    char buf[MYXML_BUFLEN+1];
    long nbyte;
} MYXML_HANDLE;

typedef struct {
    xmlTextReaderPtr reader;
    FILE *warn;
    LNKLST taglist;
    char *crnt;
    char *name;
    char *value;
    char path[MAXPATHLEN+1];
} MYXML_PARSER;

/* Constants */

#define MYXML_MAX_INDENT 128

/* parser return codes */

#define MYXML_ERROR     -1
#define MYXML_EOF        0
#define MYXML_RECORD_BEG 1
#define MYXML_RECORD_END 2
#define MYXML_VALUE      3
#define MYXML_COMPLETE   4

/* Function prototypes */

/* crc.c */
void XmlStartCRC(MYXML_HANDLE *xp);
UINT32 XmlCRC(MYXML_HANDLE *xp, BOOL clear);

/* myxml.c */
MYXML_HANDLE *XmlOpen(char *path);
void XmlClose(MYXML_HANDLE *xp);
void XmlPrint(MYXML_HANDLE *xp, char *format, ...);
void XmlIndent(MYXML_HANDLE *xp, int indent);
char *XmlEscapeString(char value);
void XmlPrintString(MYXML_HANDLE *xp, int indent, char *tag, char *string);
void XmlPrintInt(MYXML_HANDLE *xp, int indent, char *tag, int value);
void XmlPrintUINT16(MYXML_HANDLE *xp, int indent, char *tag, UINT16 value);
void XmlPrintHex32(MYXML_HANDLE *xp, int indent, char *tag, UINT32 value);
void XmlPrintBint8(MYXML_HANDLE *xp, int indent, char *tag, UINT8 value);
void XmlPrintDouble(MYXML_HANDLE *xp, int indent, char *format, char *tag, double value);
void XmlPrintDotDecimal(MYXML_HANDLE *xp, int indent, char *tag, UINT32 value);

/* parse.c */
MYXML_PARSER *XmlDestroyParser(MYXML_PARSER *xr);
MYXML_PARSER *XmlOpenParserForFile(char *fname, FILE *warn);
MYXML_PARSER *XmlOpenParserForFILE(FILE *fp, FILE *warn);
char *XmlLastTag(MYXML_PARSER *xr);
char *XmlCrntPath(MYXML_PARSER *xr);
char *XmlCrntValue(MYXML_PARSER *xr);
char *XmlCrntTag(MYXML_PARSER *xr);
void XmlMark(MYXML_PARSER *xr);
int XmlNextRecord(MYXML_PARSER *xr, char *mark);
int XmlSkipRecord(MYXML_PARSER *xr);
void XmlWarn(MYXML_PARSER *xr);

/* version.c */
char *myxmlVersionString(void);
VERSION *myxmlVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* myxml_h_included */

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
 * $Log: myxml.h,v $
 * Revision 1.4  2016/06/15 21:23:51  dechavez
 * moved list.h include to top of file for reasons that I no longer recall
 *
 * Revision 1.3  2009/10/20 21:48:32  dechavez
 * reworked parser
 *
 * Revision 1.2  2009/10/10 00:33:45  dechavez
 * reworked parser
 *
 * Revision 1.1  2009/10/07 17:32:51  dechavez
 * created
 *
 */
