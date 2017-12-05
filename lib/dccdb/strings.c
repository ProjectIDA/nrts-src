#pragma ident "$Id: strings.c,v 1.3 2015/09/30 20:10:45 dechavez Exp $"
/*======================================================================
 *
 *  Useful strings
 *
 *====================================================================*/
#include "dccdb.h"

typedef struct {
    char *text;
    int   code;   
} DCCDB_TEXT_MAP;

static DCCDB_TEXT_MAP TableMap[] = {
    { DCCDB_ABBREV_TABLE_NAME, DCCDB_TABLE_ABBREV },
    {   DCCDB_CHAN_TABLE_NAME, DCCDB_TABLE_CHAN   },
    {DCCDB_SEEDLOC_TABLE_NAME, DCCDB_TABLE_SEEDLOC},
    {   DCCDB_SITE_TABLE_NAME, DCCDB_TABLE_SITE   },
    {  DCCDB_STAGE_TABLE_NAME, DCCDB_TABLE_STAGE  },
    {  DCCDB_UNITS_TABLE_NAME, DCCDB_TABLE_UNITS  },
    {                    NULL, 0                  }       
};

static char *LocateString(int code, DCCDB_TEXT_MAP *map, char *DefaultString)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultString;
}

static int LocateCode(char *string, DCCDB_TEXT_MAP *map, int DefaultCode)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (strcasecmp(map[i].text, string) == 0) return map[i].code;
    return DefaultCode;
}

char *dccdbTableName(int code)
{
static char *DefaultString = NULL;

    return LocateString(code, TableMap, DefaultString);
}

int dccdbTableCode(char *string)
{
static int DefaultCode = DCCDB_TABLE_NULL;

    return LocateCode(string, TableMap, DefaultCode);
}

char *dccdbChanString(DCCDB_CHAN *chan, char *buf)
{
static char mt_unsafe[1024], timstr1[1024], timstr2[1024];

    if (buf == NULL) buf = mt_unsafe;

    sprintf(buf, "sta=%s, chn=%s, loc=%s, time=%19.7lf=%s, endtime=%19.7lf=%s",
        chan->sta,
        chan->chn,
        chan->loc,
        chan->begt,
        utilDttostr(chan->begt, 0, timstr1),
        chan->endt,
        utilDttostr(chan->endt, 0, timstr2)
    );

    return buf;
}

char *dccdbStageString(DCCDB_STAGE *stage, char *buf)
{
static char mt_unsafe[1024], timstr1[1024], timstr2[1024];

    if (buf == NULL) buf = mt_unsafe;

    sprintf(buf, "sta=%s, chn=%s, loc=%s, begt=%s, endt=%s, stage=%d",
        stage->sta,
        stage->chn,
        stage->loc,
        utilDttostr(stage->begt, 0, timstr1),
        utilDttostr(stage->endt, 0, timstr2),
        stage->stageid
    );

    return buf;
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
 * $Log: strings.c,v $
 * Revision 1.3  2015/09/30 20:10:45  dechavez
 * removed isduplicate field support from dccdbStageString()
 *
 * Revision 1.2  2015/07/10 18:19:52  dechavez
 * added dccdbTableName(), dccdbTableCode()
 *
 * Revision 1.1  2015/03/06 23:16:41  dechavez
 * initial release
 *
 */
