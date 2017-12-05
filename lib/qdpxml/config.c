#pragma ident "$Id: config.c,v 1.7 2016/01/21 17:49:45 dechavez Exp $"
/*======================================================================
 *
 *  Read configuration from XML file
 *
 *====================================================================*/
#include "qdp/xml.h"
#include "protos.h"

static int qdpXmlParse_config(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int result;
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {

        if ((result = XmlNextRecord(xr, mark)) != MYXML_RECORD_BEG) return result;
        if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;

             if (strcmp(tag, "writer"    ) == 0) qdpXmlParse_writer(xr, config);
        else if (strcmp(tag, "global"    ) == 0) qdpXmlParse_global(xr, config);
        else if (strcmp(tag, "interfaces") == 0) qdpXmlParse_interfaces(xr, config);
        else if (strcmp(tag, "data1"     ) == 0) qdpXmlParse_data(xr, config, QDP_LOGICAL_PORT_1);
        else if (strcmp(tag, "data2"     ) == 0) qdpXmlParse_data(xr, config, QDP_LOGICAL_PORT_2);
        else if (strcmp(tag, "data3"     ) == 0) qdpXmlParse_data(xr, config, QDP_LOGICAL_PORT_3);
        else if (strcmp(tag, "data4"     ) == 0) qdpXmlParse_data(xr, config, QDP_LOGICAL_PORT_4);
        else if (strcmp(tag, "sensctrl"  ) == 0) qdpXmlParse_sensctrl(xr, config);
        else if (strcmp(tag, "slave"     ) == 0) qdpXmlParse_slave(xr, config);
        else if (strcmp(tag, "advser1"   ) == 0) qdpXmlParse_phy2(xr, config, QDP_PP_SERIAL_1);
        else if (strcmp(tag, "advser2"   ) == 0) qdpXmlParse_phy2(xr, config, QDP_PP_SERIAL_2);
        else if (strcmp(tag, "advether"  ) == 0) qdpXmlParse_phy2(xr, config, QDP_PP_ETHERNET);
        else if (strcmp(tag, "gps"       ) == 0) qdpXmlParse_gps(xr, config);
        else if (strcmp(tag, "pll"       ) == 0) qdpXmlParse_pll(xr, config);
        else if (strcmp(tag, "automass"  ) == 0) qdpXmlParse_automass(xr, config);
        else if (strcmp(tag, "announce"  ) == 0) qdpXmlParse_announce(xr, config);
        else if (strcmp(tag, "balecfg"   ) == 0) qdpXmlParse_balecfg(xr, config);
        else if (strcmp(tag, "envproc"   ) == 0) qdpXmlParse_envproc(xr, config);
        else if (strcmp(tag, "tokens1"   ) == 0) qdpXmlParse_tokens(xr, config, QDP_MEM_TYPE_CONFIG_DP1);
        else if (strcmp(tag, "tokens2"   ) == 0) qdpXmlParse_tokens(xr, config, QDP_MEM_TYPE_CONFIG_DP2);
        else if (strcmp(tag, "tokens3"   ) == 0) qdpXmlParse_tokens(xr, config, QDP_MEM_TYPE_CONFIG_DP3);
        else if (strcmp(tag, "tokens4"   ) == 0) qdpXmlParse_tokens(xr, config, QDP_MEM_TYPE_CONFIG_DP4);
        else if (strcmp(tag, "web_page"  ) == 0) qdpXmlParse_webpage(xr, config);
        else XmlSkipRecord(xr);
    }

    return result;
}

static BOOL ParseConfig(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
char *mark;
int i, result;
static char *ConfigString = "Q330_Configuration";

    if (!qdpInitConfig(config)) return FALSE;

    mark = XmlCrntTag(xr);
    while ((result = XmlNextRecord(xr, mark)) == MYXML_RECORD_BEG) {
        if (strcmp(XmlCrntTag(xr), "Q330_Configuration") == 0) {
            result = qdpXmlParse_config(xr, config);
            break;
        } else {
            XmlSkipRecord(xr);
        }
    }

#define COMPLETE (QDP_DPORT_DEFINED_LOG | QDP_DPORT_DEFINED_TOKEN)
    for (i = 0; i < QDP_NLP; i++) config->dport[i].valid = (config->dport[i].set == COMPLETE) ? TRUE : FALSE;

    return (result == MYXML_COMPLETE || result == MYXML_RECORD_END) ? TRUE : FALSE;
}

BOOL qdpXmlConfigFromFILE(FILE *fp, QDP_TYPE_FULL_CONFIG *config, FILE *warn)
{
BOOL result;
MYXML_PARSER *xr;

    if ((xr = XmlOpenParserForFILE(fp, warn)) == NULL) return FALSE;
    result = ParseConfig(xr, config);
    XmlDestroyParser(xr);
    return result;
}

BOOL qdpXmlConfigFromFile(char *fname, QDP_TYPE_FULL_CONFIG *config, FILE *warn)
{
BOOL result;
MYXML_PARSER *xr;

    if (fname == NULL) return qdpXmlConfigFromFILE(stdin, config, warn);

    if ((xr = XmlOpenParserForFile(fname, warn)) == NULL) return FALSE;
    result = ParseConfig(xr, config);
    XmlDestroyParser(xr);
    return result;
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
 * $Log: config.c,v $
 * Revision 1.7  2016/01/21 17:49:45  dechavez
 * added <balecfg> and <envproc> support
 *
 * Revision 1.6  2015/12/04 23:04:00  dechavez
 * added missing proto.h #include to bring in local prototypes
 *
 * Revision 1.5  2012/06/24 17:54:54  dechavez
 * ParseConfig() sets dport[i].valid flag
 *
 * Revision 1.4  2010/12/22 23:48:33  dechavez
 * 1.1.0
 *
 * Revision 1.3  2009/11/05 18:43:05  dechavez
 * support NULL fname in qdpXmlConfigFromFile()
 *
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
