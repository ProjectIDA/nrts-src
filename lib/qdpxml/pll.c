#pragma ident "$Id: pll.c,v 1.2 2009/10/20 22:08:35 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.pll
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C2_GPS *gps)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "lock_usec"  ) == 0) gps->lock_usec = (UINT16) atoi(value);
    else if (strcmp(tag, "interval"   ) == 0) gps->interval = (UINT16) atoi(value);
    else if (strcmp(tag, "enab_pll"   ) == 0) gps->initial_pll |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "lock_2D"    ) == 0) gps->initial_pll |= ((UINT8) atoi(value) << 1);
    else if (strcmp(tag, "vco_temp"   ) == 0) gps->initial_pll |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "pfrac"      ) == 0) gps->pfrac = atof(value);
    else if (strcmp(tag, "vco_slope"  ) == 0) gps->vco_slope = atof(value);
    else if (strcmp(tag, "vco_icpt"   ) == 0) gps->vco_intercept = atof(value);
    else if (strcmp(tag, "max_ikm_rms") == 0) gps->max_ikm_rms = atof(value);
    else if (strcmp(tag, "ikm_weight" ) == 0) gps->ikm_weight = atof(value);
    else if (strcmp(tag, "km_weight"  ) == 0) gps->km_weight = atof(value);
    else if (strcmp(tag, "best_weight") == 0) gps->best_weight = atof(value);
    else if (strcmp(tag, "km_delta"   ) == 0) gps->km_delta = atof(value);
    else if (strcmp(tag, "crc"        ) == 0) gps->crc.pll = (UINT32) atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_pll(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
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
            config->set |= QDP_CONFIG_DEFINED_PLL;
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
 * $Log: pll.c,v $
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
