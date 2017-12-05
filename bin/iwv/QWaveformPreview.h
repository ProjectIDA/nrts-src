#pragma ident "$Id: QWaveformPreview.h,v 1.1 2010/05/11 18:24:59 akimov Exp $"

#pragma once
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMainWindow>
#include <QStatusBar>
#include <QPen>
#include <QColor>
#include "QWaveformsDisplay.h"
#include "util.h"

class QWaveformPreview :public QWidget
{
public:
	QWaveformPreview(QWaveformsDisplay *pDisplay, QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~QWaveformPreview(void);
	void DrawPreviewBitmap();
protected:
	void paintEvent( QPaintEvent * event );
	void mouseReleaseEvent ( QMouseEvent * pevent );
	void mouseMoveEvent ( QMouseEvent * pevent );
	void OnLButtonUp( QMouseEvent *pevent );
	void resizeEvent(QResizeEvent * event);
	void GetTextRectangle(QPainter *painter, const char *p, QRect &qrc);
	void DrawTimeMarks(QPainter *painter, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY);
	int nTimeLabelStep();
	

	QPixmap Bitmap;
	QMutex ScreenBitmapCriticalSection;
	QWaveformsDisplay *pDisplay;

	int MaxX, MaxY;
	int x1,x2,y1,y2,xw1, xw2, yw1, yw2; 
	static QPen   WhitePen;
	static QPen   BlackPen;
	static QPen   whitePen2;
	static QPen   grayPen2;
	static QPen   bluePen;

protected slots:

};
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
 * $Log: QWaveformPreview.h,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
