#pragma ident "$Id: envproc.c,v 1.2 2016/01/23 00:09:34 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.envproc
 *
 *====================================================================*/
#include "protos.h"
    
static void set_value(MYXML_PARSER *xr, QDP_TYPE_C2_EPCFG *epcfg)
{
char *tag, *value;
    
    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;
    
         if (strcmp(tag, "res1"       ) == 0) epcfg->res1    = (UINT32) atoi(value);
    else if (strcmp(tag, "res2"       ) == 0) epcfg->res2    = (UINT32) atoi(value);
    else if (strcmp(tag, "wxt1heat"   ) == 0) epcfg->flags1  = (UINT32) atoi(value);
    else if (strcmp(tag, "wxt2heat"   ) == 0) epcfg->flags2  = (UINT32) atoi(value);
    else if (strcmp(tag, "chancnt"    ) == 0) epcfg->chancnt = (UINT16) atoi(value);
    else if (strcmp(tag, "spare"      ) == 0) epcfg->spare   = (UINT16) atoi(value);
    /* goodness, I'm lazy! */
    else if (strcmp(tag, "chanmask0"  ) == 0) epcfg->entry[  0].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask1"  ) == 0) epcfg->entry[  1].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask2"  ) == 0) epcfg->entry[  2].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask3"  ) == 0) epcfg->entry[  3].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask4"  ) == 0) epcfg->entry[  4].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask5"  ) == 0) epcfg->entry[  5].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask6"  ) == 0) epcfg->entry[  6].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask7"  ) == 0) epcfg->entry[  7].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask8"  ) == 0) epcfg->entry[  8].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask9"  ) == 0) epcfg->entry[  9].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask10" ) == 0) epcfg->entry[ 10].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask11" ) == 0) epcfg->entry[ 11].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask12" ) == 0) epcfg->entry[ 12].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask13" ) == 0) epcfg->entry[ 13].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask14" ) == 0) epcfg->entry[ 14].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask15" ) == 0) epcfg->entry[ 15].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask16" ) == 0) epcfg->entry[ 16].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask17" ) == 0) epcfg->entry[ 17].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask18" ) == 0) epcfg->entry[ 18].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask19" ) == 0) epcfg->entry[ 19].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask20" ) == 0) epcfg->entry[ 20].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask21" ) == 0) epcfg->entry[ 21].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask22" ) == 0) epcfg->entry[ 22].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask23" ) == 0) epcfg->entry[ 23].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask24" ) == 0) epcfg->entry[ 24].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask25" ) == 0) epcfg->entry[ 25].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask26" ) == 0) epcfg->entry[ 26].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask27" ) == 0) epcfg->entry[ 27].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask28" ) == 0) epcfg->entry[ 28].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask29" ) == 0) epcfg->entry[ 29].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask30" ) == 0) epcfg->entry[ 30].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask31" ) == 0) epcfg->entry[ 31].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask32" ) == 0) epcfg->entry[ 32].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask33" ) == 0) epcfg->entry[ 33].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask34" ) == 0) epcfg->entry[ 34].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask35" ) == 0) epcfg->entry[ 35].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask36" ) == 0) epcfg->entry[ 36].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask37" ) == 0) epcfg->entry[ 37].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask38" ) == 0) epcfg->entry[ 38].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask39" ) == 0) epcfg->entry[ 39].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask40" ) == 0) epcfg->entry[ 40].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask41" ) == 0) epcfg->entry[ 41].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask42" ) == 0) epcfg->entry[ 42].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask43" ) == 0) epcfg->entry[ 43].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask44" ) == 0) epcfg->entry[ 44].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask45" ) == 0) epcfg->entry[ 45].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask46" ) == 0) epcfg->entry[ 46].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask47" ) == 0) epcfg->entry[ 47].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask48" ) == 0) epcfg->entry[ 48].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask49" ) == 0) epcfg->entry[ 49].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask50" ) == 0) epcfg->entry[ 50].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask51" ) == 0) epcfg->entry[ 51].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask52" ) == 0) epcfg->entry[ 52].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask53" ) == 0) epcfg->entry[ 53].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask54" ) == 0) epcfg->entry[ 54].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask55" ) == 0) epcfg->entry[ 55].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask56" ) == 0) epcfg->entry[ 56].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask57" ) == 0) epcfg->entry[ 57].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask58" ) == 0) epcfg->entry[ 58].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask59" ) == 0) epcfg->entry[ 59].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask60" ) == 0) epcfg->entry[ 60].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask61" ) == 0) epcfg->entry[ 61].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask62" ) == 0) epcfg->entry[ 62].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask63" ) == 0) epcfg->entry[ 63].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask64" ) == 0) epcfg->entry[ 64].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask65" ) == 0) epcfg->entry[ 65].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask66" ) == 0) epcfg->entry[ 66].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask67" ) == 0) epcfg->entry[ 67].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask68" ) == 0) epcfg->entry[ 68].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask69" ) == 0) epcfg->entry[ 69].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask70" ) == 0) epcfg->entry[ 70].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask71" ) == 0) epcfg->entry[ 71].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask72" ) == 0) epcfg->entry[ 72].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask73" ) == 0) epcfg->entry[ 73].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask74" ) == 0) epcfg->entry[ 74].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask75" ) == 0) epcfg->entry[ 75].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask76" ) == 0) epcfg->entry[ 76].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask77" ) == 0) epcfg->entry[ 77].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask78" ) == 0) epcfg->entry[ 78].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask79" ) == 0) epcfg->entry[ 79].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask80" ) == 0) epcfg->entry[ 80].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask81" ) == 0) epcfg->entry[ 81].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask82" ) == 0) epcfg->entry[ 82].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask83" ) == 0) epcfg->entry[ 83].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask84" ) == 0) epcfg->entry[ 84].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask85" ) == 0) epcfg->entry[ 85].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask86" ) == 0) epcfg->entry[ 86].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask87" ) == 0) epcfg->entry[ 87].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask88" ) == 0) epcfg->entry[ 88].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask89" ) == 0) epcfg->entry[ 89].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask90" ) == 0) epcfg->entry[ 90].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask91" ) == 0) epcfg->entry[ 91].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask92" ) == 0) epcfg->entry[ 92].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask93" ) == 0) epcfg->entry[ 93].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask94" ) == 0) epcfg->entry[ 94].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask95" ) == 0) epcfg->entry[ 95].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask96" ) == 0) epcfg->entry[ 96].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask97" ) == 0) epcfg->entry[ 97].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask98" ) == 0) epcfg->entry[ 98].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask99" ) == 0) epcfg->entry[ 99].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask100") == 0) epcfg->entry[100].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask101") == 0) epcfg->entry[101].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask102") == 0) epcfg->entry[102].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask103") == 0) epcfg->entry[103].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask104") == 0) epcfg->entry[104].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask105") == 0) epcfg->entry[105].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask106") == 0) epcfg->entry[106].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask107") == 0) epcfg->entry[107].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask108") == 0) epcfg->entry[108].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask109") == 0) epcfg->entry[109].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask110") == 0) epcfg->entry[110].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask111") == 0) epcfg->entry[111].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask112") == 0) epcfg->entry[112].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask113") == 0) epcfg->entry[113].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask114") == 0) epcfg->entry[114].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask115") == 0) epcfg->entry[115].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask116") == 0) epcfg->entry[116].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask117") == 0) epcfg->entry[117].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask118") == 0) epcfg->entry[118].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask119") == 0) epcfg->entry[119].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask120") == 0) epcfg->entry[120].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask121") == 0) epcfg->entry[121].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask122") == 0) epcfg->entry[122].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask123") == 0) epcfg->entry[123].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask124") == 0) epcfg->entry[124].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask125") == 0) epcfg->entry[125].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask126") == 0) epcfg->entry[126].raw = (UINT16) atoi(value);
    else if (strcmp(tag, "chanmask127") == 0) epcfg->entry[127].raw = (UINT16) atoi(value);
}

int qdpXmlParse_envproc(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
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
            set_value(xr, &config->epcfg);
            break;
          default:
            for (i = 0; i < QDP_MAX_EP_CHANNEL; i++) qdpDecodeRawEpChanmask(&config->epcfg.entry[i]);
            config->set |= QDP_CONFIG_DEFINED_EPCFG;
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
 * $Log: envproc.c,v $
 * Revision 1.2  2016/01/23 00:09:34  dechavez
 * changes related to reworking of QDP_TYPE_C2_EPCFG structure layout
 *
 * Revision 1.1  2016/01/21 17:49:37  dechavez
 * created
 *
 */
