#pragma ident "$Id: WaveformsPrint.h,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#pragma once
#include "QWaveformsDisplay.h"
#include <QDialog>
#include <vector>

class WaveformsPrint
{
public:
	WaveformsPrint(QWaveformsDisplay *pWaveformsDisplay);
	virtual ~WaveformsPrint(void);
	void Print();
protected:
	

private:
	float p_mm_x, p_mm_y;
	int MaxX, MaxY, nHeadHeight;
	int topMargin;
	int TimeBarYsize,WindowInfoXsize;
	std::vector <int> tmmarks; // array time - marks

private:

	void PrintStatus(QPainter &);
	void GetTextRectangle(QPainter &pdc, QString &, QRect &rc);
	void DrawTimeMarks(QPainter &pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY);
	void PrintWaveforms(QPrinter &printer, QString &);
	void CalcTimeBarYsize(QPainter &painter);
	void DrawGrid(QPainter &painter);
	void DrawWindow(int nWin, QPainter *painter, int TimeBarYsize,int x1, int x2, int y1, int y2,
   int xw1, int xw2, int yw1, int yw2, int MaxY, int MaxX, bool bPrint);


private:
	QWaveformsDisplay *pWD;
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
 * $Log: WaveformsPrint.h,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
