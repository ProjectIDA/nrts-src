#pragma ident "$Id: phy2.c,v 1.2 2012/08/07 21:24:57 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.advser1, advser2, and advether
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C2_PHY *phy)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "phynum"     ) == 0) phy->phynum = (UINT16) atoi(value);
    else if (strcmp(tag, "lognum"     ) == 0) phy->lognum = (UINT16) atoi(value);
    else if (strcmp(tag, "modem_init" ) == 0) strlcpy(phy->modem_init, value, QDP_PHY2_MODEM_INIT_LEN+1);
    else if (strcmp(tag, "user_330"   ) == 0) strlcpy(phy->dial_in.name, value, QDP_PHY2_IN_NAME_LEN+1);
    else if (strcmp(tag, "pass_330"   ) == 0) strlcpy(phy->dial_in.pass, value, QDP_PHY2_IN_PASS_LEN+1);
    else if (strcmp(tag, "trigger"    ) == 0) phy->trigger = floor((atof(value) * 2.56) + 0.5);
    else if (strcmp(tag, "ppp"        ) == 0) phy->flags |= ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "dial"       ) == 0) phy->flags |= ((UINT8) atoi(value) <<  1);
    else if (strcmp(tag, "dial_ctrl"  ) == 0) phy->flags |= ((UINT8) atoi(value) <<  2);
    else if (strcmp(tag, "baler_ctrl" ) == 0) phy->flags |= ((UINT8) atoi(value) <<  4);
    else if (strcmp(tag, "at_end"     ) == 0) phy->flags |= ((UINT8) atoi(value) <<  6);
    else if (strcmp(tag, "at_trigger" ) == 0) phy->flags |= ((UINT8) atoi(value) <<  7);
    else if (strcmp(tag, "at_interval") == 0) phy->flags |= ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "baler_cmds" ) == 0) phy->flags |= ((UINT8) atoi(value) <<  9);
    else if (strcmp(tag, "DTR"        ) == 0) phy->flags |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "NAPT"       ) == 0) phy->flags |= ((UINT8) atoi(value) << 11);
    else if (strcmp(tag, "transparent") == 0) phy->flags |= ((UINT8) atoi(value) << 12);
    else if (strcmp(tag, "baler44"    ) == 0) phy->flags |= ((UINT8) atoi(value) << 13);
    else if (strcmp(tag, "simplex"    ) == 0) phy->flags |= ((UINT8) atoi(value) << 15);
    else if (strcmp(tag, "retry_int"  ) == 0) phy->retry_int = (UINT16) atoi(value);
    else if (strcmp(tag, "interval"   ) == 0) phy->interval = (UINT16) atoi(value);
    else if (strcmp(tag, "webbps"     ) == 0) phy->webbps = (UINT16) atoi(value);
    else if (strcmp(tag, "retries"    ) == 0) phy->retries = (UINT16) atoi(value);
    else if (strcmp(tag, "reg_to"     ) == 0) phy->reg_to = (UINT16) atoi(value);
    else if (strcmp(tag, "routed_to"  ) == 0) phy->routed_to = (UINT16) atoi(value);
    else if (strcmp(tag, "ss_rate"    ) == 0) phy->ss |= ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "ss_fracdig" ) == 0) phy->ss |= ((UINT8) atoi(value) <<  4);
    else if (strcmp(tag, "ss_temp"    ) == 0) phy->ss |= ((UINT8) atoi(value) <<  7);
    else if (strcmp(tag, "ss_units"   ) == 0) phy->ss |= ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "ss_type"    ) == 0) phy->ss |= ((UINT8) atoi(value) << 12);
    else if (strcmp(tag, "serial_baud") == 0) phy->baud = (UINT16) atoi(value);
    else if (strcmp(tag, "phone"      ) == 0) strlcpy(phy->phone_number, value, QDP_PHY2_PHONE_NUMBER_LEN+1);
    else if (strcmp(tag, "user_isp"   ) == 0) strlcpy(phy->dial_out.name, value, QDP_PHY2_OUT_NAME_LEN+1);
    else if (strcmp(tag, "pass_isp"   ) == 0) strlcpy(phy->dial_out.pass, value, QDP_PHY2_OUT_PASS_LEN+1);
    else if (strcmp(tag, "poc_ip"     ) == 0) phy->poc_ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "log2_ip"    ) == 0) phy->log2_ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "poc_port"   ) == 0) phy->poc_port = (UINT16) atoi(value);
    else if (strcmp(tag, "crc"        ) == 0) phy->crc      = (UINT32) atoi(value);
    else XmlWarn(xr);
}

static int Parse_C2_PHY(MYXML_PARSER *xr, QDP_TYPE_C2_PHY *phy)
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
            set_value(xr, phy);
            break;
          default:
            return result;
        }
    }
}

int qdpXmlParse_phy2(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int index)
{
int result;

    switch (index) {
      case QDP_PP_SERIAL_1: config->set |= QDP_CONFIG_DEFINED_ADVSER1;  break;
      case QDP_PP_SERIAL_2: config->set |= QDP_CONFIG_DEFINED_ADVSER2;  break;
      case QDP_PP_ETHERNET: config->set |= QDP_CONFIG_DEFINED_ADVETHER; break;
      default:
        return MYXML_ERROR;
    }

    return Parse_C2_PHY(xr, &config->phy2[index]);
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
 * $Log: phy2.c,v $
 * Revision 1.2  2012/08/07 21:24:57  dechavez
 * added baler44 support
 *
 * Revision 1.1  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
