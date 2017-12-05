#pragma ident "$Id: xml.h,v 1.4 2016/01/21 17:15:15 dechavez Exp $"
/*======================================================================
 *
 *  Support for libqdpxml
 *
 *====================================================================*/
#ifndef qdpxml_h_included
#define qdpxml_h_included

#include "platform.h"
#include "qdp.h"
#include "myxml.h"

#ifdef __cplusplus
extern "C" {
#endif

/* function prototypes */

/* config.c */
BOOL qdpXmlConfigFromFile(char *fname, QDP_TYPE_FULL_CONFIG *config, FILE *warn);

/* print.c */
void qdpXmlPrintWriter(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_XML_WRITER *writer);
void qdpXmlPrint_C1_GLOB(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_GLOB *glob, char *basetag);
void qdpXmlPrint_C1_PHY(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_PHY *phy);
void qdpXmlPrint_C1_LOG(MYXML_HANDLE *xp, int indent, int subdent, int index, QDP_TYPE_C1_LOG *log);
void qdpXmlPrintSc(MYXML_HANDLE *xp, int indent, int subdent, UINT32 *sc);
void qdpXmlPrint_C1_SPP(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C1_SPP *spp);
void qdpXmlPrint_C2_PHY(MYXML_HANDLE *xp, int indent, int subdent, char *tag, QDP_TYPE_C2_PHY *phy);
void qdpXmlPrint_C2_AMASS(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_AMASS *amass);
void qdpXmlPrint_C2_GPS(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_GPS *gps);
void qdpXmlPrint_C3_ANNC(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C3_ANNC *annc);
void qdpXmlPrint_C3_BCFG(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C3_BCFG *bcfg);
void qdpXmlPrint_C2_EPCFG(MYXML_HANDLE *xp, int indent, int subdent, QDP_TYPE_C2_EPCFG *epcfg);
void qdpXmlPrintTokenVersion(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenLogid(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenCnfid(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenDss(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenClock(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenFirList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenIirList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenDetList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenCenList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenLcqList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenCdsList(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenSite(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokenServices(MYXML_HANDLE *xp, int indent, int subdent, QDP_DP_TOKEN *token);
void qdpXmlPrintTokens(MYXML_HANDLE *xp, int indent, int subdent, int index, QDP_DP_TOKEN *token);
void qdpXmlPrintWebpage(MYXML_HANDLE *xp, int indent, int subdent, char *tag, LNKLST *list);
void qdpXmlPrintFullConfig(MYXML_HANDLE *xp, QDP_TYPE_FULL_CONFIG *config, char *basetag);

/* version.c */
char *qdpXmlVersionString(void);
VERSION *qdpXmlVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* qdpxml_h_included */

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
 * $Log: xml.h,v $
 * Revision 1.4  2016/01/21 17:15:15  dechavez
 * updated prototypes
 *
 * Revision 1.3  2012/06/24 17:49:31  dechavez
 * updated qdpXmlPrint_C1_LOG() and qdpXmlPrintTokens() prototypes (pass index instead of tag)
 *
 * Revision 1.2  2009/10/20 21:53:54  dechavez
 * first production release
 *
 */
