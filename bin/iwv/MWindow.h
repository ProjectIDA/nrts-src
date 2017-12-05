#pragma ident "$Id: MWindow.h,v 1.1 2010/05/11 18:24:59 akimov Exp $"

#if !defined(_MWINDOW_H_INCLUDED_)
#define _MWINDOW_H_INCLUDED_


#include "WfdiscRecord.h"
#include "ChanInfo.h"
#include "Sta.h"
#include <vector>
#include <QString>



class CMWindow  
{
public:
	CMWindow();
	CMWindow(CSta *, CChanInfo *);
	void AddWfdRecord(wfdisc wfd);
	virtual ~CMWindow();
	void AutoScale(double t1=-1, double t2=-1);

public:
	int x1,x2,y1,y2;
	int xw1,xw2,yw1,yw2;
	int xp1,xp2,yp1,yp2;
	QString StaName;
	QString ChanName;
	double dSpS;
	long amin,amax;
	long amin_1, amax_1;
	std::vector<CWfdiscRecord *> wfdrec;
	CSta *pSta;
	CChanInfo *pChan;

};

#endif // !defined(_MWINDOW_H_INCLUDED_)
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
 * $Log: MWindow.h,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
