#pragma ident "$Id: SimpleTimeRuler.h,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#pragma once
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QMutex>
#include <vector>

//class WaveformsPrint;

class SimpleTimeRuler :	public QWidget
{
//	friend WaveformsPrint;
public:
	SimpleTimeRuler(QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~SimpleTimeRuler(void);
	void SetTime(double t1, double t2);
	void SetInfoWidth(int nWidth);
	void DrawTimePannel();
	int CalcTimeRulerYsize();
	long GetDeltaInterval();
public:
	std::vector <int> tmmarks; // array time - marks
protected:
	void DrawTimeMarks(QPainter *painter, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY);
	void GetTextRectangle(QPainter *painter, const char *p, QRect &qrc);
	 
	 void DrawTimeMark(QPainter *painter, int x, double tt, int MaxY);
protected:

	double tbeg, tend;
	bool bSetTime;
	int WindowInfoXsize;
	long nLastMarkRightPos;
	QPixmap timePanel;
	QMutex ScreenBitmapCriticalSection;
protected:
	void paintEvent( QPaintEvent * event );
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
 * $Log: SimpleTimeRuler.h,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
