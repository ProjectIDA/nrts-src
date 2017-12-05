#pragma ident "$Id: protos.h,v 1.2 2016/01/21 17:48:21 dechavez Exp $"
/*======================================================================
 *
 *  Prototypes for internal functions
 *
 *====================================================================*/
#ifndef qdpxml_protos_included
#define qdpxml_protos_included

#include "qdp/xml.h"

int qdpXmlParse_announce(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_automass(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_tokensCds(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id);
int qdpXmlParse_tokensClock(MYXML_PARSER *xr, QDP_DP_TOKEN *token);
int qdpXmlParse_data(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int x);
int qdpXmlParse_tokenLcqDetect(MYXML_PARSER *xr, QDP_TOKEN_LCQ *lcq);
int qdpXmlParse_tokensDss(MYXML_PARSER *xr, QDP_DP_TOKEN *token);
int qdpXmlParse_global(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_gps(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_tokensIir(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id);
int qdpXmlParse_interfaces(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_tokensLcq(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id);
int qdpXmlParse_tokensMurdock(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id);
int qdpXmlParse_phy2(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int index);
int qdpXmlParse_pll(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_sensctrl(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_slave(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_tokensThreshold(MYXML_PARSER *xr, QDP_DP_TOKEN *token, int id);
int qdpXmlParse_tokens(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config, int x);
int qdpXmlParse_webpage(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_writer(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_balecfg(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);
int qdpXmlParse_envproc(MYXML_PARSER *xr, QDP_TYPE_FULL_CONFIG *config);

#endif /* qdpxml_protos_included */

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
 * $Log: protos.h,v $
 * Revision 1.2  2016/01/21 17:48:21  dechavez
 * added qdpXmlParse_balecfg() and qdpXmlParse_envproc() prototypes
 *
 * Revision 1.1  2009/10/20 22:08:49  dechavez
 * first production release
 *
 */
