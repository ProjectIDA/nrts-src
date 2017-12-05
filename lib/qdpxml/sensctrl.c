#pragma ident "$Id: sensctrl.c,v 1.2 2009/10/20 22:08:35 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.sensctrl
 *
 *====================================================================*/
#include "protos.h"

static void SetScMap(MYXML_PARSER *xr, UINT32 *bitmask)
{
char *mark, *tag, *value;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            if ((tag = XmlCrntTag(xr)) == NULL) return;
            if ((value = XmlCrntValue(xr)) == NULL) return;
                 if (strcmp(tag, "function") == 0) *bitmask |= ((UINT8) atoi(value) << 0);
            else if (strcmp(tag, "high"    ) == 0) *bitmask |= ((UINT8) atoi(value) << 8);
            else XmlWarn(xr);
            break;
          default:
            return;
        }
    }
}

int qdpXmlParse_sensctrl(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int i, result;
char *mark, *tag, tst[] = "mappingX plus slop";

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;
            for (i = 0; i < QDP_NSC; i++) {
                sprintf(tst, "mapping%d", i+1);
                if (strcmp(tag, tst) == 0) SetScMap(xr, &config->sc[i]);
            }
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;
            if (strcmp(tag, "crc") != 0) XmlWarn(xr); // ignore CRC
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_SENSCTRL;
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
 * $Log: sensctrl.c,v $
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
