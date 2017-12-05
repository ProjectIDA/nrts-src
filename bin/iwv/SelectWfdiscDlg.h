#pragma ident "$Id: SelectWfdiscDlg.h,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#pragma once
#include <QFileDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QLayout>
#include "WfdiscRecord.h"
#include "WfdiscRecordsArray.h"


class SelectWfdiscDlg :	public QFileDialog
{
Q_OBJECT

public:
	SelectWfdiscDlg(QWidget * parent, Qt::WindowFlags flags);
	virtual ~SelectWfdiscDlg(void);

	void GetPreviewTextRectangle(const char *p, QRect &qrc);
static QString LastDir;
private:
	QTableWidget *qtw;
	int PreviewRowHeight;
	void LoadPreview(const char *lpFileName);
	void DeleteAllRawsInPreview();
	void SetPreviewColumnSize();

protected slots:
virtual	void wfd_selected(const QString & );

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
 * $Log: SelectWfdiscDlg.h,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

