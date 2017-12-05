#pragma ident "$Id: Screen.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#include "Screen.h"


CScreen::CScreen()
	{
	nMaxSel=0;
	}

CScreen::~CScreen()
	{
	Clear();
	}

void CScreen::Clear()
	{
	nMaxSel=0;
	while(Screen.size()>0)
		{
		CScreenElem  *pElm=Screen[0];
		Screen.erase(Screen.begin());
		delete pElm;
		}
	}

void CScreen::AddElement(int nRec)
	{
	CScreenElem  *pElm=new CScreenElem(nRec);
	Screen.push_back(pElm);
	}

int CScreen::GetMaxWindow()
	{
	return Screen.size();
	}

int CScreen::GetWaveformN(int n)
	{
	CScreenElem  *pElm=Screen[n];
	return pElm->nWfdRec;
	}

bool CScreen::IsSelected(int n)
	{
	CScreenElem  *pElm=Screen[n];
	if(pElm->state==0)
		return false;
	return true;
	}

int CScreen::SelectUnselectElement(int nWindow)
	{
	CScreenElem  *pElm=Screen[nWindow];
	if(pElm->state==0)
		{
		++pElm->state;
		pElm->nSel=nMaxSel++;
		return 1;
		}
	else
		{
		int nMax=GetMaxWindow();
		pElm->state=0;
		
		for(int i=0; i<nMax; ++i)
			{
			CScreenElem  *pElma=Screen[i];
			if(pElma->state>0)
				{
				if(pElma->nSel>pElm->nSel)
					{
					--pElma->nSel;
					}
				}
			}
		--nMaxSel;
		pElm->nSel=0;
		return 0;
		}
	}

int  Compare(const void *arg1, const void *arg2 )
	{
	SREANELEM *se1=(SREANELEM *)arg1;
	SREANELEM *se2=(SREANELEM *)arg2;


	if(se1->nPos<se2->nPos)
		{
		return -1;
		}
	else
		{
		return 1;
		}
	}

void CScreen::SelectTraces()
	{
	int i=0;
	bool bSelected = false;

	for(i=0; i<Screen.size(); ++i) 
		{
		if(Screen[i]->state) 
			{
			bSelected = true;
			break;
			}
		}

	if(!bSelected) return;

	i=0;

	while(i<GetMaxWindow())
		{
		CScreenElem  *pElma=Screen[i];
		if(pElma->state==0)
			{
			Screen.erase(Screen.begin()+i);
			delete pElma;
			continue;
			}
		else
			{
			pElma->state=0;
			++i;
			}
		}
	int nMax=GetMaxWindow();

	if(nMax==0) return;

	struct SREANELEM  *scrMas=new SREANELEM[nMax];

	for(i=0; i<nMax; ++i)
		{
		CScreenElem  *pElm=Screen[i];
		scrMas[i].nPos=pElm->nSel;
		scrMas[i].nWfdRec=pElm->nWfdRec;
		}
	qsort(scrMas,nMax,sizeof(SREANELEM),Compare);
	Clear();

	for(i=0; i<nMax; ++i)
		{
		AddElement(scrMas[i].nWfdRec);
		}
	nMaxSel=0;
	delete[]scrMas;
	}


void CScreen::DeleteTraces()
	{
	int i=0;

	while(i<GetMaxWindow())
		{
		CScreenElem  *pElma=Screen[i];
		if(pElma->state!=0)
			{
			Screen.erase(Screen.begin()+i);
			delete pElma;
			continue;
			}
		else
			{
			pElma->state=0;
			++i;
			}
		}
	int nMax=GetMaxWindow();
	struct SREANELEM  *scrMas=new SREANELEM[nMax];

	for(i=0; i<nMax; ++i)
		{
		CScreenElem  *pElm=Screen[i];
		scrMas[i].nPos=pElm->nSel;
		scrMas[i].nWfdRec=pElm->nWfdRec;
		}
	qsort(scrMas,nMax,sizeof(SREANELEM),Compare);
	Clear();

	for(i=0; i<nMax; ++i)
		{
		AddElement(scrMas[i].nWfdRec);
		}
	nMaxSel=0;
	delete[]scrMas;
	}


void CScreen::SelectAll()
	{
	int nMaxWin=GetMaxWindow();
	for(int i=0; i<nMaxWin; ++i)
		{
		CScreenElem  *pElma=Screen[i];
		pElma->state=1;
		pElma->nSel=i;
		}
	nMaxSel=nMaxWin;
	}

void CScreen::UnSelectAll()
	{
	int nMaxWin=GetMaxWindow();
	for(int i=0; i<nMaxWin; ++i)
		{
		CScreenElem  *pElma=Screen[i];
		pElma->state=0;
		}
	nMaxSel=0;
	}

int CScreen::GetSelectionNumber(int n)
	{
	CScreenElem  *pElm=Screen[n];
	return pElm->nSel;
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
 * $Log: Screen.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */