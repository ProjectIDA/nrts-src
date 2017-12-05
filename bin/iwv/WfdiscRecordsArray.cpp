#pragma ident "$Id: WfdiscRecordsArray.cpp,v 1.2 2011/03/17 20:47:46 dechavez Exp $"
#include "WfdiscRecordsArray.h"

CWfdiscRecordsArray::CWfdiscRecordsArray()
	{

	}

CWfdiscRecordsArray::~CWfdiscRecordsArray()
	{
	Clear();
	}
void CWfdiscRecordsArray::Clear()
	{
	while(wfdrec.size()>0)
		{
		CWfdiscRecord *wfr=wfdrec[0];
		wfdrec.erase(wfdrec.begin());
		delete wfr;
		}

	}
void CWfdiscRecordsArray::Add(struct cssio_wfdisc wfd)
	{
	CWfdiscRecord *wfr=new CWfdiscRecord(wfd);
	wfdrec.push_back(wfr);
	}
unsigned int CWfdiscRecordsArray::GetSize()
	{
	return wfdrec.size();
	}
CWfdiscRecord  *CWfdiscRecordsArray::operator [] (int i)
	{
	return wfdrec[i];
	}

void CWfdiscRecordsArray::ScanWfdLine(struct cssio_wfdisc *pwfdisc, char *line)
	{
	sscanf(&line[0], "%s", pwfdisc->sta);
	sscanf(&line[7], "%s",pwfdisc->chan);
	sscanf(&line[16], "%lf",&pwfdisc->time);
	sscanf(&line[34], "%ld",&pwfdisc->wfid);
	sscanf(&line[43], "%ld",&pwfdisc->chanid);
	sscanf(&line[52], "%ld",&pwfdisc->jdate);
	sscanf(&line[61], "%lf",&pwfdisc->endtime);
	sscanf(&line[79], "%ld",&pwfdisc->nsamp);
	sscanf(&line[88], "%11f",&pwfdisc->smprate);
	sscanf(&line[100], "%f",&pwfdisc->calib);
	sscanf(&line[117], "%f",&pwfdisc->calper);
	sscanf(&line[134], "%s",pwfdisc->instype);
	sscanf(&line[141], "%c",&pwfdisc->segtype);
	sscanf(&line[143], "%s",pwfdisc->datatype);
	sscanf(&line[146], "%c",&pwfdisc->clip);
	sscanf(&line[148], "%s",pwfdisc->dir);
	sscanf(&line[213], "%s",pwfdisc->dfile);
	sscanf(&line[246], "%ld",&pwfdisc->foff);
	sscanf(&line[257], "%ld",&pwfdisc->commid); 
	sscanf(&line[266], "%s",pwfdisc->lddate);
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
 * $Log: WfdiscRecordsArray.cpp,v $
 * Revision 1.2  2011/03/17 20:47:46  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
