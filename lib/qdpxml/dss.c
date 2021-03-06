#pragma ident "$Id: dss.c,v 1.1 2009/10/20 22:08:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.dss
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_DP_TOKEN *token)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "highest"  ) == 0) strlcpy(token->dss.passwd.hi, value, QDP_DSS_PASSWD_LEN+1);
    else if (strcmp(tag, "middle"   ) == 0) strlcpy(token->dss.passwd.mi, value, QDP_DSS_PASSWD_LEN+1);
    else if (strcmp(tag, "lowest"   ) == 0) strlcpy(token->dss.passwd.lo, value, QDP_DSS_PASSWD_LEN+1);
    else if (strcmp(tag, "timeout"  ) == 0) token->dss.timeout   = (UINT32) atoi(value);
    else if (strcmp(tag, "maxbps"   ) == 0) token->dss.maxbps    = (UINT32) atoi(value);
    else if (strcmp(tag, "verbosity") == 0) token->dss.verbosity =  (UINT8) atoi(value);
    else if (strcmp(tag, "maxcpu"   ) == 0) token->dss.maxcpu    =  (UINT8) atoi(value);
    else if (strcmp(tag, "portnum"  ) == 0) token->dss.port      = (UINT16) atoi(value);
    else if (strcmp(tag, "maxmem"   ) == 0) token->dss.maxmem    = (UINT16) atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_tokensDss(MYXML_PARSER *xr, QDP_DP_TOKEN *token)
{
int result;
char *mark;
       
    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, token);
            break;
          default:
            token->dss.valid = TRUE;
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
 * $Log: dss.c,v $
 * Revision 1.1  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
