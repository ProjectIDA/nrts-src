#pragma ident "$Id: webpage.c,v 1.4 2015/12/04 23:15:12 dechavez Exp $"
/*======================================================================
 * 
 * Webpage stuff
 *
 *====================================================================*/
#include "qdp.h"

#define MAXLINELEN 8192

BOOL qdpRequestWebpage(QDP *qdp, LNKLST *list)
{
int i, j, k;
char *macro;
QDP_MEMBLK blk;
char line[MAXLINELEN+1];
BOOL ready = FALSE;

    if (!qdp_C1_RQMEM(qdp, QDP_MEM_TYPE_WEBPAGE, &blk)) return FALSE;

    line[0] = 0;
    for (i = 0, j = 0; i < blk.nbyte; i++) {
        if (!ready && blk.data[i] == '<') ready = TRUE;
        if (!ready) continue; /* ignore everything before the first < */
        if (blk.data[i] == 0x0d) continue; /* ignore carriage returns */
        if (blk.data[i] != 0x0a) {  /* use new lines to terminate current line of text */
            if ((macro = qdpWebPageMacro(blk.data[i])) == NULL) {
                line[j++] = blk.data[i];
            } else {
                for (k = 0; k < strlen(macro); k++) line[j++] = macro[k];
            }
        } else {
            line[j] = 0;
            if (!listAppend(list, line, strlen(line)+1)) return FALSE;
            j = 0;
        }
    }

/* In case web page didn't have a final end of line */

    if (j != 0) {
        line[j] = 0;
        if (!listAppend(list, line, strlen(line)+1)) return FALSE;
        j = 0;
    }

    if (!listSetArrayView(list)) return FALSE;

    return TRUE;
}

static int PackLine(UINT8 *start, char *line)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackBytes(ptr, (UINT8 *) line, strlen(line));
    *ptr++ = 0x0d;
    *ptr++ = 0x0a;

    return (int) (ptr - start);
}

static BOOL CheckForBaseTag(char *line)
{
static char *BaseTagPrefix = "<h3>Configuration Base Tag: ";

    return strncmp(line, BaseTagPrefix, strlen(BaseTagPrefix)) == 0 ? TRUE : FALSE;

}

static int CheckForTokenSetTag(char *line)
{
int i;
char *beg, *end;
char begtag[] = "<!-- TS1 --> plus slop";
char endtag[] = "<!-- /TS1 --> plus slop";

    for (i = 0; i < QDP_NLP; i++) {
        sprintf(begtag, "<!-- TS%d -->", i+1);
        sprintf(endtag, "<!-- /TS%d -->", i+1);
        beg = strstr(line, begtag);
        end = strstr(line, endtag);
        if (beg != NULL && end != NULL && end > beg) return i+1;
    }

    return 0;
}

static BOOL IsMacro(char *in, UINT8 *code, int *len)
{
int i;
static QDP_TEXT_MAP WebPageMacro[] = QDP_WEB_PAGE_MAP;

    for (i = 0; WebPageMacro[i].text != NULL; i++) {
        if (strncmp(in, WebPageMacro[i].text, strlen(WebPageMacro[i].text)) == 0) {
            *code = WebPageMacro[i].code;
            *len = strlen(WebPageMacro[i].text);
            return TRUE;
        }
    }

    return FALSE;
}

static void SubstituteMacros(char *line, char *newtxt)
{
UINT8 code;
int len;
char *in, *out;
    
    in = line;
    out = newtxt;

    while (*in != 0) {
        if (IsMacro(in, &code, &len)) {
            *out++ = code;
            in += len;
        } else {
            *out++ = *in++;
        }
    }
    *out++ = 0;
}
static void PRINT(char *prefix, char *string)
{
    printf("%s", prefix);
    while (*string != 0) {
        if (isprint(*string)) {
            printf("%c", *string);
        } else {
            printf("@0x%02x@", *string);
        }
        ++string;
    }
    printf("\n");
}

int qdpPackWebpageMemblk(QDP_MEMBLK *dest, QDP_TYPE_FULL_CONFIG *config)
{
int i, ts;
UINT8 *ptr;
char *line, newtxt[MAXLINELEN+1], *tokenset, undefined[] = "undefined";
static char *prefix[] = {
    "HTTP/1.0 200 OK",
    "Server: Q330 V1.0",
    "Content-type: text/html",
    "Cache-Control: no-cache",
    "Pragma: no-cache",
    NULL
};

    if (dest == NULL || config == NULL) {
        errno = EINVAL;
        return -1;
    }

    ptr = dest->data;
    dest->nbyte = 0;
    dest->type = QDP_MEM_TYPE_WEBPAGE;

    for (i = 0; prefix[i] != NULL; i++) {
        ptr += utilPackBytes(ptr, (UINT8 *) prefix[i], strlen(prefix[i]));
        *ptr++ = 0x0d;
        *ptr++ = 0x0a;
    }

    for (i = 0; i < config->webpage.count; i++) {
        line = (char *) config->webpage.array[i];
        if (strcmp(line, "<![CDATA[") == 0) continue;
        if (strncmp(line, "<crc>", strlen("<crc>")) == 0) continue;
        if (strcmp(line, "]]>") == 0) continue;
        if (config->basetag != NULL && CheckForBaseTag(line)) {
            sprintf(newtxt, "<h3>Configuration Base Tag: <!-- CBT -->%s<!-- /CBT --></h3>", config->basetag);
        } else if ((ts = CheckForTokenSetTag(line)) != 0) {
            sprintf(newtxt, "<h3>Token Set Name: <!-- TS%d -->%s<!-- /TS%d --></h3>", ts, config->dport[ts-1].token.setname, ts);
        } else {
            SubstituteMacros(line, newtxt);
        }
        ptr += PackLine(ptr, newtxt);
    }
   
    dest->nbyte = (INT16) (ptr - dest->data);

    return dest->nbyte;
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
 * Revision 1.4  2015/12/04 23:15:12  dechavez
 * casts, format fixes and the like, all to calm OS X compiles
 *
 * Revision 1.3  2012/06/24 18:09:24  dechavez
 * config->dport[i].token instead of config->token[i]
 *
 * Revision 1.2  2009/11/05 18:30:53  dechavez
 * in qdpPackWebpageMemblk() set missing type, allow for NULL set names
 *
 * Revision 1.1  2009/10/29 17:42:32  dechavez
 * initial release
 *
 */
