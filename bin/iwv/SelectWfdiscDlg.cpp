#pragma ident "$Id: SelectWfdiscDlg.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "SelectWfdiscDlg.h"
#include <QApplication>
#include "util.h"

QString SelectWfdiscDlg::LastDir = "";

SelectWfdiscDlg::SelectWfdiscDlg(QWidget * parent, Qt::WindowFlags flags):QFileDialog(parent, flags)
	{
	QStringList fileNames;
	setReadOnly(true);
	setFilter(tr("wfdisc (*.wfdisc *.wfd)"));
	setFileMode(QFileDialog::ExistingFile);
	QGridLayout *lout = (QGridLayout *)layout();

	qtw = new QTableWidget;
	qtw->setMinimumHeight(100);

    qtw->setRowCount( 0 );
    qtw->setColumnCount( 5 ); 


	QStringList  labels;
	labels <<"Station"<<"Channel"<<"SpS"<<"T beg"<< "T end";
	qtw->setHorizontalHeaderLabels(labels);

	qtw->setSelectionBehavior(QAbstractItemView::SelectRows);
	qtw->setSelectionMode(QAbstractItemView::SingleSelection);
	qtw->verticalHeader()->hide();


	lout->addWidget(qtw, 5, 0, 1, -1);

	SetPreviewColumnSize();



	connect(this, SIGNAL(currentChanged( const QString & )), this, SLOT(wfd_selected(const QString & )));

	if(!LastDir.isEmpty()) setDirectory(LastDir);

	}

SelectWfdiscDlg::~SelectWfdiscDlg(void)
	{
	DeleteAllRawsInPreview();
	}

void SelectWfdiscDlg::LoadPreview(const char *lpFileName)
	{
	char cChan[32],cSpS[32],cSta[32],cTimeBeg[32],cTimeEnd[32];
	long nRecord = 0;
	static wfdisc wfd;
	struct wfdisc *pwfdisc;
	pwfdisc=&wfd;
	char line[WFDISC_SIZE+2];
	FILE *fp=fopen(lpFileName,"rt");


	DeleteAllRawsInPreview();

	if(fp==NULL)
		{
		return;
		}

	while(1)
		{
		if (fgets(line, WFDISC_SIZE+1, fp) == NULL) break;
		if (strlen(line) == WFDISC_SIZE) 
			{
//        sscanf(line, WFDISC_SCS, WFDISC_RVL(pwfdisc));

			CWfdiscRecordsArray::ScanWfdLine(pwfdisc, line);
            WFDISC_TRM(pwfdisc);

			strcpy(cSta,	pwfdisc->sta);
			strcpy(cChan,	pwfdisc->chan);
			sprintf(cSpS,"%.3f", pwfdisc->smprate);
			sprintf(cTimeBeg,"%s",util_dttostr(pwfdisc->time, 0));
			sprintf(cTimeEnd,"%s",util_dttostr(pwfdisc->endtime, 0));
			qtw->insertRow(nRecord);
			qtw->setItem(nRecord,   0, new QTableWidgetItem(cSta));
			qtw->setItem(nRecord,   1, new QTableWidgetItem(cChan));
			qtw->setItem(nRecord,   2, new QTableWidgetItem(cSpS));
			qtw->setItem(nRecord,   3, new QTableWidgetItem(cTimeBeg));
			qtw->setItem(nRecord,   4, new QTableWidgetItem(cTimeEnd));
			qtw->setRowHeight(nRecord, PreviewRowHeight);
			qtw->item(nRecord,   0)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			qtw->item(nRecord,   1)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			qtw->item(nRecord,   2)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			qtw->item(nRecord,   3)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			qtw->item(nRecord,   4)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);

			++nRecord;
			}
		}
	fclose(fp);
	}
void SelectWfdiscDlg::wfd_selected(const QString & path)
	{
	LoadPreview((const char *)path.toAscii());
	}
void SelectWfdiscDlg::DeleteAllRawsInPreview()
	{
	while(qtw->rowCount()>0) qtw->removeRow(0);
	}
void SelectWfdiscDlg::GetPreviewTextRectangle(const char *p, QRect &qrc)
	{

	QFontMetrics fm = qtw->fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}
void SelectWfdiscDlg::SetPreviewColumnSize()
	{
	QRect qrc;
	GetPreviewTextRectangle("XXXX:XXX:XX XX:XX:XX.XXX", qrc);

	qtw->setColumnWidth ( 3, qrc.width()+2);
	qtw->setColumnWidth ( 4, qrc.width()+2);
	PreviewRowHeight = qrc.height()+4;

	GetPreviewTextRectangle(" Station ", qrc);
	qtw->setColumnWidth ( 0, qrc.width()+2);
	GetPreviewTextRectangle(" Channel ", qrc);
	qtw->setColumnWidth ( 1, qrc.width()+2);
	GetPreviewTextRectangle("XXXX.XXX", qrc);
	qtw->setColumnWidth ( 2, qrc.width()+2);

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
 * $Log: SelectWfdiscDlg.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

