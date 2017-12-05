#pragma ident "$Id: balecfg.c,v 1.1 2016/01/21 17:49:37 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.balecfg
 *
 *====================================================================*/
#include "protos.h"
    
static void set_value(MYXML_PARSER *xr, QDP_TYPE_C3_BCFG *bcfg)
{
char *tag, *value;
    
    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;
    
         if (strcmp(tag, "phyport"  ) == 0) bcfg->phyport   = (UINT16) atoi(value);
    else if (strcmp(tag, "balertype") == 0) bcfg->balertype = (UINT16) atoi(value);
    else if (strcmp(tag, "version"  ) == 0) bcfg->version   = (UINT16) atoi(value);
    else if (strcmp(tag, "freeform" ) == 0) strlcpy(bcfg->freeform, value, QDP_BCFG_MAX_FREEFORM_LEN+1);
}

int qdpXmlParse_balecfg(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int i, result;
char *mark;
       
    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &config->bcfg);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_BCFG;
            return result;
        }
    }
}
/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
 * $Log: balecfg.c,v $
 * Revision 1.1  2016/01/21 17:49:37  dechavez
 * created
 *
 */
