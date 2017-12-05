#pragma ident "$Id: SelectStaChanDlg.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "SelectStaChanDlg.h"
#include "StaInfo.h"
#include <QHeaderView>
#include <QGridLayout>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDialogButtonBox>

SelectStaChanDlg::SelectStaChanDlg(QWidget* parent = 0, Qt::WFlags fl = 0 ):QDialog(parent, fl)
	{
	this->setWindowTitle("Select channels for visualization");
	ActiveSta=""; 
	InitStaTable();
	InitChanTable();
	InitLayouts();
	LoadStaList(); 

	}

SelectStaChanDlg::~SelectStaChanDlg(void)
	{

	}


void SelectStaChanDlg::InitStaTable()
	{
	QRect qrc, tblrc;
	StaTable = new QTableWidget;

	StaTable->setRowCount( 0 );
    StaTable->setColumnCount( 1 ); 


	QStringList  labels;
	labels << "Station";
	StaTable->setHorizontalHeaderLabels(labels);
//	qtw->show();
	StaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	StaTable->setSelectionMode(QAbstractItemView::SingleSelection);
	StaTable->verticalHeader()->hide();

	GetWidgetTextRectangle(StaTable, "Station", qrc);
	
	tblrc = StaTable->rect();
	StaTable->setColumnWidth ( 0, qrc.width()+8);
//
	StaTable->setMaximumWidth( qrc.width()*2);
	StaTable->setMinimumWidth( qrc.width()*2);

	tblrc = StaTable->rect();
	StaTable->setColumnWidth ( 0, tblrc.width()-4);

	connect(StaTable, SIGNAL(  currentCellChanged ( int , int , int , int  ) ), this,  SLOT(OnCurrentCellChanged(int, int, int, int))  );

	}
void SelectStaChanDlg::OnCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
	{
	QString s;
	s = StaTable->item(currentRow, 0)->text();
	LoadChanList(s);
	}
void SelectStaChanDlg::InitChanTable()
	{
	int wdth0;
	QRect qrc;

	ChanTable = new QTableWidget;


//	connect(ChanTable, SIGNAL(itemChanged ( QTableWidgetItem *  )), this, SLOT(itemChanged(QTableWidgetItem *)));
	connect(ChanTable, SIGNAL(  cellClicked ( int , int ) ), this,  SLOT(OnClickChanListTableItem(int, int))  );
	ChanTable->setRowCount( 0 );
    ChanTable->setColumnCount( 2 ); 


	QStringList  labels;
	labels << "Channel" << "SpS";
	ChanTable->setHorizontalHeaderLabels(labels);
	ChanTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ChanTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ChanTable->verticalHeader()->hide();

	GetWidgetTextRectangle(ChanTable, "Channel", qrc);
	ChanTable->setColumnWidth ( 0, qrc.width()*2);
	wdth0 = qrc.width()*2;

	GetWidgetTextRectangle(ChanTable, "XXX.XXX", qrc);
	ChanTable->setColumnWidth ( 1, qrc.width()*2);

	wdth0 += qrc.width()*2;

	ChanTable->setMaximumWidth( wdth0+4);
	ChanTable->setMinimumWidth( wdth0+4);


	TableRowHeight = qrc.height()+2;
	}
void SelectStaChanDlg::InitLayouts()
	{
	QHBoxLayout *HLayout0 = new QHBoxLayout( this);
	QVBoxLayout *VLayout0 = new QVBoxLayout();
	QHBoxLayout *HLayout1 = new QHBoxLayout();



	QGroupBox *GroupBox0 = new QGroupBox();
	QDialogButtonBox *GroupBox1 = new QDialogButtonBox(Qt::Vertical);

	HLayout0->addWidget(GroupBox0);
	HLayout0->addWidget(GroupBox1);
	GroupBox0->setLayout(HLayout1);

	
	HLayout1->addWidget(StaTable);
	HLayout1->addWidget(ChanTable);

	QPushButton *ok_button     = new QPushButton("Ok", this);
	QPushButton *selectall_button     = new QPushButton("Select all", this);
	QPushButton *unselectall_button     = new QPushButton("UnSelect all", this);
	QPushButton *cancel_button = new QPushButton("Cancel", this);

	connect(ok_button, SIGNAL(released()), this, SLOT(on_ok_button()));
	connect(cancel_button, SIGNAL(released()), this, SLOT(on_cancel_button()));
	connect(selectall_button, SIGNAL(released()), this, SLOT(on_selectall_button()));
	connect(unselectall_button, SIGNAL(released()), this, SLOT(on_unselectall_button()));



	GroupBox1->addButton(ok_button, QDialogButtonBox::ActionRole);
	GroupBox1->addButton(selectall_button, QDialogButtonBox::ActionRole);
	GroupBox1->addButton(unselectall_button, QDialogButtonBox::ActionRole);
	GroupBox1->addButton(cancel_button, QDialogButtonBox::ActionRole);



	VLayout0->setSpacing(2);

	

	}
void SelectStaChanDlg::SetStaInfo(CStaInfo &Info)
	{
	StaInfo = Info;
	LoadStaList();
	}
void SelectStaChanDlg::GetStaInfo(CStaInfo &Info)
	{
//	Info = StaInfo;
	int nChan;
	int nSta = Info.AllSta.size();
	int i, j;

	for( i =0 ; i<nSta; ++i)
		{
		int nChan = Info.AllSta[i]->ChanInfo.size();
		for( j =0 ; j<nChan; ++j)
			{
			Info.AllSta[i]->ChanInfo[j]->bSelected = StaInfo.AllSta[i]->ChanInfo[j]->bSelected;
			}

		}

	}
void SelectStaChanDlg::GetWidgetTextRectangle(QWidget *pw, const char *p, QRect &qrc)
	{
	QFontMetrics fm = pw->fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}
void SelectStaChanDlg::LoadStaList()
	{
	char cSta[16];

//	m_sta.DeleteAllItems();
	while(StaTable->rowCount()>0) StaTable->removeRow(0);
	int nSta=StaInfo.AllSta.size();
	for(int i=0; i<nSta; ++i)
		{
		StaTable->insertRow(i);
		StaTable->setRowHeight(i, TableRowHeight);
		QString s = StaInfo.AllSta[i]->Sta;
		StaTable->setItem(i,   0, new QTableWidgetItem(s));
		StaTable->item(i,   0)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled);

		bool bAnySelected = IsAnyChanSelected(i);
		SetStationState( i, bAnySelected);

		if(i==0)
			{
			LoadChanList(s);
			}
		}
	}

void SelectStaChanDlg::LoadChanList(QString &sSta)
	{
	char cChan[16],cSpS[16];

	while(ChanTable->rowCount()>0) ChanTable->removeRow(0);
	int nSta=StaInfo.AllSta.size();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.compare(StaInfo.AllSta[i]->Sta, Qt::CaseInsensitive)==0)
			{
			int nChan=StaInfo.AllSta[i]->ChanInfo.size();
			for(int j=0; j<nChan; ++j)
				{
				sprintf(cSpS,"%.3f",StaInfo.AllSta[i]->ChanInfo[j]->dSpS);

				ChanTable->insertRow(j);
				ChanTable->setRowHeight(j, TableRowHeight);
				ChanTable->setItem(j,   0, new QTableWidgetItem(StaInfo.AllSta[i]->ChanInfo[j]->Chan));
				ChanTable->setItem(j,   1, new QTableWidgetItem(cSpS));
				ChanTable->item(j,   0)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
				ChanTable->item(j,   1)->setFlags( Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	
				if(StaInfo.AllSta[i]->ChanInfo[j]->bSelected)
					{
						ChanTable->item(j, 0)->setCheckState(Qt::Checked);
					}
				else
					{
						ChanTable->item(j, 0)->setCheckState(Qt::Unchecked);
					}

				}
			return;
			}
		}
	}
 void SelectStaChanDlg::on_ok_button()
	{	
	done(1);
	}
 void SelectStaChanDlg::on_cancel_button()
	{	
	done(0);
	}
 void SelectStaChanDlg::on_selectall_button()
	{
	int nStaRow = StaTable->currentItem()->row();
	SelUnselAllChan(nStaRow, true);
	}
void SelectStaChanDlg::on_unselectall_button()
	{
	int nStaRow = StaTable->currentItem()->row();
	SelUnselAllChan(nStaRow, false);
	}
void SelectStaChanDlg::SelUnselAllChan(int nStaRow, bool bSel)
	{
	int i;
	QString Sta = StaTable->item(nStaRow, 0)->text();
	int nSta=StaInfo.AllSta.size();
	for(i=0; i<nSta; ++i)
		{
		if(Sta.compare(StaInfo.AllSta[i]->Sta, Qt::CaseInsensitive)==0)
			{
			int nChan=StaInfo.AllSta[i]->ChanInfo.size();
			for(int j=0; j<nChan; ++j)
				{
				StaInfo.AllSta[i]->ChanInfo[j]->bSelected = bSel;
				}
			break;
			}
		}
	SetStationState(nStaRow, bSel);

	int nch = ChanTable->rowCount();
	Qt::CheckState state;
	if(bSel) state = Qt::Checked; else state = Qt::Unchecked;

	for(i=0; i<nch; ++i)
		{
		ChanTable->item(i, 0)->setCheckState(state);
		}

	}

void SelectStaChanDlg::SetStationState(int nRow, bool bAnySelected)
	{
	QFont f;
	if(bAnySelected)
		{
		f = StaTable->item(nRow, 0)->font();
		f.setBold(true);
		}
	else
		{
		f = StaTable->item(nRow, 0)->font();
		f.setBold(false);
		}
	StaTable->item(nRow, 0)->setFont(f);
	}

void SelectStaChanDlg::itemChanged ( QTableWidgetItem * item ) 
	{
	}
 void SelectStaChanDlg::OnClickChanListTableItem(int row, int col)
	{
	QTableWidgetItem *item = (QTableWidgetItem *)ChanTable->item(row, 0);
	QString Sta   = StaTable->currentItem()->text();
	QString Chan  = ChanTable->item(row, 0)->text();
	if(item->checkState()==Qt::Checked)
		{
		SetChanState(Sta, Chan, TRUE);
		}
	else
		{
		SetChanState(Sta, Chan, FALSE);
		}
	} 

 void SelectStaChanDlg::SetChanState(const QString &sSta, const QString &sChan, bool bSelected)
	{
	int nSta=StaInfo.AllSta.size();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.compare(StaInfo.AllSta[i]->Sta, Qt::CaseInsensitive)==0)
			{
			int nChan=StaInfo.AllSta[i]->ChanInfo.size();
			for(int j=0; j<nChan; ++j)
				{
				if(sChan.compare( StaInfo.AllSta[i]->ChanInfo[j]->Chan,Qt::CaseInsensitive)==0 )
					{
					StaInfo.AllSta[i]->ChanInfo[j]->bSelected = bSelected;
					break;
					}
				}
			SetStationState(sSta);
			break;
			}
		}

	} 

bool SelectStaChanDlg::IsAnyChanSelected(const QString &s)
	{
	int i;
	int nStations=StaInfo.AllSta.size();

	for(i=0; i<nStations; ++i)
		{
		if(s.compare(StaInfo.AllSta[i]->Sta, Qt::CaseInsensitive)==0)
			{
			return IsAnyChanSelected(i);
			}
		}
	return false;
	}
bool SelectStaChanDlg::IsAnyChanSelected(int nSta)
	{
	int j;
	int nChan=StaInfo.AllSta[nSta]->ChanInfo.size();
	for(j=0; j<nChan; ++j)
		{
		if(StaInfo.AllSta[nSta]->ChanInfo[j]->bSelected)
			{
			return true;
			}
		}
	return false;
	}

 void SelectStaChanDlg::SetStationState(const QString &s)
	{
	QFont f;
	int k;
	bool bAnySelected = false;
	int nSta=StaInfo.AllSta.size();

	bAnySelected = IsAnyChanSelected(s);


	for(k=0; k<nSta; ++k)
		{
		QString sname = StaTable->item(k, 0)->text();
		if(sname.compare(s, Qt::CaseInsensitive)==0)
			{
			if(bAnySelected)
				{
				f = StaTable->item(k, 0)->font();
				f.setBold(true);
				}
			else
				{
				f = StaTable->item(k, 0)->font();
				f.setBold(false);
				}
			StaTable->item(k, 0)->setFont(f);
			break;
			}
		}

	
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
 * $Log: SelectStaChanDlg.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
