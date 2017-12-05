#pragma ident "$Id: string.c,v 1.2 2015/12/04 22:18:12 dechavez Exp $"
/*======================================================================
 * 
 * String stuff
 *
 *====================================================================*/
#include "liss.h"
#include "util.h"

typedef struct {
    char *text;
    int code;
} TEXT_MAP;

static TEXT_MAP DataFormatMap[] = {
    {"ASCII",         0},
    {"INT16",         1},
    {"INT24",         2},
    {"INT32",         3},
    {"IEEE32",        4},
    {"IEEE64",        5},
    {"Steim1",       10},
    {"Steim2",       11},
    {"GEOSCOPE24",   12},
    {"GEOSCOPE16.3", 13},
    {"GEOSCOPE16.4", 14},
    {"USNN",         15},
    {"CDSH16",       16},
    {"Graf16",       17},
    {"IPG15",        18},
    {"Steim3",       19},
    {"SRO",          30},
    {"HGLP",         31},
    {"DWWSSN",       32},
    {"RTSN",         33},
    {NULL, -1}
};

static char *LocateString(int code, TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

char *lissDataFormatString(int code)
{
int i;
static char *DefaultMessage = "UNKNOWN_FORMAT";

    return LocateString(code, DataFormatMap, DefaultMessage);

}

char *lissMiniSeedHdrString(LISS_MSEED_HDR *hdr, char *buf)
{
char tbuf[128];
static char *mt_unsafe = "123456 D II KIVMA BHZ 00 2011:123-12:34:23.56* 240 10.0025 REAL64 1024* plus lots of slop";

    if (buf == NULL) buf = mt_unsafe;

    buf[0] = 0;
    sprintf(buf+strlen(buf), "%06ld ",   hdr->fsdh.seqno);
    sprintf(buf+strlen(buf), "`%c' ",   hdr->fsdh.qcode);
    sprintf(buf+strlen(buf), "%-2s ",   hdr->fsdh.netid);
    sprintf(buf+strlen(buf), "%-5s ",   hdr->fsdh.staid);
    sprintf(buf+strlen(buf), "%-3s ",   hdr->fsdh.chnid);
    sprintf(buf+strlen(buf), "%-2s ",   hdr->fsdh.locid);
    sprintf(buf+strlen(buf), "%s",      utilDttostr(hdr->fsdh.start, 0, tbuf));
    sprintf(buf+strlen(buf), "%c ",  (hdr->fsdh.ioclck & LISS_IOC_CLOCK_LOCKED) ? ' ' : '*');
    sprintf(buf+strlen(buf), "%6.4lf ", lissSint(&hdr->fsdh));
    sprintf(buf+strlen(buf), "%-6s ",   lissDataFormatString(hdr->b1000.format));
    sprintf(buf+strlen(buf), "%4d", (int) pow(2.0, (double) hdr->b1000.length));
    if (hdr->fsdh.ioclck & LISS_IOC_SHORT_READ) sprintf(buf+strlen(buf), "*");

    return buf;
}

void lissPrintMiniSeedHdr(FILE *fp, LISS_MSEED_HDR *hdr)
{
char *tmpbuf = "123456 D II KIVMA BHZ 00 2011:123-12:34:23.56* 240 10.0025 REAL64 1024* plus lots of slop";

    fprintf(fp, "%s\n", lissMiniSeedHdrString(hdr, tmpbuf));

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
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
 * $Log: string.c,v $
 * Revision 1.2  2015/12/04 22:18:12  dechavez
 * fixed format strings to calm OS X compiles
 *
 * Revision 1.1  2011/11/03 17:41:44  dechavez
 * initial release (lissDataFormatString(), lissMiniSeedHdrString(), lissPrintMiniSeedHdr())
 *
 */
