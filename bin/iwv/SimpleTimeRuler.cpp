#pragma ident "$Id: SimpleTimeRuler.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "SimpleTimeRuler.h"
#include "util.h"

SimpleTimeRuler::SimpleTimeRuler(QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	tbeg = 0;
	tend = 0;
	bSetTime = false;
	}

SimpleTimeRuler::~SimpleTimeRuler(void)
	{
	}

void SimpleTimeRuler::paintEvent( QPaintEvent * event )
	{
	event->accept();
	QPainter p;


	p.begin(this);

	if(!timePanel.isNull())
		{
		p.drawPixmap(0, 0, timePanel);
		}
	p.end();
	}

void SimpleTimeRuler::SetTime(double t1, double t2)
	{

	tbeg = t1; tend = t2;

	bSetTime = true;
	}

void SimpleTimeRuler::SetInfoWidth(int nWidth)
	{
	WindowInfoXsize = nWidth;
	}

void SimpleTimeRuler::DrawTimeMarks(QPainter *painter, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY)
	{
	int x;
	QBrush brush(QColor(255,0,0));
	QPen   WhitePen(QColor(255, 255, 255));
	QPen   BlackPen(QColor(0, 0, 0));
	QRect rcc1,rcc2;

	int delta=GetDeltaInterval();
	if(delta==0) delta=1;

	nLastMarkRightPos=-1;
	long ttstart= (long)t1 - ((long)t1)%3600;

	tmmarks.clear();

	painter->setPen(WhitePen);

	for(long tt=ttstart; tt<t2; tt++)
		{
		if(tt<t1) continue;
		if(((int)tt)%delta==0)
			{
			x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);
			{
			char cBuff1[64], cBuff2[64];


			strcpy(cBuff1, util_dttostr(tt, 15));
			GetTextRectangle(painter, cBuff1, rcc1);


			rcc1.moveLeft( x - rcc1.width()/2);
			rcc1.moveTop(MaxY - (yw2-1));


			strcpy(cBuff2, util_dttostr(tt, 4));

			GetTextRectangle(painter, cBuff2,rcc2);

			rcc2.moveLeft( x - rcc2.width()/2);
			rcc2.moveTop(MaxY - (yw2-1-rcc1.height()));



			if( nLastMarkRightPos<min(rcc2.left(),rcc1.left()))
				{
				painter->drawText(rcc1,Qt::AlignCenter,cBuff1);
				painter->drawText(rcc2,Qt::AlignCenter,cBuff2);
				nLastMarkRightPos=(rcc1.right(),rcc2.right());
				tmmarks.push_back(x+WindowInfoXsize);
				}

			}

			}


		}
	}



void SimpleTimeRuler::DrawTimePannel()
	{
	QSize sz = size();
	int MaxY=sz.height();
	QImage Bitmap1(sz, QImage::Format_ARGB32_Premultiplied);
    QBrush BlueBrush(QColor(0,0,128));
    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));

	QPainter painter;

	painter.begin(&Bitmap1);


	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setPen(WhitePen);


	int xw1=WindowInfoXsize;
	int xw2=sz.width()-1;
	int yw1=0;
	int yw2=sz.height()-1;
	int x, x_old;

	QRect rc(0, 0, sz.width(), sz.width());

	painter.fillRect(rc, BlueBrush);

	painter.setPen(WhitePen);

	painter.drawLine(xw1,MaxY-yw2, xw2,MaxY-yw2);
	painter.drawLine(xw1,MaxY-yw2, xw1,MaxY-yw1);


	painter.setPen(DarkPen);


	painter.drawLine(xw2,MaxY-yw1, xw2,MaxY-yw2);
	painter.drawLine(xw2,MaxY-yw1, xw1,MaxY-yw1);


	x_old=-1;



	DrawTimeMarks(&painter, tbeg, tend, xw1, xw2, yw1, yw2, MaxY);

	painter.end();

	ScreenBitmapCriticalSection.lock();
	timePanel = QPixmap::fromImage(Bitmap1);
	ScreenBitmapCriticalSection.unlock();
	update();
	}

void SimpleTimeRuler::GetTextRectangle(QPainter *painter, const char *p, QRect &qrc)
	{
	QFontMetrics fm = painter->fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}
 long SimpleTimeRuler::GetDeltaInterval()
	{
	static int deltas[]={1,5,10,20,30,60,300,600,1200,1800,3600};
	float delta=(float)((tend-tbeg)/10.);

	for(int i=0;i<sizeof(deltas)/sizeof(int);++i)
		{
		if(delta<=deltas[i]) return deltas[i];
		}
	return ((long)delta/3600)*3600;
	}

int SimpleTimeRuler::CalcTimeRulerYsize()
	{
	QFontMetrics fm = fontMetrics();
	int textHeightInPixels = fm.height();

	return (textHeightInPixels)*2+8;
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
 * $Log: SimpleTimeRuler.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

