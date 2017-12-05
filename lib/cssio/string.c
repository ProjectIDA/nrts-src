#pragma ident "$Id: string.c,v 1.2 2014/04/18 18:27:46 dechavez Exp $"
/*======================================================================
 *
 * String conversions
 *
 *====================================================================*/
#include "cssio.h"

typedef struct {
    char *text;
    int code;
} CSSIO_TEXT_MAP;

static CSSIO_TEXT_MAP FieldMap[] = {
    {     "sta", CSSIO_FIELD_STA     }, 
    {    "chan", CSSIO_FIELD_CHAN    },
    {    "time", CSSIO_FIELD_TIME    },
    {    "wfid", CSSIO_FIELD_WFID    },
    {  "chanid", CSSIO_FIELD_CHANID  },
    {   "jdate", CSSIO_FIELD_JDATE   },
    { "endtime", CSSIO_FIELD_ENDTIME },
    {   "nsamp", CSSIO_FIELD_NSAMP   },
    { "smprate", CSSIO_FIELD_SMPRATE },
    {"samprate", CSSIO_FIELD_SMPRATE }, /* alias */
    {   "calib", CSSIO_FIELD_CALIB   },
    {  "calper", CSSIO_FIELD_CALPER  },
    { "instype", CSSIO_FIELD_INSTYPE },
    { "segtype", CSSIO_FIELD_SEGTYPE },
    {"datatype", CSSIO_FIELD_DATATYPE},
    {    "clip", CSSIO_FIELD_CLIP    },
    {     "dir", CSSIO_FIELD_DIR     },
    {   "dfile", CSSIO_FIELD_DFILE   },
    {    "foff", CSSIO_FIELD_FOFF    },
    {  "commid", CSSIO_FIELD_COMMID  },
    {  "remark", CSSIO_FIELD_COMMID  }, /* alias */
    {  "lddate", CSSIO_FIELD_LDDATE  },
    {    "path", CSSIO_FIELD_PATH    },
    {      NULL, 0                   }
};

static CSSIO_TEXT_MAP DatatypeMap[] = {
    {  "s2", CSSIO_DATATYPE_S2  },
    {  "i2", CSSIO_DATATYPE_I2  },
    {  "s4", CSSIO_DATATYPE_S4  },
    {  "i4", CSSIO_DATATYPE_I4  },
    {  "s8", CSSIO_DATATYPE_S8  },
    {  "i8", CSSIO_DATATYPE_I8  },
    {  "t4", CSSIO_DATATYPE_T4  },
    {  "f4", CSSIO_DATATYPE_F4  },
    {  "u4", CSSIO_DATATYPE_F4  }, /* alias */
    {  "t8", CSSIO_DATATYPE_T8  },
    {  "f8", CSSIO_DATATYPE_F8  },
    {  "u8", CSSIO_DATATYPE_F8  }, /* alias */
    {"text", CSSIO_DATATYPE_TEXT}, /* alias */
    {  NULL, 0                  }
};

static char *LocateString(int code, CSSIO_TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

static int LocateCode(char *string, CSSIO_TEXT_MAP *map, int DefaultCode)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (strcasecmp(map[i].text, string) == 0) return map[i].code;
    return DefaultCode;
}

char *cssioFieldString(int code)
{
static char *DefaultMessage = NULL;

    return LocateString(code, FieldMap, DefaultMessage);
}

int cssioFieldCode(char *string)
{
static int DefaultCode = CSSIO_FIELD_UNKNOWN;

    return LocateCode(string, FieldMap, DefaultCode);
}

BOOL cssioFieldMatch(WFDISC_C *wd, int field, char *value)
{
char *actual;
BOOL retval;

    switch (field) {
      case CSSIO_FIELD_STA:      actual = wd->sta;      break;
      case CSSIO_FIELD_CHAN:     actual = wd->chan;     break;
      case CSSIO_FIELD_TIME:     actual = wd->time;     break;
      case CSSIO_FIELD_WFID:     actual = wd->wfid;     break;
      case CSSIO_FIELD_CHANID:   actual = wd->chanid;   break;
      case CSSIO_FIELD_JDATE:    actual = wd->jdate;    break;
      case CSSIO_FIELD_ENDTIME:  actual = wd->endtime;  break;
      case CSSIO_FIELD_NSAMP:    actual = wd->nsamp;    break;
      case CSSIO_FIELD_SMPRATE:  actual = wd->smprate;  break;
      case CSSIO_FIELD_CALIB:    actual = wd->calib;    break;
      case CSSIO_FIELD_CALPER:   actual = wd->calper;   break;
      case CSSIO_FIELD_INSTYPE:  actual = wd->instype;  break;
      case CSSIO_FIELD_SEGTYPE:  actual = wd->segtype;  break;
      case CSSIO_FIELD_DATATYPE: actual = wd->datatype; break;
      case CSSIO_FIELD_CLIP:     actual = wd->clip;     break;
      case CSSIO_FIELD_DIR:      actual = wd->dir;      break;
      case CSSIO_FIELD_DFILE:    actual = wd->dfile;    break;
      case CSSIO_FIELD_FOFF:     actual = wd->foff;     break;
      case CSSIO_FIELD_COMMID:   actual = wd->commid;   break;
      case CSSIO_FIELD_LDDATE:   actual = wd->lddate;   break;
      case CSSIO_FIELD_PATH:     actual = wd->path;     break;
      default:                   actual = NULL;
    }

    return strcmp(value, actual) == 0 ? TRUE : FALSE;
}

char *cssioDatatypeString(int code)
{
static char *DefaultMessage = NULL;

    return LocateString(code, DatatypeMap, DefaultMessage);
}

int cssioDatatypeCode(char *string)
{
static int DefaultCode = CSSIO_DATATYPE_UNKNOWN;

    return LocateCode(string, DatatypeMap, DefaultCode);
}

char *cssioDatatypeListString(char *delimiter, char *buf)
{
int i;
static CSSIO_TEXT_MAP *map;
static char *default_delimiter = " ";
static char mt_unsafe[MAXPATHLEN+1];

    if (delimiter == NULL) delimiter = default_delimiter;
    if (      buf == NULL) buf = mt_unsafe;

    map = DatatypeMap;
    buf[0] = 0;
    for (i = 0; map[i].text != NULL; i++) {
        if (i) strcat(buf, delimiter);
        strcat(buf, map[i].text);
    }

    return buf;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2014 Regents of the University of California            |
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
 * Revision 1.2  2014/04/18 18:27:46  dechavez
 * added cssioDatatypeString(), cssioDatatypeListString().  Changed code from UINT8 to int.
 *
 * Revision 1.1  2014/04/14 18:39:18  dechavez
 * created, introduces cssioFieldString(), cssioFieldCode(), cssioFieldMatch()
 *
 */
