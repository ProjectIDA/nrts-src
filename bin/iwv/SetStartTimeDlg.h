#pragma ident "$Id: SetStartTimeDlg.h,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#pragma once
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateTimeEdit>

class SetStartTimeDlg :	public QDialog
{
Q_OBJECT

public:
	SetStartTimeDlg(QWidget* parent, Qt::WFlags);
	virtual ~SetStartTimeDlg(void);

	QDateTimeEdit *beg;
	QDateTimeEdit *end;

	void setBegTime(double t1);
	void setEndTime(double t1);
	double getBegTime();
	double getEndTime();



protected slots:
	void on_ok_button();
	void on_cancel_button();

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
 * $Log: SetStartTimeDlg.h,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */

