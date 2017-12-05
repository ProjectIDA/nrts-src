#pragma ident "$Id: MainWindow.h,v 1.1 2010/05/11 18:24:59 akimov Exp $"

#if !defined(_MAINWINDOW_H_)
#define _MAINWINDOW_H_
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QMainWindow>
#include <QWidget>
#include <QCloseEvent>
#include <QMap>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include "QWaveformPreview.h"
#include "QWaveformsDisplay.h"
#include "SimpleTimeRuler.h"
#include "MainWindowInterface.h"
#include <vector>


class MainWindow : public QMainWindow,  public  MainWindowInterface 
{
Q_OBJECT


public:
	MainWindow(QWidget * parent = 0, Qt::WFlags f = Qt::Window);
	virtual ~MainWindow();
	QPixmap image0;
	QWaveformPreview   *m_Preview;
	QWaveformsDisplay  *m_Display;
	SimpleTimeRuler    *m_TimeRuler;
	QScrollArea * scrollArea;
	QMenu *pAmpMenu;
	QMenu * pFilterMenu;
	QAction *a_ampfixed, *a_gain1, *a_invert, *a_cliponoff, *a_autoA, *a_auto1, *a_auto0, *a_auto;
	std::vector <QAction *>filter_menus;
	std::vector <struct gather_plot_filter_> Filters;
	void LoadFilterMenu(QMenu * pMenu);
	void LoadPreview(const char *lpFileName);
	bool setWfdisc(const char *pName);
	void StartOpen();
	void LoadFilters();
	void LoadDBpickrc();
	void SetStatusBarText(const char *pText);
	void UpdatePreviewWindow(void);
	void AdjustWaveformWindowSize();
	void AdjustWindowsSize(QSize &qsz);
	void AdjustWindowsSize();

	
protected:
	double twin;

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent * event);

protected slots:
	virtual void file_open();
	virtual void amp_on_ampfixed(); 
	virtual void amp_on_auto();
	virtual void amp_on_auto1();
	virtual void amp_on_auto0();
	virtual void amp_on_autoA();
	virtual void traces_select(); 
	virtual void traces_select_all();
	virtual void traces_delete_all();
	virtual void traces_original();
	virtual void traces_cliponoff();
	virtual void traces_delete();
	virtual void traces_invert();
	virtual void traces_gain1(); 
	virtual void filter_selected(QAction *pAA);
	virtual void commands_fit();
	virtual void commands_fit_vertical();
	virtual void help_about();
	virtual void traces_choose();
	virtual void file_print();
	virtual void commands_settime();
	virtual void commands_tobegin();
	virtual void commands_toend();
	virtual void commands_config();
signals:
	void openWFDISC();
	void updatePreview();
};

#endif // !defined(_MAINWINDOW_H_)
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
 * $Log: MainWindow.h,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
