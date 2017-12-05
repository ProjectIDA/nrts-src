#pragma ident "$Id: version.c,v 1.5 2015/12/04 23:34:22 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "myxml.h"

static VERSION version = {1, 1, 7};

/* paro library release notes

1.1.7   12/05/2015
        myxml.c: fixed printf() format statement to calm OS X compiles
        parse.c: XmlOpenParserForFile() and XmlOpenParserForFILE() return NULL instead of FALSE, to calm OS X compiles

1.1.6   08/11/2014
        iir.c:    made ParseIirSection() static and void
        tokens.c: added missing return value in error return branch of ParseTokens()

1.1.5   10/20/2009
        parse.c: reworked parser to use improved handle

1.1.0   10/07/2009
        parse.c: created

1.0.0   10/02/2009
        myxml.h: created
        crc.c: created
        myxml.c: created
 */

char *myxmlVersionString()
{
static char string[] = "myxml library version 100.100.100 and slop";

    snprintf(string, strlen(string), "myxml library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *myxmlVersion()
{
    return &version;
}
