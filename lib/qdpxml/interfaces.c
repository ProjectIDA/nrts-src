#pragma ident "$Id: interfaces.c,v 1.3 2016/01/19 23:09:08 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.interfaces
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C1_PHY *phy)
{
int ivalue;
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;


         if (strcmp(tag, "ser1_baud"  ) == 0) phy->serial[0].baud = qdpBaudCode(atoi(value));
    else if (strcmp(tag, "ser1_enable") == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "ser1_hard"  ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "ser1_break" ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "ser1_rping" ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "ser1_rtcp"  ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "ser1_route" ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 6);
    else if (strcmp(tag, "ser1_rqping") == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 7);
    else if (strcmp(tag, "ser1_bootp" ) == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "ser1_unlock") == 0) phy->serial[0].flags |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "ser1_throt" ) == 0) phy->serial[0].throttle = (ivalue = atoi(value)) ? 1024000 / ivalue : 0;
    else if (strcmp(tag, "ser1_ip"    ) == 0) phy->serial[0].ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "ser2_baud"  ) == 0) phy->serial[1].baud = qdpBaudCode(atoi(value));
    else if (strcmp(tag, "ser2_enable") == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "ser2_hard"  ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "ser2_break" ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "ser2_rping" ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "ser2_rtcp"  ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "ser2_route" ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 6);
    else if (strcmp(tag, "ser2_rqping") == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 7);
    else if (strcmp(tag, "ser2_bootp" ) == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "ser2_unlock") == 0) phy->serial[1].flags |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "ser2_throt" ) == 0) phy->serial[1].throttle = (ivalue = atoi(value)) ? 1024000 / ivalue : 0;
    else if (strcmp(tag, "ser2_ip"    ) == 0) phy->serial[1].ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "ser3_baud"  ) == 0) phy->serial[2].baud = qdpBaudCode(atoi(value));
    else if (strcmp(tag, "ser3_enable") == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "ser3_hard"  ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "ser3_break" ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "ser3_rping" ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "ser3_rtcp"  ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "ser3_route" ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 6);
    else if (strcmp(tag, "ser3_rqping") == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 7);
    else if (strcmp(tag, "ser3_bootp" ) == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "ser3_unlock") == 0) phy->serial[2].flags |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "ser3_throt" ) == 0) phy->serial[2].throttle = (ivalue = atoi(value)) ? 1024000 / ivalue : 0;
    else if (strcmp(tag, "ser3_ip"    ) == 0) phy->serial[2].ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "eth_enable" ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "eth_always" ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 1);
    else if (strcmp(tag, "eth_rping"  ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "eth_rtcp"   ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "eth_route"  ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 6);
    else if (strcmp(tag, "eth_rqping" ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 7);
    else if (strcmp(tag, "eth_bootp"  ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "eth_unlock" ) == 0) phy->ethernet.flags |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "eth_ip"     ) == 0) phy->ethernet.ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "base_port"  ) == 0) phy->baseport = (UINT16) atoi(value);
    else if (strcmp(tag, "crc"        ) == 0) phy->crc = (UINT32) atoi(value);
    else XmlWarn(xr);
}

int qdpXmlParse_interfaces(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
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
            set_value(xr, &config->phy);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_INTERFACES;
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
 * $Log: interfaces.c,v $
 * Revision 1.3  2016/01/19 23:09:08  dechavez
 * changed ser[123]_throttle to ser[123]_throt
 *
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
