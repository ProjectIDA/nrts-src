#pragma ident "$Id: QWaveformsDisplay.h,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMutex>
#include <QString>
#include <vector>
#include "MWindow.h"
#include "WfdiscRecordsArray.h"
#include "StaInfo.h"
#include "InfoWindow.h"
#include "Screen.h"
#include "SimpleTimeRuler.h"
#include "MainWindowInterface.h"

const int WindowInfoXsize=100;

class QWaveformsDisplay:public QWidget
{
	friend class WaveformsPrint;
	friend class SimpleTimeRuler;
public:
	QWaveformsDisplay(SimpleTimeRuler *pTimeRuler, MainWindowInterface *,QWidget * parent,  Qt::WindowFlags f = 0);
	virtual ~QWaveformsDisplay(void);

	void ClearWaveformInfo();
	void DrawFocused(int, QPainter *);
	void LoadWfdiscFileInfo(QString &FileName);
	void RunSelectDlg();
	void FillWaveformInfo();
	void FindMinMaxTimeInWaveformInfo();
	int CalculateMWindowsSize();
	void DrawWaveforms();
	void DrawWindow(int nWin, QPainter *painter, int x1, int x2, int y1, int y2,
	int xw1, int xw2, int yw1, int yw2, int MaxY, int MaxX, bool bPrint=FALSE);
	void GetTextRectangle(QPainter *, const char * , QRect &);

	void RedrawPreview();
	void RepaintPreview();
	void OnFitToWindow();
	void OnExpandHorizontal();
	void OnContractHorizontal();
	void OnExpandVertical();
	void OnContractVertical();
	void OnShiftToLeft();
	void OnShiftToRight();
	void OnShiftToDown();
	void OnShiftToUp();
	int GetWindowFromPoint(int x, int y);
	void ContractX(int x);
	void ContractY(int y);
	void OnFitToWindow(int);
	void Original();
	void SelectTraces();
	void SelectAll();
	void DeleteAll();
	void SetFitMaxMin(int nWf);
	void SetAmpFlagFixed();
	void SetAmpFlagAuto();
	void SetAmpFlagInvert();
	void SetAmpFlagAuto0();
	void SetAmpFlagAuto1();
	void SetAmpFlagAutoA();
	void SetStartTime();
	long GetDeltaInterval();
	void Fit();
	void FitVertical();
	void DeleteTraces();


	void ExpandY(int y);
	void ExpandX(int x);
	void RedrawWindow(int nWindow);
	bool ClipOnOff();
	int GetLeftPanelNumberFromPoint(int x, int y);
	void AutoA();
	void Auto0();
	void ApplyAuto();
	void ApplyAuto0();
	void ApplyAuto1();
	void ApplyAutoA();
	void ApplyScale();
	void GetMinMaxForTrace(int nWf, long &amax, long &amin);
	void FlipInvertFlag();
	void ChangeGain(double nGain, int flag);
	void DrawTimeMark(int x, double tt, QPainter *pDC);
	void DrawLeftPannel();
	void DrawLeftPannelWindow(int nScreen, QPainter *pDC);
	void DrawTimePannel(double t1, double t2);
	void MakeShiftedScreenBmp();
	void SetFilterNumber(int nFilt);
	void DrawTimeMarks(QPainter *pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY);
	void Print();
	void ProcessCursorAction(int x, int y, QMouseEvent * pevent);
	void setBegTime(double tt);
	void setEndTime(double tt);
	double getBegTime();
	double getEndTime();
	void toEnd();
	void toBegin();
	void AdjustWidgetSize();
	void adjusth();
//	void SetTimeRuler(SimpleTimeRuler *pTimeRuler);
	void SetTWIN(double t);
	double Interpolate(double tt1, double aa1, double tt2, double aa2, double tt);
	bool GetVisibleTraces(int &nFirst, int &nLast);
	void SyncStaInfo();
	void MakeScreenInfoFromWaveformInfo();



//********************************************
protected:
	void GetTextRectangle(const char *lpText, QRect &rc);
	void InvertRect(int x1, int y1, int x2, int y2);
	void DrawDraggedBitmap(QPainter &painter);
	QRect InvRect;
	bool bSelectionActive;
	SimpleTimeRuler *pTimeRuler;

public:
	QImage Bitmap; // off screen bitmap
	QPixmap leftPannel;
	QPixmap offscrbmp;
	QImage selectedReg;
	std::vector <CMWindow *> WaveformInfo; // array of all waveforms 
	std::vector <int> tmmarks; // array time - marks 
	static std::vector<struct gather_plot_filter_> Filters;



	QMutex ScreenBitmapCriticalSection;

	CWfdiscRecordsArray wfdarray;

	int MaxX, MaxY;
	int xcursor,ycursor;
	double dGain;
	double t1,t2;
	double tB1,tB2;
	int nActiveScreen;
	CStaInfo AllStaChan;
	bool bClip;
	bool bPanelSelectionActive;
	bool bInvert;
	int oldPanelAct;
	int DisplayMode;

//****************** BOX *****************
private:
	int nAmpFlag;
	int mouse_x;
	int mouse_y;

	int mouse_x1;
	int mouse_y1;

	bool bBox;
	int nBoxWindow;
	bool bBoxExpandX;
	bool bBoxContractX;
	bool bBoxExpandY;
	bool bBoxContractY;
	bool bMiddleButtonDrag;
	int mouse_x_old,mouse_y_old;
	enum DisplayModes { mFx, mAuto, mAuto0, mAuto1, mAutoA}; 
	int nLastMarkRightPos;
	QString WfdFileName;
	MainWindowInterface *MainWindow; 
	double twin;
public:
	CInfoWindow infwnd;
	CInfoWindow amplinfwnd;
	CScreen *pScreen;
public:
	int nWindowFromPoint(int x, int y);

	void RunSetTimeDlg();
	void ApplyGain();
	int mbuttonx;
	int mbuttony;
	int xX1, xX2;
	int nFilterType;

protected:
	void OnLButtonDown(QMouseEvent * pevent);
	void OnRButtonDown(QMouseEvent * pevent);
	void OnLButtonUp(QMouseEvent * pevent);
	void OnRButtonUp(QMouseEvent * pevent);
	void OnMButtonUp(QMouseEvent * pevent);
	void OnMButtonDown(QMouseEvent * pevent);
//****************************************
void DrawInverted(QPainter &);


protected:
	void paintEvent( QPaintEvent * pevent );
	void mousePressEvent(QMouseEvent * pevent);
	void mouseReleaseEvent ( QMouseEvent * pevent );
	void mouseMoveEvent ( QMouseEvent * pevent );
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
 * $Log: QWaveformsDisplay.h,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
