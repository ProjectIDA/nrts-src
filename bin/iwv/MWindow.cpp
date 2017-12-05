#pragma ident "$Id: MWindow.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#include "MWindow.h"



CMWindow::CMWindow()
	{
	amin=-3000;
	amax=3000;
	dSpS=1.;
	}
CMWindow::CMWindow(CSta * pSt, CChanInfo * pCh)
	{
	pSta = pSt; pChan = pCh;

	amin=-3000;
	amax=3000;
	this->dSpS=pChan->dSpS;
	StaName=pSta->Sta;
	ChanName=pChan->Chan;
	}

CMWindow::~CMWindow()
	{
	while(wfdrec.size()>0)
		{
		CWfdiscRecord *dblk=wfdrec[0];
		wfdrec.erase(wfdrec.begin());
		delete dblk;
		}

	}
void CMWindow::AddWfdRecord(wfdisc wfd)
	{
	CWfdiscRecord *wf=new CWfdiscRecord(wfd);
	wfdrec.push_back(wf);
	}
void CMWindow::AutoScale(double t1, double t2)
	{
	bool bfirstcount=TRUE;
	int nDataBlocks=wfdrec.size();

	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=wfdrec[j]->wfd.time;
		int nsamp=wfdrec[j]->wfd.nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=wfdrec[j]->GetCount(k);// lData[k];
			double tt=t1packet+(1./dSpS)*k;

			if( (tt<t1) && (t1!=-1.) ) continue;
			if( (tt>t2) && (t2!=-1.) ) break;
			if(!bfirstcount)
				{
				if(a>amax) amax=a;
				if(a<amin) amin=a;
				}
			else
				{
				bfirstcount=FALSE;
				amax=a;
				amin=a;
				}

			}
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
 * $Log: MWindow.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
