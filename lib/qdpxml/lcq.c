#pragma ident "$Id: lcq.c,v 1.3 2012/06/24 17:58:21 dechavez Exp $"
/*======================================================================
 *
 * Parse Q330_configuration.tokens.lcq
 *
 *====================================================================*/
#include "protos.h"

static void SetPrecount(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->pebuf = (INT16) atoi(value);
    lcq->options |= QDP_LCQ_HAVE_PRE_EVENT_BUFFERS;
}

static void SetGap(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->gapthresh = atof(value);
    lcq->options |= QDP_LCQ_HAVE_GAP_THRESHOLD;
}

static void SetCaldly(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->caldly = (INT16) atoi(value);
    lcq->options |= QDP_LCQ_HAVE_CALIB_DELAY;
}

static void SetMaxframe(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->comfr = (INT16) atoi(value);
    lcq->options |= QDP_LCQ_HAVE_FRAME_COUNT;
}

static void SetFirfix(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->firfix = atof(value);
    lcq->options |= QDP_LCQ_HAVE_FIR_MULTIPLIER;
}

static void SetAveSamps(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->ave.len = (UINT32) atoi(value);
    lcq->options |= QDP_LCQ_HAVE_AVEPAR;
}

static void SetAveFilt(QDP_TOKEN_LCQ *lcq, char *value)
{
    strlcpy(lcq->ave.filt.name, value, QDP_PASCALSTRING_LEN+1);
    lcq->options |= QDP_LCQ_HAVE_AVEPAR;
}

static void SetCtrlDet(QDP_TOKEN_LCQ *lcq, char *value)
{
    lcq->options |= QDP_LCQ_HAVE_CNTRL_DETECTOR;
    qdpSetTokenIdent(&lcq->cntrl, QDP_UNDEFINED_ID, value);
}

static void SetDecimSource(QDP_TOKEN_LCQ *lcq, char *value)
{
    qdpSetTokenIdent(&lcq->decim.src, QDP_UNDEFINED_ID, value);
    lcq->options |= QDP_LCQ_HAVE_DECIM_ENTRY;
}

static void SetDecimFilt(QDP_TOKEN_LCQ *lcq, char *value)
{
    qdpSetTokenIdent(&lcq->decim.fir, QDP_UNDEFINED_ID, value);
    lcq->options |= QDP_LCQ_HAVE_DECIM_ENTRY;
}

static void set_value(MYXML_PARSER *xr, QDP_TOKEN_LCQ *lcq)
{
char *tag, *value;

    if ((tag = XmlCrntTag(xr)) == NULL) return;
    if ((value = XmlCrntValue(xr)) == NULL) return;

         if (strcmp(tag, "loc"      ) == 0) strlcpy(lcq->loc, value, QDP_LNAME_LEN+1);
    else if (strcmp(tag, "seed"     ) == 0) strlcpy(lcq->chn, value, QDP_CNAME_LEN+1);
    else if (strcmp(tag, "chan"     ) == 0) lcq->src[0] = (UINT8) strtol(value, NULL, 2);
    else if (strcmp(tag, "subchan"  ) == 0) lcq->src[1] = (UINT8) atoi(value);
    else if (strcmp(tag, "rate"     ) == 0) lcq->rate = (INT16) atoi(value);
    else if (strcmp(tag, "evonly"   ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_TRIGGERED_MODE        : 0);
    else if (strcmp(tag, "detpack"  ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_WRITE_DETECT_PKTS     : 0);
    else if (strcmp(tag, "calpack"  ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_WRITE_CALIB_PKTS      : 0);
    else if (strcmp(tag, "nooutput" ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_DO_NOT_OUTPUT         : 0);
    else if (strcmp(tag, "disable"  ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_DISABLE               : 0);
    else if (strcmp(tag, "dataserv" ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_SEND_TO_DATA_SERVER   : 0);
    else if (strcmp(tag, "netserv"  ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_SEND_TO_NET_SERVER    : 0);
    else if (strcmp(tag, "netevt"   ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_NETSERV_EVENT_ONLY    : 0);
    else if (strcmp(tag, "cnpforce" ) == 0) lcq->options |= (atoi(value) ? QDP_LCQ_FORCE_CNP_BLOCKETTTES : 0);
    else if (strcmp(tag, "precount" ) == 0) SetPrecount(lcq, value);
    else if (strcmp(tag, "gap"      ) == 0) SetGap(lcq, value);
    else if (strcmp(tag, "caldly"   ) == 0) SetCaldly(lcq, value);
    else if (strcmp(tag, "maxframe" ) == 0) SetMaxframe(lcq, value);
    else if (strcmp(tag, "firfix"   ) == 0) SetFirfix(lcq, value);
    else if (strcmp(tag, "avgsamps" ) == 0) SetAveSamps(lcq, value);
    else if (strcmp(tag, "avgfilt"  ) == 0) SetAveFilt(lcq, value);
    else if (strcmp(tag, "ctrldet"  ) == 0) SetCtrlDet(lcq, value);
    else if (strcmp(tag, "decsource") == 0) SetDecimSource(lcq, value);
    else if (strcmp(tag, "decfilt"  ) == 0) SetDecimFilt(lcq, value);
    else XmlWarn(xr);

    qdpConvertSampleRate(lcq->rate, &lcq->frate, &lcq->dsint, &lcq->nsint);
}

int qdpXmlParse_tokensLcq(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id)
{
int result;
char *mark, *tag;
QDP_TOKEN_LCQ new;

    qdpInitLcq(&new);
    new.ident.code = id; new.ident.name[0] = 0;

    mark = XmlCrntTag(xr);
    while (1) {
        switch (result = XmlNextRecord(xr, mark)) {
          case MYXML_RECORD_BEG:
            if ((tag = XmlCrntTag(xr)) == NULL) return MYXML_ERROR;
            if (strcmp(tag, "detect") == 0) qdpXmlParse_tokenLcqDetect(xr, &new);
            break;
          case MYXML_RECORD_END:
            break;
          case MYXML_VALUE:
            set_value(xr, &new);
            break;
          default:
            qdpSetLcqIdentName(&new);
            if (!listAppend(&token->lcq, &new, sizeof(QDP_TOKEN_LCQ))) return MYXML_ERROR;
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
 * $Log: lcq.c,v $
 * Revision 1.3  2012/06/24 17:58:21  dechavez
 * set derived fields frate, dsint, nsint
 *
 * Revision 1.2  2011/01/14 00:31:27  dechavez
 * qdpSetIdent() to qdpSetTokenIdent()
 *
 * Revision 1.1  2009/10/20 22:08:35  dechavez
 * first production release
 *
 */
