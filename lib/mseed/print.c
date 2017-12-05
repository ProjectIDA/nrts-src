#pragma ident "$Id: print.c,v 1.5 2015/09/15 23:20:53 dechavez Exp $"
/*======================================================================
 * 
 * String stuff
 *
 *====================================================================*/
#include "mseed.h"

typedef struct {
    char *text;
    int code;
} TEXT_MAP;

static TEXT_MAP DataFormatMap[] = {
    {"INT16",  MSEED_FORMAT_INT_16},
    {"INT32",  MSEED_FORMAT_INT_32},
    {"IEEE32", MSEED_FORMAT_IEEE_F},
    {"IEEE64", MSEED_FORMAT_IEEE_D},
    {"Steim1", MSEED_FORMAT_STEIM1},
    {"Steim2", MSEED_FORMAT_STEIM2},
    {NULL, -1}
};

static char *LocateString(int code, TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

char *mseedDataFormatString(int code)
{
int i;
static char *DefaultMessage = "UNKNOWN_FORMAT";

    return LocateString(code, DataFormatMap, DefaultMessage);

}

char *mseedHdrString(MSEED_HDR *hdr, char *buf)
{
#define TBUFLEN 128
char tbuf1[TBUFLEN];
char tbuf2[TBUFLEN];
static char mt_unsafe[1024];

    if (buf == NULL) buf = mt_unsafe;

    buf[0] = 0;
    sprintf(buf+strlen(buf), "%06d ",   hdr->seqno);
    sprintf(buf+strlen(buf), "`%c' ",   hdr->flags.qc);
    sprintf(buf+strlen(buf), "%-2s ",   hdr->netid);
    sprintf(buf+strlen(buf), "%-5s ",   hdr->staid);
    sprintf(buf+strlen(buf), "%-3s ",   hdr->chnid);
    sprintf(buf+strlen(buf), "%-2s ",   hdr->locid);
    sprintf(buf+strlen(buf), "%s ",      utilTimeString(hdr->tstamp, 1000, tbuf1, TBUFLEN));
    if (hdr->tqual != -1) {
        sprintf(buf+strlen(buf), "%3d ", hdr->tqual);
    } else {
        sprintf(buf+strlen(buf), "--- ");
    }
    sprintf(buf+strlen(buf), "%s",      utilTimeString(hdr->endtime, 1000, tbuf2, TBUFLEN));
    sprintf(buf+strlen(buf), "%c ",     (hdr->flags.ioc & MSEED_IOC_CLOCK_LOCKED) ? ' ' : '*');
    sprintf(buf+strlen(buf), "%6.4lf ", (REAL64) hdr->nsint / NANOSEC_PER_SEC);
    sprintf(buf+strlen(buf), "%d=%-6s ",   hdr->format, mseedDataFormatString(hdr->format));
    sprintf(buf+strlen(buf), "0x%0x ",  hdr->flags.act);
    sprintf(buf+strlen(buf), "0x%0x ",  hdr->flags.ioc);
    sprintf(buf+strlen(buf), "0x%0x ",  hdr->flags.dat);
    sprintf(buf+strlen(buf), "%5d ",    hdr->nsamp);
    sprintf(buf+strlen(buf), "%5d",     hdr->reclen);
    if (hdr->flags.ioc & MSEED_IOC_SHORT_READ) sprintf(buf+strlen(buf), "*");

    return buf;
}

void mseedPrintHeader(FILE *fp, MSEED_HDR *hdr)
{
char tmpbuf[1024];

    fprintf(fp, "%s\n", mseedHdrString(hdr, tmpbuf));

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * $Log: print.c,v $
 * Revision 1.5  2015/09/15 23:20:53  dechavez
 * include format code together with format string in mseedHdrString()
 *
 * Revision 1.4  2014/10/29 21:21:39  dechavez
 * mseedDataFormatString() changed to only recognize MSEED_FORMAT_X codes
 *
 * Revision 1.3  2014/08/19 20:39:52  dechavez
 * don't bother with % sign when printing clock quality
 *
 * Revision 1.2  2014/08/19 18:01:05  dechavez
 * changed how time quality percentage is displaye
 *
 * Revision 1.1  2014/08/11 18:18:01  dechavez
 * initial release
 *
 */
