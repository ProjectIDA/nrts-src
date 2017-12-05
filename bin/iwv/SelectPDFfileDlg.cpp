#pragma ident "$Id: SelectPDFfileDlg.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "SelectPDFfileDlg.h"
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>

QString SelectPDFfileDlg::LastDir     = ""; 
QString SelectPDFfileDlg::sHeaderText = "Interactive Waveform Viewer";

SelectPDFfileDlg::SelectPDFfileDlg(QWidget * parent, Qt::WindowFlags flags):QFileDialog(parent, flags) 
	{
	QStringList fileNames;
	setReadOnly(true);
	setAcceptMode(QFileDialog::AcceptSave);
	setFilter(tr("pdf (*.pdf)")); 
//	if(!LastDir.isEmpty()) setDirectory(LastDir);
	setDirectory(QDir::homePath());

	QLabel *textLabel0 = new QLabel("Header:");
	headerText = new QLineEdit(); 
	QGroupBox *groupBox0 = new QGroupBox();
	QHBoxLayout *HLayout0 = new QHBoxLayout();

	QGridLayout *lout = (QGridLayout *)layout(); 
	lout->addWidget(groupBox0, 5, 0, 1, -1);
	groupBox0->setLayout(HLayout0);
	HLayout0->addWidget(textLabel0);
	HLayout0->addWidget(headerText);
	headerText->setText(sHeaderText);

	}

SelectPDFfileDlg::~SelectPDFfileDlg(void)
	{
	}
QString SelectPDFfileDlg::GetHeaderText()
	{
	QString s;
	s =	headerText->text();
	sHeaderText = s;
	return s;
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
 * $Log: SelectPDFfileDlg.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
