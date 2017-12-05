#pragma ident "$Id: tokens.c,v 1.6 2014/08/11 18:25:15 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens
 *
 *====================================================================*/
#include "protos.h"

static void SetVersion(QDP_DP_TOKEN *token, char *value)
{
    token->ver.version = (UINT16) atoi(value);
    token->ver.valid = TRUE;
}

static void SetSiteNname(QDP_TOKEN_SITE *site, char *value)
{
    strlcpy(site->nname, value, QDP_NNAME_LEN+1);
    site->valid = TRUE;
}

static void SetSiteSname(QDP_TOKEN_SITE *site, char *value)
{
    strlcpy(site->sname, value, QDP_SNAME_LEN+1);
    site->valid = TRUE;
}

static void SetServerPort(QDP_TOKEN_SERVER *server, char *value)
{
    server->port = (INT16) atoi(value);
    server->valid = TRUE;
}

static void SetLogIdMesgLoc(QDP_TOKEN_LOGID *logid, char *value)
{
    strlcpy(logid->mesg.loc, value, QDP_LNAME_LEN+1);
    logid->valid = TRUE;
}

static void SetLogIdMesgChn(QDP_TOKEN_LOGID *logid, char *value)
{
    strlcpy(logid->mesg.chn, value, QDP_CNAME_LEN+1);
    logid->valid = TRUE;
}

static void SetLogIdTimeLoc(QDP_TOKEN_LOGID *logid, char *value)
{
    strlcpy(logid->time.loc, value, QDP_LNAME_LEN+1);
    logid->valid = TRUE;
}

static void SetLogIdTimeChn(QDP_TOKEN_LOGID *logid, char *value)
{
    strlcpy(logid->time.chn, value, QDP_CNAME_LEN+1);
    logid->valid = TRUE;
}

static void SetCnfIdLoc(QDP_TOKEN_CNFID *cnfid, char *value)
{
    strlcpy(cnfid->loc, value, QDP_LNAME_LEN+1);
    cnfid->valid = TRUE;
}

static void SetCnfIdChn(QDP_TOKEN_CNFID *cnfid, char *value)
{
    strlcpy(cnfid->chn, value, QDP_CNAME_LEN+1);
    cnfid->valid = TRUE;
}

static void SetCnfIdInterval(QDP_TOKEN_CNFID *cnfid, char *value)
{
    cnfid->interval = (UINT16) atoi(value);
    cnfid->valid = TRUE;
}

static void SetCnfIdFlags(QDP_TOKEN_CNFID *cnfid, UINT8 mask)
{
    cnfid->flags |= mask;
    cnfid->valid = TRUE;
}

static void AddCen(QDP_DP_TOKEN *token, int id, char *value)
{
QDP_TOKEN_CEN new;

    new.id = id;
    strlcpy(new.name, value, QDP_PASCALSTRING_LEN+1);
    listAppend(&token->cen, &new, sizeof(QDP_TOKEN_CEN));
}

static void AddFir(QDP_DP_TOKEN *token, char *value, int *nfir)
{
QDP_TOKEN_FIR new;

    *nfir += 1;
    new.id = *nfir;
    strlcpy(new.name, value, QDP_PASCALSTRING_LEN+1);
    listAppend(&token->fir, &new, sizeof(QDP_TOKEN_FIR));
}

static void set_value(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int *nfir)
{
int i;
char label[] = "commname_XX + slop";
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

    for (i = 1; i <= 32; i++) {
        sprintf(label, "commname_%d", i);
        if (strcmp(tag, label) == 0) {
            AddCen(token, i-1, value);
            return;
        }
    }

         if (strcmp(tag, "tokever"    ) == 0) SetVersion(token, value);
    else if (strcmp(tag, "tokenset"   ) == 0) strlcpy(token->setname, value, QDP_MAX_TOKENSET_NAME+1);
    else if (strcmp(tag, "msgloc"     ) == 0) SetLogIdMesgLoc(&token->logid, value);
    else if (strcmp(tag, "msgname"    ) == 0) SetLogIdMesgChn(&token->logid, value);
    else if (strcmp(tag, "timloc"     ) == 0) SetLogIdTimeLoc(&token->logid, value);
    else if (strcmp(tag, "timname"    ) == 0) SetLogIdTimeChn(&token->logid, value);
    else if (strcmp(tag, "cfgloc"     ) == 0) SetCnfIdLoc(&token->cnfid, value);
    else if (strcmp(tag, "cfgname"    ) == 0) SetCnfIdChn(&token->cnfid, value);
    else if (strcmp(tag, "cfginterval") == 0) SetCnfIdInterval(&token->cnfid, value);
    else if (strcmp(tag, "cfgflgbeg"  ) == 0) SetCnfIdFlags(&token->cnfid, ((UINT8) atoi(value) << 0));
    else if (strcmp(tag, "cfgflgend"  ) == 0) SetCnfIdFlags(&token->cnfid, ((UINT8) atoi(value) << 1));
    else if (strcmp(tag, "cfgflgint"  ) == 0) SetCnfIdFlags(&token->cnfid, ((UINT8) atoi(value) << 2));
    else if (strcmp(tag, "noncomp"    ) == 0) token->noncomp = atoi(value);
    else if (strcmp(tag, "network"    ) == 0) SetSiteNname(&token->site, value);
    else if (strcmp(tag, "station"    ) == 0) SetSiteSname(&token->site, value);
    else if (strcmp(tag, "netport"    ) == 0) SetServerPort(&token->srvr.net, value);
    else if (strcmp(tag, "webport"    ) == 0) SetServerPort(&token->srvr.web, value);
    else if (strcmp(tag, "dataport"   ) == 0) SetServerPort(&token->srvr.data, value);
    else if (strcmp(tag, "firname"    ) == 0) AddFir(token, value, nfir);
    else if (strcmp(tag, "crc"        ) == 0) token->crc = (UINT32) atoi(value);
    else XmlWarn(xr);
}

static int ParseTokens(MYXML_PARSER *xr, QDP_DP_TOKEN *token)
{
int result, nfir=0, niir=0, nlcq=0, ncds=0;
int ndet=1; /* since 0 is assumed to be DEC10 */
char *mark, *tag;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) return result;
                 if (strcmp(tag, "dss"       ) == 0) qdpXmlParse_tokensDss(xr, token);
            else if (strcmp(tag, "clock"     ) == 0) qdpXmlParse_tokensClock(xr, token);
            else if (strcmp(tag, "iir"       ) == 0) qdpXmlParse_tokensIir(xr, token, ++niir);
            else if (strcmp(tag, "threshold" ) == 0) qdpXmlParse_tokensThreshold(xr, token, ++ndet);
            else if (strcmp(tag, "murdock"   ) == 0) qdpXmlParse_tokensMurdock(xr, token, ++ndet);
            else if (strcmp(tag, "lcq"       ) == 0) qdpXmlParse_tokensLcq(xr, token, ++nlcq);
            else if (strcmp(tag, "controldet") == 0) qdpXmlParse_tokensCds(xr, token, ++ncds);
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, token, &nfir);
            break;
          default:
            if (!qdpFinalizeToken(token)) return MYXML_ERROR;
            return result;
        }
    }
}

int qdpXmlParse_tokens(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int id)
{
int index;

    switch (id) {
      case QDP_MEM_TYPE_CONFIG_DP1: index = 0; break;
      case QDP_MEM_TYPE_CONFIG_DP2: index = 1; break;
      case QDP_MEM_TYPE_CONFIG_DP3: index = 2; break;
      case QDP_MEM_TYPE_CONFIG_DP4: index = 3; break;
      default: 
        return MYXML_ERROR;
    }
    config->dport[index].set |= QDP_DPORT_DEFINED_TOKEN;
    return ParseTokens(xr, &config->dport[index].token);
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
 * $Log: tokens.c,v $
 * Revision 1.6  2014/08/11 18:25:15  dechavez
 * added missing return value in error return branch of ParseTokens()
 *
 * Revision 1.5  2012/06/24 17:56:35  dechavez
 * use config->dport[i].token instead of config->token[i]
 *
 * Revision 1.4  2010/12/22 23:48:33  dechavez
 * 1.1.0
 *
 * Revision 1.3  2009/11/05 18:41:24  dechavez
 * set valid flag where needed
 *
 * Revision 1.2  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
