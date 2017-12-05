#pragma ident "$Id: gps.c,v 1.2 2009/10/20 22:08:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.gps
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C2_GPS *gps)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "mode"       ) == 0) gps->mode |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "export"     ) == 0) gps->mode |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "RS422"      ) == 0) gps->mode |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "Serial_DGPS") == 0) gps->mode |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "Net_DGPS"   ) == 0) gps->mode |= ((UINT8) atoi(value) << 6);
    else if (strcmp(tag, "power_mode" ) == 0) gps->flags = (UINT16) atoi(value);
    else if (strcmp(tag, "off_time"   ) == 0) gps->off_time = (UINT16) atoi(value);
    else if (strcmp(tag, "resync"     ) == 0) gps->resync  = (UINT16) (atof(value) / 0.15);
    else if (strcmp(tag, "max_on"     ) == 0) gps->max_on  = (UINT16) atoi(value);
    else if (strcmp(tag, "crc"        ) == 0) gps->crc.gps = (UINT32) atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_gps(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
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
            set_value(xr, &config->gps);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_GPS;
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
 * $Log: gps.c,v $
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
