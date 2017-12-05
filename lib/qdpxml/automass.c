#pragma ident "$Id: automass.c,v 1.2 2009/10/20 22:08:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.automass
 *
 *====================================================================*/
#include "protos.h"

static void SetSensorValue(MYXML_PARSER *xr, QDP_TYPE_C2_AMASS_SENSOR *sensor)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "tol_A"      ) == 0) sensor->tolerance[0] = (UINT16) atoi(value);
    else if (strcmp(tag, "tol_B"      ) == 0) sensor->tolerance[1] = (UINT16) atoi(value);
    else if (strcmp(tag, "tol_C"      ) == 0) sensor->tolerance[2] = (UINT16) atoi(value);
    else if (strcmp(tag, "max_try"    ) == 0) sensor->maxtry = (UINT16) atoi(value);
    else if (strcmp(tag, "norm_int"   ) == 0) sensor->interval.normal = (UINT16) atoi(value);
    else if (strcmp(tag, "squelch_int") == 0) sensor->interval.squelch = (UINT16) atoi(value);
    else if (strcmp(tag, "sensor_map" ) == 0) sensor->bitmap = (UINT16) atoi(value);
    else if (strcmp(tag, "duration"   ) == 0) sensor->duration = atof(value) / 0.010;
    else XmlWarn(xr);
}

static void SetSensor(MYXML_PARSER *xr, QDP_TYPE_C2_AMASS_SENSOR *sensor)
{
char *mark;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            SetSensorValue(xr, sensor);
            break;
          default:
            return;
        }
    }
}

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C2_AMASS *amass)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

    if (strcmp(tag, "crc") == 0) amass->crc = (UINT32) atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_automass(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int result;
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            tag = XmlCrntTag(xr);
            if (strcmp(tag, "group1") == 0) SetSensor(xr, &config->amass.sensor[0]);
            if (strcmp(tag, "group2") == 0) SetSensor(xr, &config->amass.sensor[1]);
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &config->amass);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_AUTOMASS;
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
 * $Log: automass.c,v $
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
