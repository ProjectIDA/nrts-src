#pragma ident "$Id: glob.c,v 1.3 2016/01/19 23:03:54 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.global
 *
 * The fsdetect field, marked GUESS, was found in a third party XML file.
 * I have been unable to find any vendor documentation as to what it
 * means and where it is found but am GUESSing that it is a newly defined
 * bit in the aux/status/web flags bitmap (confusingly named samp_rates
 * to match the Quanterra structures).
 *
 *====================================================================*/
#include "protos.h"

static void set_value(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "basetag"      ) == 0) config->basetag = strdup(value);
    else if (strcmp(tag, "clock_to"     ) == 0) config->glob.clock_to = (UINT16) atoi(value);
    else if (strcmp(tag, "initial_vco"  ) == 0) config->glob.initial_vco = (UINT16) atoi(value);
    else if (strcmp(tag, "gps_backup"   ) == 0) config->glob.gps_backup = (UINT16) atoi(value);
    else if (strcmp(tag, "aux_rate"     ) == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 0);
    else if (strcmp(tag, "stat_rate"    ) == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 8);
    else if (strcmp(tag, "open_balepwr" ) == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 2);
    else if (strcmp(tag, "open_balelink") == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 3);
    else if (strcmp(tag, "show_sn"      ) == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 4);
    else if (strcmp(tag, "fsdetect"     ) == 0) config->glob.samp_rates |= ((UINT8) atoi(value) << 5); /* GUESS */
    else if (strcmp(tag, "gainmap1"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "gainmap2"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) <<  2);
    else if (strcmp(tag, "gainmap3"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) <<  4);
    else if (strcmp(tag, "gainmap4"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) <<  6);
    else if (strcmp(tag, "gainmap5"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "gainmap6"     ) == 0) config->glob.gain_map |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "filtmap1"     ) == 0) config->glob.filter_map |= ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "filtmap2"     ) == 0) config->glob.filter_map |= ((UINT8) atoi(value) <<  2);
    else if (strcmp(tag, "input1"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  0);
    else if (strcmp(tag, "input2"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  1);
    else if (strcmp(tag, "input3"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  2);
    else if (strcmp(tag, "input4"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  3);
    else if (strcmp(tag, "input5"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  4);
    else if (strcmp(tag, "input6"       ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  5);
    else if (strcmp(tag, "mux1"         ) == 0) config->glob.input_map |= ((UINT8) atoi(value) <<  8);
    else if (strcmp(tag, "mux2"         ) == 0) config->glob.input_map |= ((UINT8) atoi(value) << 10);
    else if (strcmp(tag, "dp_to"        ) == 0) config->glob.server_to = (UINT16) atoi(value);
    else if (strcmp(tag, "drift_tol"    ) == 0) config->glob.drift_tol = (UINT16) atoi(value);
    else if (strcmp(tag, "jump_filt"    ) == 0) config->glob.jump_filt = (UINT16) atoi(value);
    else if (strcmp(tag, "jump_thresh"  ) == 0) config->glob.jump_thresh = (UINT16) atoi(value);
    else if (strcmp(tag, "cal_offset"   ) == 0) config->glob.cal_offset = (UINT16) atoi(value);
    else if (strcmp(tag, "sensor_map"   ) == 0) config->glob.sensor_map = (UINT16) atoi(value);
    else if (strcmp(tag, "gps_cold"     ) == 0) config->glob.gps_cold = (UINT16) atoi(value);
    else if (strcmp(tag, "samp_phase"   ) == 0) config->glob.sampling_phase = (UINT16) (floor(((double) atoi(value) / 1.5626) + 0.5));
    else if (strcmp(tag, "offsets1"     ) == 0) config->glob.offset[0] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "offsets2"     ) == 0) config->glob.offset[1] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "offsets3"     ) == 0) config->glob.offset[2] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "offsets4"     ) == 0) config->glob.offset[3] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "offsets5"     ) == 0) config->glob.offset[4] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "offsets6"     ) == 0) config->glob.offset[5] = (UINT16) ((UINT32) atoi(value) / 100);
    else if (strcmp(tag, "gains1"       ) == 0) config->glob.gain[0] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "gains2"       ) == 0) config->glob.gain[1] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "gains3"       ) == 0) config->glob.gain[2] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "gains4"       ) == 0) config->glob.gain[3] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "gains5"       ) == 0) config->glob.gain[4] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "gains6"       ) == 0) config->glob.gain[5] = (UINT16) (atof(value) * 1024.0);
    else if (strcmp(tag, "web_port"     ) == 0) config->glob.web_port = (UINT16) atoi(value);
    else if (strcmp(tag, "user_tag"     ) == 0) config->glob.user_tag = (UINT16) atoi(value);
    else if (strcmp(tag, "crc"          ) == 0) config->glob.crc      = (UINT32) atoi(value);
    else XmlWarn(xr);
    
}

static void SetScaling(MYXML_PARSER *xr, UINT16 *scaling)
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
            for (i = 0; i < QDP_NFREQ; i++) {
                if (strcmp(tag, label[i]) == 0) scaling[i] = (UINT16) atof(value) * 1024.0;
            }
            break;
          default:
            return;
        }
    }
}

int qdpXmlParse_global(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config)
{
int result;
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;
                 if (strcmp(tag, "scaling1") == 0) SetScaling(xr, &config->glob.scaling[0][0]);
            else if (strcmp(tag, "scaling2") == 0) SetScaling(xr, &config->glob.scaling[1][0]);
            else if (strcmp(tag, "scaling3") == 0) SetScaling(xr, &config->glob.scaling[2][0]);
            else if (strcmp(tag, "scaling4") == 0) SetScaling(xr, &config->glob.scaling[3][0]);
            else if (strcmp(tag, "scaling5") == 0) SetScaling(xr, &config->glob.scaling[4][0]);
            else if (strcmp(tag, "scaling6") == 0) SetScaling(xr, &config->glob.scaling[5][0]);
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, config);
            break;
          default:
            config->set |= QDP_CONFIG_DEFINED_GLOBAL;
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
 * $Log: glob.c,v $
 * Revision 1.3  2016/01/19 23:03:54  dechavez
 * added fsdetect (with caveats)
 *
 * Revision 1.2  2009/10/20 22:08:34  dechavez
 * first production release
 *
 */
