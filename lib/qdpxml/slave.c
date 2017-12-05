#pragma ident "$Id: slave.c,v 1.2 2009/10/20 22:08:35 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.slave
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C1_SPP *spp)
{
int ivalue;
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "max_main" ) == 0) spp->max_main_current    = (UINT16)  atoi(value);
    else if (strcmp(tag, "min_off"  ) == 0) spp->min_off_time        = (UINT16)  atoi(value);
    else if (strcmp(tag, "min_ps"   ) == 0) spp->min_ps_voltage      = (UINT16) (atof(value) / 0.15);
    else if (strcmp(tag, "max_ant"  ) == 0) spp->max_antenna_current = (UINT16)  atoi(value);
    else if (strcmp(tag, "min_temp" ) == 0) spp->min_temp            = (UINT16)  atoi(value);
    else if (strcmp(tag, "max_temp" ) == 0) spp->max_temp            = (UINT16)  atoi(value);
    else if (strcmp(tag, "temp_hyst") == 0) spp->temp_hysteresis     = (UINT16)  atoi(value);
    else if (strcmp(tag, "volt_hyst") == 0) spp->volt_hysteresis     = (UINT16) (atof(value) / 0.15);
    else if (strcmp(tag, "def_vco"  ) == 0) spp->default_vco         = (UINT16)  atoi(value);
    else if (strcmp(tag, "crc"      ) == 0) spp->crc                 = (UINT32)  atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_slave(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
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
            set_value(xr, &config->spp);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_SLAVE;
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
 * $Log: slave.c,v $
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
