#pragma ident "$Id: xml.c,v 1.7 2016/02/03 17:40:57 dechavez Exp $"
/*======================================================================
 *
 *  Read Q330 configuration and token data and generate XML file
 *
 *====================================================================*/
#include "q330cc.h"
#include "myxml.h"

void XmlHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ -f  ]\n", name);
    printf("\n");
    printf("By default the output goes to the screen (stdout), but you can have it go\n");
    printf("to a file with the standard name of staXX-yyyymmdd.xml using the -f option.\n");
}

BOOL VerifyXml(Q330 *q330)
{
char *arg;

    q330->cmd.code = Q330_CMD_XML;
    if (q330->cmd.arglst->count == 0) {
        q330->cmd.p32 = 0;
        return TRUE;
    }

    if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);
    arg = (char *) q330->cmd.arglst->array[0];
    if (strcmp(arg, "-f") != 0) return UnexpectedArg(q330, arg);
    q330->cmd.p32 = 1;

    return TRUE;
}

static void BuildStandardFileName(char *name, char *path)
{
time_t now;
struct tm result;

    now = time(NULL);
    localtime_r(&now, &result);
    sprintf(path, "%s-%04d%02d%02d.xml", name, result.tm_year+1900, result.tm_mon+1, result.tm_mday);
}

BOOL xml(Q330 *q330)
{
MYXML_HANDLE *xp;
char path[MAXPATHLEN+1];
QDP_TYPE_FULL_CONFIG config;
char basetag[128];

    if (q330->cmd.p32 == 0) {
        path[0] = 0;
    } else {
        BuildStandardFileName(q330->addr.connect.ident, path);
    }

    if ((xp = XmlOpen(path)) == NULL) {
        perror("XmlOpen");
        return FALSE;
    }

    if (!qdpRequestFullConfig(q330->qdp, &config)) {
        printf("qdpRequestFullConfig failed\n");
        return FALSE;
    }

    basetag[0] = 0;
    sprintf(basetag+strlen(basetag), "%s", q330->addr.connect.ident);
    sprintf(basetag+strlen(basetag), " %4u", config.fix.proper_tag);
    sprintf(basetag+strlen(basetag), " %016llX", config.fix.sys_num);
    sprintf(basetag+strlen(basetag), " %d.%03d", config.fix.sys_ver.major, config.fix.sys_ver.minor);
    sprintf(basetag+strlen(basetag), " %d.%02d", config.fix.sp_ver.major, config.fix.sp_ver.minor);

    qdpXmlPrintFullConfig(xp, &config, basetag);
    if (strlen(path) != 0) printf("Configuration data written to %s\n", path);
    XmlClose(xp);

    return TRUE;
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
 * $Log: xml.c,v $
 * Revision 1.7  2016/02/03 17:40:57  dechavez
 * changes related to the introduction of QDP_SOFTVER structures for version numbers
 *
 * Revision 1.6  2014/08/11 19:18:32  dechavez
 * changed format from %4lu to %4u  when printing config.fix.proper_tag
 *
 * Revision 1.5  2011/01/25 18:26:15  dechavez
 * Use new QDP_CONNECT "connect" field in Q330_ADDR
 *
 * Revision 1.4  2009/10/20 22:13:09  dechavez
 * added myxml.h include
 *
 * Revision 1.3  2009/10/02 19:25:16  dechavez
 * initial production release
 *
 * Revision 1.2  2009/09/28 22:13:24  dechavez
 * Initial release.  Still missing <controldet>, and <netevt> and <noncomp>
 * are hardcoded.  CRCs are stubbed as zero.
 *
 * Revision 1.1  2009/09/22 19:46:08  dechavez
 * checkpoint
 *
 */
