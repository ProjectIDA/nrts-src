#pragma ident "$Id: WaveformsPrint.cpp,v 1.2 2010/06/07 18:32:51 akimov Exp $"
#include <QPrintDialog>
#include <QPrinter>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include "WaveformsPrint.h"
#include "util.h"
#include "filter.h"
#include "SelectPDFfileDlg.h"
#include <algorithm>





WaveformsPrint::WaveformsPrint(QWaveformsDisplay *pWaveformsDisplay)
	{
	pWD=pWaveformsDisplay;
	QRegion qReg = pWD->visibleRegion();
	QRect rc = qReg.boundingRect();
	}

WaveformsPrint::~WaveformsPrint(void)
	{
	}

void WaveformsPrint::Print()
	{
	SelectPDFfileDlg Dlg(pWD, 0);

	if(Dlg.exec()!=QDialog::Accepted) return;


	QStringList fileNames;
    QString fileName; 

    fileNames = Dlg.selectedFiles();
	if(fileNames.size()>0)
		{
		QDir dir = Dlg.directory();
		QString s = dir.absolutePath();
		QString sHeader;

		Dlg.LastDir = s;

		fileName = fileNames[0];

		if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
			fileName.append(".pdf");}
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
		printer.setPageSize(QPrinter::A4);
		printer.setOrientation(QPrinter::Landscape);

		sHeader = Dlg.GetHeaderText();
		PrintWaveforms(printer, sHeader);
		}


	QMessageBox::information ( pWD, "Saving to PDF", "Done", QMessageBox::Ok, QMessageBox::Ok ); 
	}
	
void WaveformsPrint::PrintWaveforms(QPrinter &printer, QString &sHead)
	{
	QPen Pen(QColor(0,0,0));
	QFont fStaNameFont("Helvetica"), fAmplFont("Helvetica"), fHeadFont("Helvetica");
	QPainter painter;
	int nTrace;


	QRect rcText;
	char cText[128];

	QFont defFont = QApplication::font();
	QString defFontFamily = defFont.family();
	fStaNameFont.setFamily(defFontFamily);
	fAmplFont.setFamily(defFontFamily);
	fHeadFont.setFamily(defFontFamily);

	Pen.setWidth (0);
	painter.begin(&printer);

	QRect rc(0,0,MaxX-1,MaxY-1);
	int i;

	rc = painter.window();
	MaxX = rc.width()-1;
	MaxY = rc.height()-1;
	int nMMX = printer.heightMM();
	int nMMY = printer.widthMM();
	p_mm_x=((float)MaxX/nMMX);	
	p_mm_y=((float)MaxY/nMMY);


	painter.setPen(Pen);


	int nWin=pWD->pScreen->GetMaxWindow();
	TimeBarYsize=(int)(30*p_mm_y);
	WindowInfoXsize=(int)(30*p_mm_x);
	topMargin=(int)(p_mm_y*20);

	int x1,x2,y1,y2,xw1,xw2,yw1,yw2;

	
	fHeadFont.setPixelSize(8*p_mm_y);

	painter.setFont(fHeadFont);
	GetTextRectangle(painter, sHead, rcText);

	nHeadHeight=rcText.bottom();

	rcText.setRight(MaxX-1);
	rcText.setBottom(rcText.bottom());
	rcText.setTop(rcText.top());
	painter.drawText(rcText,Qt::TextSingleLine|Qt::AlignVCenter|Qt::AlignHCenter, sHead);



	QPen BoldPen(QColor(0,0,0),(int)p_mm_x*.25, Qt::SolidLine);
	fStaNameFont.setPixelSize(5*p_mm_y);
	fAmplFont.setPixelSize(4*p_mm_y);
	int nFirst, nLast;
	bool bdraw = pWD->GetVisibleTraces(nFirst, nLast);
	nWin = nLast - nFirst+1;
	int yHeight=(MaxY-TimeBarYsize-nHeadHeight-topMargin)/nWin;



	if(bdraw)
	for(int j=nFirst, nTrace=0; j<=nLast; ++j, ++nTrace)
		{
		i=pWD->pScreen->GetWaveformN(j);

		long amin=pWD->WaveformInfo[i]->amin;
		long amax=pWD->WaveformInfo[i]->amax;

		xw1=0;
		xw2=MaxX-1;
		yw1=yHeight*nTrace+TimeBarYsize;
		yw2=(yHeight*(nTrace+1)+TimeBarYsize)-5*p_mm_y;

		x1=WindowInfoXsize;
		x2=MaxX;

		y1=yw1+1;
		y2=yw2-1;

		painter.setPen(Pen);
		painter.setPen(BoldPen);
		pWD->DrawWindow(j, &painter,  x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX, true);

//      Draw left side

		painter.setPen(BoldPen);
		painter.drawLine(xw1+WindowInfoXsize,y1,xw1+WindowInfoXsize,y2);
		


		painter.drawLine(xw1+WindowInfoXsize,y1, xw1+WindowInfoXsize+2*p_mm_x,yw1);

		
		painter.drawLine(xw1+WindowInfoXsize,y2, xw1+WindowInfoXsize+2*p_mm_x,yw2);
		painter.setPen(Pen);

		fAmplFont.setPixelSize(5*p_mm_y);
		painter.setFont(fAmplFont);

		QRect rcx;

		rcx.setLeft(xw1);
		rcx.setRight(WindowInfoXsize-2*p_mm_x);
		rcx.setTop(yw1);
		rcx.setBottom( yw2);

		sprintf(cText,"%d",amax);
		painter.drawText(rcx, Qt::AlignRight | Qt::AlignTop, cText);

		sprintf(cText,"%d",amin);
		painter.drawText(rcx, Qt::AlignRight | Qt::AlignBottom, cText);


		painter.setFont(fStaNameFont);
		sprintf(cText,"%s::%s",  (const char *)pWD->WaveformInfo[i]->StaName.toAscii(), (const char *)pWD->WaveformInfo[i]->ChanName.toAscii());
		painter.drawText(rcx, Qt::AlignVCenter | Qt::AlignHCenter, cText);

		
		}

	xw1 = WindowInfoXsize;
	xw2 = MaxX;
	yw1 = 0;
	yw2 = TimeBarYsize;

	painter.setFont(fAmplFont);
	DrawTimeMarks(painter, pWD->t1, pWD->t2, xw1, xw2, yw1, yw2, MaxY);
	DrawGrid(painter);
	PrintStatus(painter);

	painter.end();
	}

void WaveformsPrint::CalcTimeBarYsize(QPainter &painter)
	{
	QFontMetrics fm = painter.fontMetrics();
	int textHeightInPixels = fm.height();

	TimeBarYsize=(textHeightInPixels)*2+8;
	}
void WaveformsPrint::GetTextRectangle(QPainter &painter, QString &p, QRect &qrc)
	{
	QFontMetrics fm = painter.fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}

void WaveformsPrint::DrawTimeMarks(QPainter &painter, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY)
	{
	int x;


	QPen   BlackPen(QColor(0, 0, 0));
	QRect rcc1,rcc2;

	int delta=pWD->pTimeRuler->GetDeltaInterval();
	if(delta==0) delta=1;

	int nLastMarkRightPos=-1;
	long ttstart= (long)t1 - ((long)t1)%3600;


	painter.setPen(BlackPen);

	for(long tt=ttstart; tt<t2; tt++)
		{
		if(tt<t1) continue;
		if(((int)tt)%delta==0)
			{
			x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);
			{
			char cBuff1[64], cBuff2[64];


			strcpy(cBuff1, util_dttostr(tt, 15));
			QString ss(cBuff1); 
			GetTextRectangle(painter, ss, rcc1);


			rcc1.moveLeft( x - rcc1.width()/2);
			rcc1.moveTop(MaxY - (yw2-1));


			strcpy(cBuff2, util_dttostr(tt, 4));
			ss = QString(cBuff2);
			GetTextRectangle(painter, ss,rcc2);

			rcc2.moveLeft( x - rcc2.width()/2);
			rcc2.moveTop(MaxY - (yw2-1-rcc1.height()));



			if( nLastMarkRightPos<min(rcc2.left(),rcc1.left()))
				{
				painter.drawText(rcc1,Qt::AlignCenter,cBuff1);
				painter.drawText(rcc2,Qt::AlignCenter,cBuff2);
				nLastMarkRightPos=max(rcc1.right(),rcc2.right());
				tmmarks.push_back(x);
				}

			}

			}


		}
	}
void WaveformsPrint::DrawGrid(QPainter &painter)
	{
	unsigned int i;
	QPen dotPen(QColor(0,0,0), 1, Qt::DotLine);
	painter.setPen(dotPen);
	for(i=0; i<tmmarks.size(); ++i)
		{
		int x=tmmarks[i];
		painter.drawLine(x,MaxY-TimeBarYsize,x,nHeadHeight+topMargin);
		}
	}
void WaveformsPrint::PrintStatus(QPainter &painter)
	{
	QRect rcText;
	QFont fStatusFont;


	fStatusFont.setPixelSize(4*p_mm_y);
	painter.setFont(fStatusFont);

	QString sPrint;
	sPrint="Filter:";
	if(pWD->nFilterType==-1)
		{
		sPrint+="none";
		}
	else
		{
			sPrint=sPrint+QWaveformsDisplay::Filters[pWD->nFilterType].label;
		}

	sPrint+=", Amp:";
	switch(pWD->DisplayMode)
		{
		case QWaveformsDisplay::mFx:
			sPrint+="Fixed";
			break;
		case QWaveformsDisplay::mAuto:
			sPrint+="Auto";
			break;
		case QWaveformsDisplay::mAuto0:
			sPrint+="Auto0";
			break;
		case QWaveformsDisplay::mAuto1:
			sPrint+="Auto1";
			break;
		case QWaveformsDisplay::mAutoA:
			sPrint+="AutoA";
			break;
		}
	

	sPrint+="  File:"+pWD->WfdFileName;

	GetTextRectangle(painter, sPrint, rcText);


	int n1=(MaxY-rcText.bottom());
	int n2=(MaxY-rcText.top());
	rcText.setTop(n2);
	rcText.setBottom(n1);


	painter.drawText(rcText, Qt::AlignLeft | Qt::AlignVCenter, sPrint);
		

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
 * $Log: WaveformsPrint.cpp,v $
 * Revision 1.2  2010/06/07 18:32:51  akimov
 * Changed pen width and style in order to correct problem with first trace in PDF output
 *
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

 
