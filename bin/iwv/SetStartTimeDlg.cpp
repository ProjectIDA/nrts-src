#pragma ident "$Id: SetStartTimeDlg.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "SetStartTimeDlg.h"

SetStartTimeDlg::SetStartTimeDlg(QWidget* parent = 0, Qt::WFlags fl = 0):QDialog(parent, fl)
	{
    setModal( TRUE );

	beg = new QDateTimeEdit(this);
	end = new QDateTimeEdit(this);
	beg->setDisplayFormat("yyyy.MM.dd hh:mm:ss.zzz");
	end->setDisplayFormat("yyyy.MM.dd hh:mm:ss.zzz");


	QHBoxLayout *HLayout0 = new QHBoxLayout(this);
	HLayout0->setAlignment(Qt::AlignTop);


	QLabel *textLabel0 = new QLabel("T begin");
	QLabel *textLabel1 = new QLabel("T end");
	QDialogButtonBox *GroupBox1 = new QDialogButtonBox(Qt::Vertical);

	HLayout0->addWidget(textLabel0);
	HLayout0->addWidget(beg);
	HLayout0->addWidget(textLabel1);
	HLayout0->addWidget(end);

	HLayout0->setAlignment(textLabel0, Qt::AlignTop);
	HLayout0->setAlignment(beg, Qt::AlignTop);
	HLayout0->setAlignment(textLabel1, Qt::AlignTop);
	HLayout0->setAlignment(end, Qt::AlignTop);


	HLayout0->addWidget(GroupBox1);

	QPushButton *ok_button     = new QPushButton("Ok", this);
	QPushButton *cancel_button = new QPushButton("Cancel", this);
	connect(ok_button, SIGNAL(released()), this, SLOT(on_ok_button()));
	connect(cancel_button, SIGNAL(released()), this, SLOT(on_cancel_button()));

	GroupBox1->addButton(ok_button, QDialogButtonBox::ActionRole);
	GroupBox1->addButton(cancel_button, QDialogButtonBox::ActionRole);

	resize(minimumSize ());


	}

SetStartTimeDlg::~SetStartTimeDlg(void)
	{
	}
void SetStartTimeDlg::on_ok_button()
	{	
	done(1);
	}
void SetStartTimeDlg::on_cancel_button()
	{	
	done(0);
	}
void SetStartTimeDlg::setBegTime(double t1)
	{
	QDateTime tt;
	tt.setTimeSpec(Qt::UTC);
	tt.setTime_t ( (uint) t1 );
	beg->setDateTime(tt);
	}
void SetStartTimeDlg::setEndTime(double t1)
	{
	QDateTime tt;
	tt.setTimeSpec(Qt::UTC);
	tt.setTime_t ( (uint) t1 );
	end->setDateTime(tt);
	}
double SetStartTimeDlg::getBegTime()
	{
	QDateTime tt;

	tt.setTimeSpec(Qt::UTC);
	tt = beg->dateTime();
	return tt.toTime_t();
	}
double SetStartTimeDlg::getEndTime()
	{
	QDateTime tt;
	tt.setTimeSpec(Qt::UTC);
	tt = end->dateTime();
	return tt.toTime_t();
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
 * $Log: SetStartTimeDlg.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
