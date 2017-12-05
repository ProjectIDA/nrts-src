#pragma ident "$Id: IWVConfig.h,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#pragma once
#include <QDialog>
#include <QString>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>


class IWVConfig :	public QDialog
{
Q_OBJECT
public:
	IWVConfig(QWidget* parent = 0, Qt::WFlags fl = 0);
	virtual ~IWVConfig(void);
	QLineEdit *MaxTraces;
	QLineEdit *displaywidth;
	QLineEdit *displayheight;
	QLineEdit *font;
	QLineEdit *fontsize;


	static int display_width;
	static int display_height;

	static QString FontName;
	static int nFontSize;
	static int m_MaxWindow;
	static void Loadiwvrc();
	static void Saveiwvrc();

protected:
	bool CheckValues();


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
 * $Log: IWVConfig.h,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
