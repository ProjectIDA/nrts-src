#pragma ident "$Id: QWaveformPreview.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#include "QWaveformPreview.h"

QWaveformPreview::QWaveformPreview(QWaveformsDisplay *pWaveformsDisplay, QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	this->pDisplay=pWaveformsDisplay; 
	setMouseTracking(true);
	}

QWaveformPreview::~QWaveformPreview(void)
	{
	}
QPen  QWaveformPreview::WhitePen  = QPen(QColor(255, 255, 255));
QPen  QWaveformPreview::BlackPen  = QPen(QColor(0, 0, 0));
QPen  QWaveformPreview::whitePen2 = QPen(QColor(255, 255, 255), 2);
QPen  QWaveformPreview::grayPen2 = QPen(QColor(64, 64, 64), 2);
QPen  QWaveformPreview::bluePen = QPen(QColor(0, 0, 128), 1);


void QWaveformPreview::paintEvent( QPaintEvent * event )
	{
	event->accept();
	QPainter p;
 

	p.begin(this);

	ScreenBitmapCriticalSection.lock();

	if(!Bitmap.isNull())
		{
		p.drawPixmap(0, 0, Bitmap);
		}
	ScreenBitmapCriticalSection.unlock();






	p.end();

	}

void QWaveformPreview::mouseMoveEvent ( QMouseEvent * pevent )
	{
	int x=pevent->x();
	int y=pevent->y();


	if(pDisplay->t1<0 || pDisplay->t2<0 || pDisplay->tB1<0 || pDisplay->tB2<0) return;


	double ttt=(x-x1)*(pDisplay->tB2-pDisplay->tB1)/(x2-x1)+pDisplay->tB1;


	char cBuff[64];	
	sprintf(cBuff,"T:%s", util_dttostr(ttt, 1));

	QMainWindow * qw = (QMainWindow *)parentWidget(); 
	qw->statusBar()->showMessage ( cBuff, 5000);

	}
void QWaveformPreview::mouseReleaseEvent ( QMouseEvent * pevent ) 
	{
	if(pevent->button()==Qt::LeftButton)
		{
		OnLButtonUp(pevent);
		return;
		}
	}

void QWaveformPreview::OnLButtonUp(QMouseEvent *pevent)
	{
	int x=pevent->x();
	int y=pevent->y();

	double tB1=pDisplay->tB1;
	double tB2=pDisplay->tB2;
	double t1=pDisplay->t1;
	double t2=pDisplay->t2;
	double half=(t2-t1)/2.;

	double ttt=(x-x1)*(tB2-tB1)/(x2-x1)+tB1;

	pDisplay->t1=ttt-half;
	pDisplay->t2=pDisplay->t1+2.*half;

	if(pDisplay->t2>tB2)
		{
		pDisplay->t2=tB2;
		pDisplay->t1=tB2-2.*half;
		}
	if(pDisplay->t1<tB1)
		{
		pDisplay->t1=tB1;
		pDisplay->t2=tB1+2.*half;
		}

	DrawPreviewBitmap();
	update();
	pDisplay->DrawWaveforms();
	}

void QWaveformPreview::DrawPreviewBitmap() 
	{
	QSize qsz = size();
	MaxX= qsz.width()-1;
	MaxY= qsz.height()-1;

	


	x1=4;
	x2=MaxX-4;
	y1=4;
	y2=MaxY-4;

	xw1=1;
	xw2=MaxX;
	yw1=0;
	yw2=MaxY; 



	QImage Bitmap1(qsz.width(),  qsz.height(), QImage::Format_RGB32);
	QPainter painter;

	painter.begin(&Bitmap1);
	painter.fillRect(0,0, width(), height(), QBrush(QColor(128, 128, 128)/*, Qt::LinearGradientPattern*/));
	painter.fillRect(0,0, width(), 4, QBrush(QColor(192, 192, 192)));
	painter.fillRect(0,height()-4-1, width(), 4, QBrush(QColor(192, 192, 192)));

	painter.setPen(WhitePen);
	
	painter.drawLine(0, 0, MaxX,  0);
	painter.drawLine(0, MaxY-1, MaxX,  MaxY-1);

/*	painter.setPen(BlackPen);
	painter.drawLine(0, MaxY, MaxX,  MaxY);*/

	painter.setPen(whitePen2);
	int xl=(int)((pDisplay->t1 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	int xr=(int)((pDisplay->t2 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	
	 
	QRect rc(xl, MaxY-yw2, (xr-xl)+1, (yw2-yw1));

//	p.drawRect(rc);
	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setPen(bluePen);
//	painter.fillRect(rc, QBrush(QColor(0,0,255), Qt::Dense4Pattern));
	painter.setBrush(QBrush(QColor(0,0,255), Qt::Dense4Pattern));
	painter.drawRect(rc);

//	painter.setBackgroundMode(Qt::OpaqueMode);



	DrawTimeMarks(&painter, pDisplay->tB1, pDisplay->tB2,  xw1, xw2, yw1, yw2, MaxY);

	painter.end();
	ScreenBitmapCriticalSection.lock();
	Bitmap = QPixmap::fromImage(Bitmap1);
	ScreenBitmapCriticalSection.unlock();

	}
void QWaveformPreview::resizeEvent(QResizeEvent * event)
	{
	DrawPreviewBitmap();
	update();
	}
void QWaveformPreview::DrawTimeMarks(QPainter *painter, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY)
	{
	int x;
	QBrush brush(QColor(255,0,0));

	QRect rcc1,rcc2;

	int delta=nTimeLabelStep();

	if(delta<=0) return;

	int nLastMarkRightPos=-1;
	long ttstart= (long)t1 - ((long)t1)%3600;


	painter->setPen(WhitePen);

	for(long tt=ttstart; tt<t2; tt+=delta)
		{
		if(tt<t1) continue;
	
			x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);
			{
			char cBuff1[64], cBuff2[64];
			painter->setPen(BlackPen);
			painter->drawLine(x, (MaxY-yw2), x,  (MaxY-yw2)+3);
			painter->setPen(WhitePen);
			painter->drawLine(x+1, (MaxY-yw2), x+1,  (MaxY-yw2)+3);

			strcpy(cBuff1, util_dttostr(tt, 15));
			GetTextRectangle(painter, cBuff1, rcc1);


			rcc1.moveLeft( x - rcc1.width()/2);
			rcc1.moveTop(MaxY - (yw2-3));


			strcpy(cBuff2, util_dttostr(tt, 4));

			GetTextRectangle(painter, cBuff2,rcc2);

			rcc2.moveLeft( x - rcc2.width()/2);
			rcc2.moveTop(MaxY - (yw2-3-rcc1.height()));



			if( nLastMarkRightPos<min(rcc2.left(),rcc1.left()))
				{
				painter->drawText(rcc1,Qt::AlignCenter,cBuff1);
				painter->drawText(rcc2,Qt::AlignCenter,cBuff2);
				nLastMarkRightPos=max(rcc1.right(),rcc2.right());
				}

			}

			


		}
	}
void QWaveformPreview::GetTextRectangle(QPainter *painter, const char *p, QRect &qrc)
	{
	QFontMetrics fm = painter->fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}
int QWaveformPreview::nTimeLabelStep()
	{
	int i;
	QFontMetrics fm = fontMetrics();
	int textWidthInPixels = fm.width("XX:XX:XX")*3;
	int nTotalPixel = size().width();

	int nRatio = nTotalPixel/textWidthInPixels;

	int nSec = (pDisplay->tB2-pDisplay->tB1)/nRatio;

	if( nSec<1 )  return 1;
	if( nSec<10 )  return 10;
	if( nSec<60 )  return 60;
	if( nSec<300 )  return 300;
	if( nSec<600 ) return 600;
	if( nSec<1200 ) return 1200;
	if( nSec<1800) return 1800;

	for(i=1; i<24; ++i)
		if( nSec<3600*i) return 3600*i;
	
	for(i=1; i<10; ++i)
		if( nSec<3600*24*i) return 3600*24*i;

	return -1;
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
 * $Log: QWaveformPreview.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
