#pragma ident "$Id: Sta.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "Sta.h"


CSta::CSta()
	{
	Sta="";
	}

CSta::CSta(QString &StaName)
	{
	Sta=StaName;
	}
CSta::CSta(const char *pStaName)
	{
	Sta=QString(pStaName);
	}

void CSta::AddChan(QString &ChanName, double dSpS, bool bSelected)
	{
	for(unsigned int i=0; i<ChanInfo.size(); ++i)
		{
		if(ChanName.compare(ChanInfo[i]->Chan)==0)
			{
			return;
			}
		}
	CChanInfo *ci=new CChanInfo(ChanName,dSpS,bSelected);
	ChanInfo.push_back(ci);
	}


CSta::~CSta()
	{
	while(ChanInfo.size()>0)
		{
		CChanInfo *ci=ChanInfo[0];
		ChanInfo.erase(ChanInfo.begin());
		delete ci;
		}

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
 * $Log: Sta.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
