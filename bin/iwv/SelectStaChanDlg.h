#pragma ident "$Id: SelectStaChanDlg.h,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#pragma once
#include <QDialog>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include "StaInfo.h"

class SelectStaChanDlg :
	public QDialog
{
Q_OBJECT

public:
	SelectStaChanDlg(QWidget* parent , Qt::WFlags fl  );
	virtual ~SelectStaChanDlg(void);
	void SetStaInfo(CStaInfo &Info);
	void GetStaInfo(CStaInfo &Info);
protected:
	void InitStaTable();
	void InitChanTable();
	void InitLayouts();
	void GetWidgetTextRectangle(QWidget *pw, const char *p, QRect &qrc);
	void LoadStaList();
	void LoadChanList(QString &sSta);
	void SetChanState(const QString &sSta, const QString &sChan, bool bSelected);
	void SetStationState(const QString &s);
	void SelUnselAllChan(int nStaRow, bool bSel);
	void SetStationState(int nRow, bool bAnySelected);
	bool IsAnyChanSelected(const QString &s);
	bool IsAnyChanSelected(int nSta);




	CStaInfo StaInfo;
	QString ActiveSta;

	QTableWidget *StaTable;
	QTableWidget *ChanTable;

	int TableRowHeight;
protected slots:
	void on_ok_button();
	void on_cancel_button();
	void itemChanged ( QTableWidgetItem * item );
	void OnClickChanListTableItem(int row, int col);
	void on_unselectall_button();
	void on_selectall_button();
	void OnCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);


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
 * $Log: SelectStaChanDlg.h,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
