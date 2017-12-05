#pragma ident "$Id: announce.c,v 1.2 2009/10/20 22:08:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.announce
 *
 *====================================================================*/
#include "protos.h"

static void SetAnncEntryValue(MYXML_PARSER *xr, QDP_TYPE_C3_ANNC_ENTRY *entry)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "dp_ip"    ) == 0) entry->dp_ip     = ntohl(inet_addr(value));
    else if (strcmp(tag, "router_ip") == 0) entry->router_ip = ntohl(inet_addr(value));
    else if (strcmp(tag, "timeout"  ) == 0) entry->timeout   = (UINT16) atoi(value);
    else if (strcmp(tag, "resume"   ) == 0) entry->resume    = (UINT16) atoi(value);
    else if (strcmp(tag, "dp_port"  ) == 0) entry->dp_port   = (UINT16) atoi(value);
    else if (strcmp(tag, "log1"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "log2"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 1);
    else if (strcmp(tag, "log3"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "log4"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "ser1"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "ser2"     ) == 0) entry->flags    |= ((UINT8) atoi(value) << 5);
    else if (strcmp(tag, "eth"      ) == 0) entry->flags    |= ((UINT8) atoi(value) << 7);
    else if (strcmp(tag, "ignore"   ) == 0) entry->flags    |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "random"   ) == 0) entry->flags    |= ((UINT8) atoi(value) << 9);
    else XmlWarn(xr);
}

static void SetAnnounceEntry(MYXML_PARSER *xr, QDP_TYPE_C3_ANNC_ENTRY *entry)
{
int i;
char *mark;

	mark = XmlCrntTag(xr);
    while (1) {
        switch (XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            SetAnncEntryValue(xr, entry);
            break;
          default:
            return;
        }
    }
}

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C3_ANNC *annc)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "dps_active" ) == 0) annc->dps_active = (UINT16) atoi(value);
    else if (strcmp(tag, "ser1_unlock") == 0) annc->flags      = ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "ser2_unlock") == 0) annc->flags      = ((UINT8) atoi(value) <<  9);
    else if (strcmp(tag, "eth_unlock" ) == 0) annc->flags      = ((UINT8) atoi(value) << 11);
    else XmlWarn(xr);
}

int qdpXmlParse_announce(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int i, result;
char *mark, *tag, tst[] = "dp + slop";

    config->set |= QDP_CONFIG_DEFINED_ANNOUNCE;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            tag = XmlCrntTag(xr);
            for (i = 0; i < QDP_MAX_ANNC; i++) {
                sprintf(tst, "dp%d", i+1);
                if (strcmp(tag, tst) == 0) SetAnnounceEntry(xr, &config->annc.entry[i]);
            }
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &config->annc);
            break;
          default:
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
 * $Log: announce.c,v $
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
