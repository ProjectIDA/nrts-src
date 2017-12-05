#pragma ident "$Id: CheckWfdisc.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"


#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "cssio.h"


bool CheckWfdiscRecord(QString &, const char *dir, const char *file);

void ScanWfdLine(struct wfdisc *pwfdisc, char *line)
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

bool CheckWfdisc(const char *pName)
	{
	QString fileName = QString(pName);


	if(!fileName.endsWith(".wfdisc", Qt::CaseInsensitive)) fileName = fileName + ".wfdisc";


	QFile qwfd(fileName);

	if(!qwfd.exists()) 
		{
		QString error = QString(pName);
		error += QString(": no such file\n");
		printf(error.toAscii().constData());
		return false;
		}
	
	QString qPath = QFileInfo(qwfd).absolutePath();

	char line[WFDISC_SIZE+2];
	wfdisc wfd;
	wfdisc *pwfdisc;
	pwfdisc=&wfd;
	FILE *fp=fopen((const char *)fileName.toLocal8Bit(),"rt");

	if(fp==NULL) return false;


	while(1)
		{
		if (fgets(line, WFDISC_SIZE+1, fp) == NULL) break;

		if (strlen(line) == WFDISC_SIZE) 
			{
			ScanWfdLine(pwfdisc, line);
            WFDISC_TRM(pwfdisc);
			if(!CheckWfdiscRecord(qPath, pwfdisc->dir, pwfdisc->dfile)) return false;

			}
		}
	fclose(fp);

	return true;

	}

bool CheckWfdiscRecord(QString &qWFD, const char *dir, const char *file)
	{
	QFileInfo fi;

	QDir Dir(qWFD);


	if(strcmp(dir, ".")!=0)
		if(!Dir.cd(dir))
			{
			QString error = dir;
			error += QString(": no such directory\n");
			printf(error.toAscii().constData());
			return false;
			}

	QString s = Dir.absolutePath()+QDir::separator()+QString(file);
	if( !QFile::exists(s))
		{
		QString error = s;
		error += QString(": no such file\n");
		printf(error.toAscii().constData());
		return false;
		}
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
 * $Log: CheckWfdisc.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
