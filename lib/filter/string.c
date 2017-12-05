#pragma ident "$Id: string.c,v 1.5 2015/11/04 23:17:10 dechavez Exp $"
/*======================================================================
 *
 * String conversions
 *
 *====================================================================*/
#include "filter.h"

typedef struct {
    char *text;
    int   code;
} FILTER_TEXT_MAP;

static FILTER_TEXT_MAP TypeMap[] = {
    {        "analog", FILTER_TYPE_ANALOG  },
    {           "IIR", FILTER_TYPE_IIR_PZ  },
    {       "Laplace", FILTER_TYPE_LAPLACE },
    {         "combo", FILTER_TYPE_COMB    },
    { "symmetric FIR", FILTER_TYPE_FIR_SYM },
    {"asymmetric FIR", FILTER_TYPE_FIR_ASYM},
    {          "none", 0                   }
};

static FILTER_TEXT_MAP UnitsMap[] = {
   {      "M", FILTER_UNITS_DIS    },
   {    "M/S", FILTER_UNITS_VEL    },
   { "M/S**2", FILTER_UNITS_ACC    },
   {  "M/S/S", FILTER_UNITS_ACC    },
   { "COUNTS", FILTER_UNITS_COUNTS },
   {     "PA", FILTER_UNITS_PASCALS},
   {      "1", FILTER_UNITS_STRAIN },
   {      "T", FILTER_UNITS_TESLAS },
   {   "USEC", FILTER_UNITS_USEC   },
   {   "TILT", FILTER_UNITS_RADIANS},
   {      "V", FILTER_UNITS_VOLTS  },
   {"PERCENT", FILTER_UNITS_PERCENT},
   {     NULL, 0                   }
};

static char *LocateString(int code, FILTER_TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

static int LocateCode(char *string, FILTER_TEXT_MAP *map, int DefaultCode)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (strcasecmp(map[i].text, string) == 0) return map[i].code;
    return DefaultCode;
}

static void SetPandZString(FILTER_PANDZ *pandz, char *buf)
{
    sprintf(buf+strlen(buf), " nzero=%d", pandz->nzero);
    sprintf(buf+strlen(buf), " npole=%d", pandz->npole);
}

static void SetCoeffString(FILTER_COEFF *coeff, char *buf)
{
    sprintf(buf+strlen(buf), " ncoef=%d", coeff->ncoef);
    sprintf(buf+strlen(buf), " delay=%lf", coeff->delay);
}

char *filterString(FILTER *filter, char *buf)
{
#define FILTER_STRING_LEN 255
static char mt_unsafe[FILTER_STRING_LEN+1];

    if (buf == NULL) buf = mt_unsafe;

    sprintf(buf, "type=0x%04x (%s)", filter->type, filterTypeString(filter->type));
    if (filter->type & FILTER_TYPE_PANDZ) {
        SetPandZString(&filter->data.pz, buf);
    } else if (filter->type & FILTER_TYPE_COEFF) {
        SetCoeffString(&filter->data.cf, buf);
    }

    return buf;
}

char *filterTypeString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, TypeMap, DefaultMessage);
}

int filterUnitsCode(char *string)
{
int DefaultCode = FILTER_UNITS_NULL;

    return LocateCode(string, UnitsMap, DefaultCode);
}

char *filterUnitsString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, UnitsMap, DefaultMessage);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2015 Regents of the University of California            |
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
 * $Log: string.c,v $
 * Revision 1.5  2015/11/04 23:17:10  dechavez
 * define SetPandZString() as void (oversight)
 *
 * Revision 1.4  2015/09/30 20:19:51  dechavez
 * added support for FILTER_UNITS_PERCENT
 *
 * Revision 1.3  2015/09/01 18:48:26  dechavez
 * print filter type code in hex
 *
 * Revision 1.2  2015/08/24 18:42:58  dechavez
 * added filterUnitsString()
 *
 * Revision 1.1  2015/07/15 17:29:40  dechavez
 * created
 *
 */
