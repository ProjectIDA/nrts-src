#pragma ident "$Id: itsvqtAbout.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "itsvqtAbout.h"
#include <qobject.h>
#include <QVariant>
#include <QLayout>
#include <qtooltip.h>
#include <QVBoxLayout> 
#include <QGroupBox>
#include <QPushButton>
#include <QSizePolicy>

itsvqtAbout::itsvqtAbout( QWidget* parent, Qt::WFlags fl ) : QDialog( parent, fl ) 
	{
    setModal( TRUE );
	QHBoxLayout *HLayout0 = new QHBoxLayout(this); 
	QVBoxLayout *VLayout0 = new QVBoxLayout(); 

	QGroupBox *groupBox0 = new QGroupBox();
	QGroupBox *groupBox1 = new QGroupBox();

	HLayout0->addWidget(groupBox0);
	HLayout0->addWidget(groupBox1);

	textLabel0 = new QLabel( this);
    textLabel1 = new QLabel( this);
//	textLabel1->SetURL(QString("http:\\\\www.ida.ucsd.edu"));
	textLabel1->setCursor( Qt::PointingHandCursor );
    textLabel2 = new QLabel( this);
	textLabel3 = new QLabel( this);
	textLabel4 = new QLabel( this);
    textLabel4->setCursor( Qt::PointingHandCursor );

    languageChange();
    resize( QSize(330, 138).expandedTo(minimumSizeHint()) );
	VLayout0->addWidget(textLabel0);
	VLayout0->addWidget(textLabel3);
	VLayout0->addWidget(textLabel4);

	groupBox0->setLayout(VLayout0);

    setWindowTitle(  "About Interactive Waveform Viewer"  );
    textLabel0->setText("<b>IWV Version 1.0</b>");
    

	textLabel3->setText("Developed for Project IDA, UC San Diego by"  );
	textLabel4->setText( "Andrey Akimov"  );


	QPushButton *ok_button = new QPushButton("Ok");
	QVBoxLayout *VLayoutR = new QVBoxLayout(); 
	groupBox1->setLayout(VLayoutR);
	VLayoutR->addWidget(ok_button);
	connect(ok_button, SIGNAL(released()), this, SLOT(close()));

	adjustSize(); 
	this->setSizeGripEnabled(false);
	setFixedSize(QSizePolicy::Fixed, QSizePolicy::Fixed);
	}

itsvqtAbout::~itsvqtAbout(void)
	{
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
 * $Log: itsvqtAbout.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */