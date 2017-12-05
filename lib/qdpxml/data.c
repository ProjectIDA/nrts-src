#pragma ident "$Id: data.c,v 1.4 2012/06/24 17:58:51 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.data
 *
 *====================================================================*/
#include "protos.h"

static void SetFreqMapBitmap(MYXML_PARSER *xr, UINT16 *bitmask)
{
int i;
char *mark, *tag, *value;
static char *label[QDP_NFREQ] = { "Hz1", "Hz10", "Hz20", "Hz40", "Hz50", "Hz100", "Hz200", "notdef" };

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
            for (i = 0; i < QDP_NFREQ; i++) if (strcmp(label[i], tag) == 0) *bitmask |= ((UINT8) atoi(value) << i);
            break;
          default:
            return;
        }
    }
}

static void set_value(MYXML_PARSER *xr, QDP_TYPE_C1_LOG *log)
{
int ivalue;
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "lport"      ) == 0) log->port         = (UINT16) atoi(value) + 1;
    else if (strcmp(tag, "flooding"   ) == 0) log->flags        = ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "freeze_out" ) == 0) log->flags        = ((UINT8) atoi(value) <<  1);
    else if (strcmp(tag, "freeze_in"  ) == 0) log->flags        = ((UINT8) atoi(value) <<  2);
    else if (strcmp(tag, "keepold"    ) == 0) log->flags        = ((UINT8) atoi(value) <<  3);
    else if (strcmp(tag, "piggyback"  ) == 0) log->flags        = ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "flushwarn"  ) == 0) log->flags        = ((UINT8) atoi(value) <<  9);
    else if (strcmp(tag, "hotswap"    ) == 0) log->flags        = ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "base96"     ) == 0) log->flags        = ((UINT8) atoi(value) << 13);
    else if (strcmp(tag, "perc"       ) == 0) log->perc         = floor((UINT16) (atof(value) * 2.56) + 0.5);
    else if (strcmp(tag, "mtu"        ) == 0) log->mtu          = (UINT16) atoi(value);
    else if (strcmp(tag, "grp_cnt"    ) == 0) log->group_cnt    = (UINT16) atoi(value);
    else if (strcmp(tag, "rsnd_max"   ) == 0) log->rsnd_max     = (UINT16) (atof(value) * 10.0);
    else if (strcmp(tag, "grp_to"     ) == 0) log->grp_to       = (UINT16) (atof(value) * 10.0);
    else if (strcmp(tag, "rsnd_min"   ) == 0) log->rsnd_min     = (UINT16) (atof(value) * 10.0);
    else if (strcmp(tag, "window"     ) == 0) log->window       = (UINT16) atoi(value);
    else if (strcmp(tag, "ack_cnt"    ) == 0) log->ack_cnt      = (UINT16) atoi(value);
    else if (strcmp(tag, "ack_to"     ) == 0) log->ack_to       = (UINT16) (atof(value) * 10.0);
    else if (strcmp(tag, "eth_throt"  ) == 0) log->eth_throttle = (ivalue = atoi(value)) ? (1024000 / ivalue) : 0;
    else if (strcmp(tag, "full_alert" ) == 0) log->full_alert   = (UINT16) floor((atof(value) * 2.56) + 0.5);
    else if (strcmp(tag, "auto_filter") == 0) log->auto_filter  = (UINT16) atoi(value);
    else if (strcmp(tag, "man_filter" ) == 0) log->man_filter   = (UINT16) atoi(value);
    else if (strcmp(tag, "crc"        ) == 0) log->crc          = (UINT32) atoi(value);
    else XmlWarn(xr);
}

static int Parse_C1_LOG(MYXML_PARSER *xr, QDP_TYPE_C1_LOG *log)
{
int i, result;
char tst[] = "freq_mapX + slop";
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) MYXML_ERROR;
            for (i = 0; i < QDP_NCHAN; i++) {
                sprintf(tst, "freq_map%d", i+1);
                if (strcmp(tag, tst) == 0) SetFreqMapBitmap(xr, &log->freqs[i]);
            }
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, log);
            break;
          default:
            return result;
        }
    }
}

int qdpXmlParse_data(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int id)
{
int index;

    switch (id) {
      case QDP_LOGICAL_PORT_1: index = 0; break;
      case QDP_LOGICAL_PORT_2: index = 1; break;
      case QDP_LOGICAL_PORT_3: index = 2; break;
      case QDP_LOGICAL_PORT_4: index = 3; break;
      default:
        XmlWarn(xr);
        return XmlSkipRecord(xr);
    }

    config->dport[index].set |= QDP_DPORT_DEFINED_LOG;
    return Parse_C1_LOG(xr, &config->dport[index].log);
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
 * $Log: data.c,v $
 * Revision 1.4  2012/06/24 17:58:51  dechavez
 * config->dport[i].token instead of config->token[i]
 *
 * Revision 1.3  2010/12/22 23:48:33  dechavez
 * 1.1.0
 *
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
