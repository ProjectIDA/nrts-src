#pragma ident "$Id: myxml.c,v 1.2 2015/12/04 23:33:59 dechavez Exp $"
/*======================================================================
 *
 * Convenience functions for writing simple XML strings without having
 * to actually learn anything about XML
 *
 *====================================================================*/
#include "myxml.h"
#include "util.h"

#define MYXML_MAX_INDENT 128

MYXML_HANDLE *XmlOpen(char *path)
{
FILE *fp;
MYXML_HANDLE *handle;

    if (path == NULL) {
        fp = stdout;
    } else if (strlen(path) == 0) {
        fp = stdout;
    } else if (strcasecmp(path, "-") == 0) {
        fp = stdout;
    } else if (strcasecmp(path, "stdout") == 0) {
        fp = stdout;
    } else if (strcasecmp(path, "stderr") == 0) {
        fp = stderr;
    } else if ((fp = fopen(path, "w")) == NULL) {
        return NULL;
    }

    if ((handle = (MYXML_HANDLE *) malloc(sizeof(MYXML_HANDLE))) == NULL) return NULL;
    handle->fp = fp;
    handle->nbyte = -1;
    handle->buf[0] = 0;

    return handle;
}

void XmlClose(MYXML_HANDLE *xp)
{
    if (xp == NULL) return;
    if (xp->fp == stdout || xp->fp == stderr) return;
    fclose(xp->fp);
    free(xp);
    return;
}

void XmlPrint(MYXML_HANDLE *xp, char *format, ...)
{
int i, maxlen;
va_list marker;
char *ptr;

    va_start(marker, format);
    vsnprintf(xp->str, MYXML_STRLEN, format, marker);
    va_end(marker);
    fprintf(xp->fp, "%s", xp->str);
    fflush(xp->fp);

    if (xp->nbyte < 0) return; 

/* save all but line termination for later CRC */

    for (ptr = xp->str; xp->nbyte < MYXML_BUFLEN && *ptr != 0; ptr++) {
        if (*ptr != 0x0A && *ptr != 0x0D) xp->buf[xp->nbyte++] = *ptr;
    }
    xp->buf[xp->nbyte] = 0;
}

void XmlIndent(MYXML_HANDLE *xp, int indent)
{
int i;

    if (indent > MYXML_MAX_INDENT) indent = MYXML_MAX_INDENT;
    for (i = 0; i < indent; i++) XmlPrint(xp, " ");
}

char *XmlEscapeString(char value)
{
#define NUM_ESCAPE 5
static struct {
    char reserved; /* reserved character */
    char *string;  /* escape string */
} EscapeMap[NUM_ESCAPE] = {
    {'&',  "&amp"},
    {'<',  "&lt"}, 
    {'>',  "&gt"}, 
    {'\'', "&apos"},
    {'"',  "&quot"}
};  
int i;
 
    for (i = 0; i < NUM_ESCAPE; i++) if (value == EscapeMap[i].reserved) return EscapeMap[i].string;
    return NULL;
}
void XmlPrintString(MYXML_HANDLE *xp, int indent, char *tag, char *string)
{
char *ptr, *escape;

    XmlIndent(xp, indent);       
    XmlPrint(xp, "<%s>\"", tag);
    for (ptr = string; *ptr != 0; ptr++) {
        if ((escape = XmlEscapeString(*ptr)) != NULL) {
            XmlPrint(xp, "%s", escape);
        } else {
            XmlPrint(xp, "%c", *ptr);
        }
    }
    XmlPrint(xp, "\"</%s>\n", tag);
}

void XmlPrintInt(MYXML_HANDLE *xp, int indent, char *tag, int value)
{
    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>%d</%s>\n", tag, value, tag);
}

void XmlPrintUINT16(MYXML_HANDLE *xp, int indent, char *tag, UINT16 value)
{
    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>%hu</%s>\n", tag, value, tag);
}

void XmlPrintHex32(MYXML_HANDLE *xp, int indent, char *tag, UINT32 value)
{
    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>%X</%s>\n", tag, value, tag);
}
void XmlPrintBint8(MYXML_HANDLE *xp, int indent, char *tag, UINT8 value)
{
char buf[9];

    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>%s</%s>\n", tag, utilBinString(value, buf), tag);
}

void XmlPrintDouble(MYXML_HANDLE *xp, int indent, char *format, char *tag, double value)
{
    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>", tag);
    XmlPrint(xp, format, value);
    XmlPrint(xp, "</%s>\n", tag);
}

void XmlPrintDotDecimal(MYXML_HANDLE *xp, int indent, char *tag, UINT32 value)
{
char dot_decimal[16];

    XmlIndent(xp, indent);
    XmlPrint(xp, "<%s>%s</%s>\n", tag, utilDotDecimalString(value, dot_decimal), tag);
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
 * $Log: myxml.c,v $
 * Revision 1.2  2015/12/04 23:33:59  dechavez
 * fixed printf() format statement to calm OS X compiles
 *
 * Revision 1.1  2009/10/02 19:45:55  dechavez
 * initial release
 *
 */
