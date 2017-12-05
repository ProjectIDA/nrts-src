#pragma ident "$Id: StaInfo.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "StaInfo.h"


CStaInfo::CStaInfo()
	{

	}

CStaInfo::~CStaInfo()
	{
	Clear();
	}
void CStaInfo::Clear()
	{
	while(AllSta.size()>0)
		{
		CSta *si=AllSta[0];
		AllSta.erase(AllSta.begin());
		delete si;
		}

	}
void CStaInfo::AddInfo(wfdisc &wfd)
	{
	int i,j;
	CChanInfo *ci;
	CSta *si;

	for(i=0; i<AllSta.size();++i)
		{
		if(AllSta[i]->Sta.compare(wfd.sta)==0)
			{
			for(j=0; j<AllSta[i]->ChanInfo.size(); ++j)
				{
				if(AllSta[i]->ChanInfo[j]->Chan.compare(wfd.chan)==0)
					{
					return; // Sta and channel already exist in the list. No adding
					}
				}
			// Station exist, but channel info should be added
			ci=new	CChanInfo(wfd.chan, (double)wfd.smprate, true);
			AllSta[i]->ChanInfo.push_back(ci);
			return;
			}
		}
	ci=new	CChanInfo(wfd.chan, wfd.smprate, true);
	si=new CSta(wfd.sta);
	si->ChanInfo.push_back(ci);
	AllSta.push_back(si);
//	*(AllSta.end()).ChanInfo.push_back(ci);
	return;
	}
CStaInfo &CStaInfo::operator =(CStaInfo &sinf)
	{
	CChanInfo *ci;
	CSta *si;
	int i,j;

	Clear();

	for(i=0; i<sinf.AllSta.size(); ++i)
		{
		si=new CSta(sinf.AllSta[i]->Sta);
		AllSta.push_back(si);
		for(j=0; j<sinf.AllSta[i]->ChanInfo.size(); ++j)
			{
			ci=new	CChanInfo(sinf.AllSta[i]->ChanInfo[j]->Chan, sinf.AllSta[i]->ChanInfo[j]->dSpS, sinf.AllSta[i]->ChanInfo[j]->bSelected);
			si->ChanInfo.push_back(ci);
			}
		}
	return *this;
	}
/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: StaInfo.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

