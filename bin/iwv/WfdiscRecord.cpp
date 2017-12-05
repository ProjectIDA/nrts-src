#pragma ident "$Id: WfdiscRecord.cpp,v 1.2 2011/03/17 20:47:46 dechavez Exp $"
#include <QApplication>
#include <QSysInfo>
#include <QDir>
#include "WfdiscRecord.h"
#include "util.h"
#include "cssio.h"

#ifdef Q_OS_UNIX
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#endif

#ifdef Q_OS_WIN32 
#include <stdio.h>
#include <io.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif



void CopyStringWithBlankRemove(char *lpTar,  char *lpSou);
int cssio_wrdsize(char *datatype);


CWfdiscRecord::CWfdiscRecord()
	{
	wfd=wfdisc_null;
	nCacheFirstCount=0;
	nCacheMaximumSize=100000;//counts
	nCacheSize=0;
	lData=NULL;
	}

CWfdiscRecord::CWfdiscRecord(struct cssio_wfdisc wfd)
	{
	this->wfd=wfd;
	nCacheFirstCount=0;
	nCacheMaximumSize=100000;//counts
	nCacheSize=0;
	lData=NULL;
	}

CWfdiscRecord::~CWfdiscRecord()
	{
	if(lData!=NULL) delete[]lData;
	}
long CWfdiscRecord::LoadData(int nDesCount, int nMaxCounts)
	{
	bool bLittleEndian=false;
	int nType;
	unsigned long i;
	unsigned long nWordSize=cssio_wrdsize(wfd.datatype);
	unsigned long nSamples=wfd.nsamp;
	void *vData;
	long *llData; short *shData;
	int FirstCount, nTotalCounts;

	if(QSysInfo::ByteOrder==QSysInfo::LittleEndian) bLittleEndian=true;
	if(nDesCount>=nSamples) return 0;

	FirstCount=nDesCount;

	if(nDesCount+nCacheMaximumSize>nSamples)
		{
		nTotalCounts=nSamples-nDesCount;
		}
	else
		{
		nTotalCounts=nCacheMaximumSize;
		}
	if(nMaxCounts>0)
		if(nTotalCounts>nMaxCounts) 
			{
			nTotalCounts=nMaxCounts;
			}
	if(lData!=NULL) delete []lData;
	lData=new long[nTotalCounts];

	nCacheSize=nTotalCounts;
	nCacheFirstCount=FirstCount;


	vData=new char[nWordSize*nCacheSize];
	lseek(ncssFile, wfd.foff+nCacheFirstCount*nWordSize,SEEK_SET); 
	int nRead = read(ncssFile, vData,nWordSize*nCacheSize);
	nCacheSize = nRead/nWordSize;

	if(strcmp(wfd.datatype,"i2")==0) nType=0;
	else if(strcmp(wfd.datatype,"i4")==0) nType=1;
	else if(strcmp(wfd.datatype,"s2")==0) nType=2;
	else if(strcmp(wfd.datatype,"s4")==0) nType=3;
	else nType=-1;

	if(nType==-1)
		{
		if(lData!=NULL) delete[] lData;
		delete[] vData;
		return -1;
		}
	
	switch(nType)
		{
		case 0:
			shData=(short *)vData;
			if(!bLittleEndian)util_sswap(shData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=shData[i];
			break;
		case 1:
			llData=(long *)vData;
			if(!bLittleEndian) util_lswap(llData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=llData[i];
			break;
		case 2:
			shData=(short *)vData;
			if(bLittleEndian)util_sswap(shData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=shData[i];
			break;
		case 3:
			llData=(long *)vData;
			if(bLittleEndian) util_lswap(llData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=llData[i];
			break;
		}
	delete[] vData;

	return nCacheSize;
	}
long CWfdiscRecord::GetCount(int n)
	{
	if( (n>=nCacheFirstCount) && (n<nCacheFirstCount+nCacheSize) )
		{
		long count=lData[n-nCacheFirstCount];
		return count;
		}
	else
		{
		LoadData(n);
		return GetCount(n);
		}
	}
bool CWfdiscRecord::OpenDataSet()
	{
	char cFullPath[MAX_PATH],cTmp[MAX_PATH];

	CopyStringWithBlankRemove(cTmp,wfd.dir);


	if(strcmp(cTmp,".")==0)
		{
		*cFullPath=0;
		}
	else
		{
		strcpy(cFullPath,cTmp);
		int nLen = strlen(cTmp);
		cFullPath[nLen]   = QDir::separator().toAscii();
		cFullPath[nLen+1] = 0;
		}
	strcat(cFullPath,wfd.dfile);

	if( (ncssFile = open(cFullPath, O_BINARY| O_RDONLY))<0)
		{
		return false;
		}
	return true;
	}
bool CWfdiscRecord::CloseDataSet()
	{
	delete []lData;
	lData=NULL;
	close(ncssFile);
	return true;
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
 * $Log: WfdiscRecord.cpp,v $
 * Revision 1.2  2011/03/17 20:47:46  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
