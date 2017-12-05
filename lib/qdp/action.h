#pragma ident "$Id: action.h,v 1.17 2016/08/04 21:25:56 dechavez Exp $"
/*======================================================================
 * 
 * Prototypes for the event handlers
 *
 *====================================================================*/
#ifndef qdp_fsa_action_included
#define qdp_fsa_action_included

#ifdef __cplusplus
extern "C" {
#endif

#define SEQNO_MASK (QDP_MAX_WINDOW_NBUF - 1)

/* action.c */
void ActionSTART(QDP *qp, QDP_EVENT *event);
void ActionSEND(QDP *qp, QDP_EVENT *event);
void ActionPOLLSN(QDP *qp, QDP_EVENT *event);
void ActionLDSN(QDP *qp, QDP_EVENT *event);
void ActionSRVRQ(QDP *qp, QDP_EVENT *event);
void ActionDECODECH(QDP *qp, QDP_EVENT *event);
void ActionBLDRSP(QDP *qp, QDP_EVENT *event);
void ActionREGISTERED(QDP *qp, QDP_EVENT *event);
void ActionRQ_C1_FIX(QDP *qp, QDP_EVENT *event);
void ActionLD_C1_FIX(QDP *qp, QDP_EVENT *event);
void ActionRQ_C1_LOG(QDP *qp, QDP_EVENT *event);
void ActionLD_C1_LOG(QDP *qp, QDP_EVENT *event);
void ActionRQ_C2_EPD(QDP *qp, QDP_EVENT *event);
void ActionLD_C2_EPD(QDP *qp, QDP_EVENT *event);
void ActionRQ_C1_FLGS(QDP *qp, QDP_EVENT *event);
void ActionLD_C1_FLGS(QDP *qp, QDP_EVENT *event);
void ActionRQ_TOKENS(QDP *qp, QDP_EVENT *event);
void ActionLD_TOKENS(QDP *qp, QDP_EVENT *event);
void ActionTLU(QDP *qp, QDP_EVENT *event);
void ActionTLD(QDP *qp, QDP_EVENT *event);
void ActionREGERR(QDP *qp, QDP_EVENT *event);
void ActionCERR(QDP *qp, QDP_EVENT *event);
void ActionCACK(QDP *qp, QDP_EVENT *event);
void ActionCTRL(QDP *qp, QDP_EVENT *event);
void ActionOPEN(QDP *qp, QDP_EVENT *event);
void ActionDTO(QDP *qp, QDP_EVENT *event);
void ActionHBEAT(QDP *qp, QDP_EVENT *event);
void ActionAPPCMD(QDP *qp, QDP_EVENT *event);
void ActionDACK(QDP *qp, QDP_EVENT *event);
void ActionFILL(QDP *qp, QDP_EVENT *event);
void ActionDROP(QDP *qp, QDP_EVENT *event);
void ActionFLUSH(QDP *qp, QDP_EVENT *event);
void ActionCRCERR(QDP *qp, QDP_EVENT *event);
void ActionIOERR(QDP *qp, QDP_EVENT *event);
void ActionWATCHDOG(QDP *qp, QDP_EVENT *event);

/* reorder.c */
void ActionINITDACK(QDP *qp);
void ActionDATA(QDP *qp, QDP_EVENT *event);

#ifdef __cplusplus
}
#endif

#endif /* qdp_fsa_action_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: action.h,v $
 * Revision 1.17  2016/08/04 21:25:56  dechavez
 * updated prototypes to correspond to changes in action.c Revision 1.32
 *
 * Revision 1.16  2016/07/20 16:36:16  dechavez
 * updated prototypes following rework of state machine
 *
 * Revision 1.15  2016/06/16 15:34:37  dechavez
 * added ActionGIVEUP() prototype
 *
 * Revision 1.14  2016/02/11 18:52:18  dechavez
 * updated prototypes
 *
 * Revision 1.13  2016/02/03 17:38:28  dechavez
 * updated prototypes
 *
 * Revision 1.12  2016/01/27 00:17:39  dechavez
 * added ActionRQEPD() and ActionLDEPD() prototypes
 *
 * Revision 1.11  2014/08/11 18:07:39  dechavez
 * MAJOR CHANGES TO SUPPORT Q330 DATA COMM OVER SERIAL PORT (see 8/11/2014 comments in version.c)
 *
 * Revision 1.10  2011/01/31 21:14:42  dechavez
 * added new prototypes
 *
 * Revision 1.9  2011/01/13 20:31:11  dechavez
 * added new prototypes
 *
 * Revision 1.8  2010/03/22 21:33:23  dechavez
 * added ActionBUSY
 *
 * Revision 1.7  2006/05/18 06:44:53  dechavez
 * initial release
 *
 */
