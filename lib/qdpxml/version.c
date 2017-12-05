#pragma ident "$Id: version.c,v 1.12 2016/01/23 00:09:50 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "qdp/xml.h"

static VERSION version = {1, 4, 1};

/* qdpxml library release notes

1.4.1   01/22/2016
        envproc.c: changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
        print.c    changes related to reworking of QDP_TYPE_C2_EPCFG structure layout

1.4.0   01/21/2016
        qdp/xml.h: updated prototypes
        balecfg: created
        envproc: created
        config.c: added <balecfg> and <envproc> support
        print.c: added <balecfg> and <envproc> support
        protos.h: added qdpXmlParse_balecfg() and qdpXmlParse_envproc() prototypes

1.3.1   01/19/2016
        glob.c, print.c: added fsdetect (with caveats)
        print.c:    ser3_throt support
        interfaces: changed ser[123]_throttle to ser[123]_throt

1.3.0   08/06/2012
        phy2.c, print.c: added baler44 support

1.2.0   06/24/2012 (requires libqdp 3.4.0 or greater (Bound C1_LOG and tokens together with QDP_TYPE_DPORT))
        qdp/xml.h: updated qdpXmlPrint_C1_LOG() and qdpXmlPrintTokens() prototypes (pass index instead of tag)
        config.c: ParseConfig() sets dport[i].valid flag
        lcq.c: set derived fields frate, dsint, nsint
        data.c:   config->dport[i].token instead of config->token[i]
        print.c:  ditto
        tokens.c: ditto

1.1.1   01/13/2011
        detect.c:  qdpSetIdent() to qdpSetTokenIdent()
        lcq.c:          "                   "
        murdock.c:      "                   "
        thresh.c:       "                   "

1.1.0   12/22/2010
        config.c: use symbolic names for ports instead of hard coded numbers
        data.c:   use symbolic names for ports instead of hard coded numbers
        tokens.c: use symbolic names for ports instead of hard coded numbers

1.0.2   11/05/2009
        config.c: support NULL fname in qdpXmlConfigFromFile()
        tokens.c: set valid flag where needed
        webpage.c: use isspace() instead of isblank()

1.0.1   10/29/2009
        print.c: ignore records that don't have the valid field set
        webpage.c: removed tabs

1.0.0   10/20/2009
        first production release
 */

char *qdpXmlVersionString()
{
static char string[] = "qdpXml library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qdpXml library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qdpXmlVersion()
{
    return &version;
}
