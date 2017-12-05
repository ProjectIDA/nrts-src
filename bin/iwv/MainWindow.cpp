#pragma ident "$Id: MainWindow.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#include "MainWindow.h"
#include <QMenuBar>
#include <QLayout>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextEdit>
#include <QApplication>
#include <QRect>
#include <QStatusBar>
#include <QScrollArea>
#include <QScrollBar>
#include "SetStartTimeDlg.h"
#include "WaveformsPrint.h"
#include "IWVConfig.h"

#include "filter.h"
#include "SelectWfdiscDlg.h"
#include "itsvqtAbout.h"


static const unsigned char icon0_data[]  = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 
0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 
0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 
0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 
0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC, 
0x61, 0x05, 0x00, 0x00, 0x00, 0x20, 0x63, 0x48, 0x52, 0x4D, 
0x00, 0x00, 0x7A, 0x26, 0x00, 0x00, 0x80, 0x84, 0x00, 0x00, 
0xFA, 0x00, 0x00, 0x00, 0x80, 0xE8, 0x00, 0x00, 0x75, 0x30, 
0x00, 0x00, 0xEA, 0x60, 0x00, 0x00, 0x3A, 0x98, 0x00, 0x00, 
0x17, 0x70, 0x9C, 0xBA, 0x51, 0x3C, 0x00, 0x00, 0x00, 0xA4, 
0x49, 0x44, 0x41, 0x54, 0x38, 0x4F, 0xA5, 0x93, 0x51, 0x12, 
0x80, 0x20, 0x08, 0x44, 0xED, 0xE6, 0x1E, 0xCD, 0x9B, 0x91, 
0x90, 0xAB, 0x80, 0x52, 0xC3, 0xD4, 0x0C, 0x1F, 0x21, 0xFB, 
0x40, 0xC0, 0xAB, 0xB5, 0x46, 0xE5, 0xCF, 0xC7, 0x80, 0x93, 
0xD5, 0x5A, 0x19, 0x6C, 0x8C, 0x7D, 0x3E, 0xB6, 0x9C, 0xC4, 
0x10, 0x6E, 0xC1, 0x03, 0xA8, 0xFD, 0x1B, 0x80, 0xC5, 0x3E, 
0x53, 0x21, 0x22, 0x18, 0x2A, 0x03, 0xC4, 0x00, 0xB6, 0xC3, 
0xAE, 0x32, 0xD9, 0x00, 0x52, 0x49, 0x0C, 0x80, 0xB3, 0x4F, 
0x72, 0x17, 0xF7, 0x5F, 0x31, 0x0F, 0x19, 0x8D, 0x17, 0x3F, 
0x6B, 0x66, 0x20, 0x00, 0x5A, 0xF8, 0x09, 0x30, 0x74, 0x69, 
0xD2, 0x21, 0xAB, 0xAA, 0x62, 0x25, 0x1C, 0xB1, 0x1E, 0x20, 
0x65, 0xF5, 0xBB, 0x5A, 0xBF, 0xAE, 0xF2, 0x39, 0x9B, 0xD5, 
0xEA, 0x40, 0xDD, 0xC4, 0x08, 0x02, 0x31, 0x26, 0x15, 0x8E, 
0x11, 0x63, 0xF3, 0xBB, 0xF0, 0x3A, 0x46, 0xBF, 0x79, 0xA9, 
0x45, 0xD2, 0x62, 0x16, 0xA6, 0x56, 0xD9, 0x8B, 0xA3, 0xF7, 
0x11, 0xAC, 0xFD, 0x7A, 0x30, 0x19, 0xE1, 0x5A, 0xB8, 0xC3, 
0x03, 0xC9, 0x80, 0x6E, 0xE4, 0x30, 0x81, 0xC3, 0xAB, 0xBB, 
0xD8, 0x21, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 
0xAE, 0x42, 0x60, 0x82 };


static const unsigned char icon1_data[]  = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 
0x00, 0x00, 0x00, 0x20, 0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 
0x7A, 0x7A, 0xF4, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 
0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 
0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC, 
0x61, 0x05, 0x00, 0x00, 0x00, 0x20, 0x63, 0x48, 0x52, 0x4D, 
0x00, 0x00, 0x7A, 0x26, 0x00, 0x00, 0x80, 0x84, 0x00, 0x00, 
0xFA, 0x00, 0x00, 0x00, 0x80, 0xE8, 0x00, 0x00, 0x75, 0x30, 
0x00, 0x00, 0xEA, 0x60, 0x00, 0x00, 0x3A, 0x98, 0x00, 0x00, 
0x17, 0x70, 0x9C, 0xBA, 0x51, 0x3C, 0x00, 0x00, 0x01, 0x9D, 
0x49, 0x44, 0x41, 0x54, 0x58, 0x47, 0xED, 0x57, 0x5B, 0x12, 
0x83, 0x20, 0x0C, 0xC4, 0x9B, 0x7B, 0x34, 0x6F, 0x66, 0x79, 
0x2D, 0x2C, 0x31, 0x89, 0xF8, 0xE8, 0x4F, 0xA7, 0xCE, 0x30, 
0xB5, 0x88, 0xEC, 0xB2, 0x24, 0x4B, 0x5C, 0x42, 0x08, 0x7B, 
0x6C, 0xF9, 0xDA, 0xF7, 0x76, 0x1B, 0x96, 0x65, 0x41, 0xF7, 
0x77, 0xFB, 0x23, 0x68, 0x42, 0xCD, 0x0D, 0x17, 0xFF, 0x97, 
0xCF, 0xB4, 0xF1, 0xDA, 0x7B, 0x72, 0x9C, 0x39, 0xCF, 0x9F, 
0xC0, 0x5F, 0x81, 0x27, 0x0A, 0x84, 0xB0, 0xC6, 0xE0, 0x95, 
0xED, 0x72, 0x30, 0x97, 0x0C, 0xB0, 0x32, 0x81, 0xB3, 0xA3, 
0x93, 0x2D, 0xA0, 0xDB, 0xB6, 0x1D, 0x1A, 0x08, 0xC9, 0xCC, 
0xD0, 0xE7, 0x89, 0xB8, 0x2D, 0xF7, 0x26, 0x6E, 0x30, 0x39, 
0x80, 0xB5, 0x57, 0xF0, 0x4C, 0x12, 0xB1, 0xA6, 0xCF, 0xFE, 
0xE3, 0xE5, 0x3A, 0x98, 0xF3, 0x8A, 0x27, 0xB8, 0x36, 0x65, 
0x0A, 0x91, 0xD1, 0x67, 0x78, 0xDB, 0xB3, 0x02, 0xDA, 0x80, 
0x61, 0x50, 0x95, 0x1B, 0xC0, 0xEB, 0xBA, 0xEE, 0xA9, 0xCD, 
0x5C, 0x49, 0x11, 0x90, 0x50, 0x8D, 0xEE, 0x2A, 0x01, 0x09, 
0x4C, 0x06, 0xAA, 0xF2, 0x79, 0x81, 0x40, 0x09, 0xB6, 0x74, 
0x69, 0xAB, 0x3E, 0x23, 0x90, 0xDE, 0x03, 0x89, 0x9B, 0x0A, 
0x74, 0xA9, 0xCF, 0x08, 0x80, 0x8C, 0x24, 0xF5, 0x80, 0x40, 
0x5F, 0xBD, 0xA6, 0x6F, 0x02, 0x62, 0x50, 0x8B, 0x80, 0x54, 
0x61, 0x38, 0xEC, 0xFC, 0x18, 0xB0, 0x09, 0x00, 0xDC, 0xFB, 
0x65, 0xD2, 0xAC, 0x82, 0x38, 0x6D, 0x3D, 0x23, 0xEA, 0x04, 
0x66, 0x00, 0xAD, 0x31, 0x5E, 0x1C, 0xB8, 0x46, 0x84, 0xDC, 
0xF7, 0xE4, 0xBF, 0xA3, 0x00, 0xCF, 0x37, 0xF8, 0xC0, 0xD1, 
0x2E, 0xFD, 0x18, 0x28, 0xDB, 0x57, 0xA6, 0x93, 0xF1, 0x20, 
0xB3, 0x46, 0x06, 0x22, 0x6D, 0x7D, 0x77, 0x29, 0xD5, 0xB1, 
0xA2, 0xE1, 0x20, 0x0D, 0x2D, 0x25, 0x98, 0x08, 0x8F, 0x41, 
0xD6, 0xB0, 0x17, 0x74, 0x67, 0x6D, 0x5B, 0xFF, 0x8C, 0x80, 
0xE5, 0x86, 0xEC, 0x96, 0x8F, 0x09, 0x84, 0x49, 0xDB, 0xD5, 
0xAC, 0xDA, 0x89, 0x7E, 0x1C, 0x01, 0xB6, 0x02, 0x09, 0x18, 
0xCD, 0xDB, 0x06, 0x09, 0xCC, 0x86, 0x75, 0x9B, 0x00, 0x83, 
0x33, 0x89, 0x19, 0x22, 0x48, 0x3B, 0xD7, 0x01, 0xFB, 0x29, 
0x7C, 0x54, 0x40, 0x03, 0xCF, 0x7D, 0xB5, 0xFA, 0xB9, 0x52, 
0x0F, 0x58, 0x25, 0x7E, 0x2B, 0x03, 0xD8, 0x09, 0x73, 0x27, 
0xC9, 0x3E, 0xDC, 0x47, 0xF0, 0x7E, 0x98, 0x9C, 0x57, 0x44, 
0xDA, 0x77, 0x86, 0x5A, 0x15, 0x0D, 0xA6, 0xA0, 0x81, 0xD7, 
0x55, 0xEB, 0x29, 0x78, 0xAC, 0x09, 0xAD, 0xAC, 0xB0, 0xFA, 
0xBB, 0x11, 0x39, 0xE0, 0x66, 0x3D, 0x27, 0x8E, 0xBD, 0xD9, 
0xEA, 0x4A, 0x1C, 0xCB, 0xD5, 0x10, 0x24, 0x81, 0x5A, 0x4A, 
0xC9, 0xC3, 0xEA, 0xAD, 0xCF, 0x36, 0x22, 0x41, 0x04, 0x86, 
0x12, 0x5B, 0x4F, 0xCF, 0xAF, 0x11, 0x98, 0x5D, 0xE9, 0xCF, 
0x11, 0xF8, 0x00, 0xF7, 0x2B, 0xB2, 0x36, 0x26, 0x34, 0x29, 
0xB6, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 
0x42, 0x60, 0x82 };



MainWindow::MainWindow( QWidget * parent, Qt::WFlags f):QMainWindow(parent, f ) 
	{
	QPixmap img0, img1;
	QIcon  ico;

	twin = 60.;

	this->setWindowTitle("Interactive Waveform Viewer");


	img0.loadFromData( icon0_data, sizeof( icon0_data ), "PNG" ); 
	img1.loadFromData( icon1_data, sizeof( icon1_data ), "PNG" );

	ico.addPixmap(img0, QIcon::Normal, QIcon::On);
	ico.addPixmap(img1, QIcon::Normal, QIcon::Off);

	setWindowIcon( ico);
	menuBar();
	statusBar();


	LoadDBpickrc();

	QMenu * pMenu;
//	tap->addAction(
	pMenu = menuBar()->addMenu("File");
	pMenu->addAction( "&Open...", this, SLOT(file_open()));
	pMenu->addAction( "&Print", this, SLOT(file_print()));
	pMenu->addAction( "&Exit", this, SLOT(close()), Qt::Key_Escape);

		


	pMenu = menuBar()->addMenu("Traces");
//	help =menuBar()->insertItem( "&Help", help ); 
//	help->addAction( "&About", this, about(), Qt::Key_F1 );
	pMenu->addAction( "Choose", this, SLOT(traces_choose())); 
    pMenu->addAction( "Select", this, SLOT(traces_select()) );
	pMenu->addAction( "Delete", this, SLOT(traces_delete()) );
	pMenu->addAction( "Original", this, SLOT(traces_original()) );
	pMenu->addAction( "Select All", this, SLOT(traces_select_all()) );
	pMenu->addAction( "Delete All", this, SLOT(traces_delete_all()) );


	pAmpMenu =menuBar()->addMenu( "AMP:" ); 
    a_ampfixed = pAmpMenu->addAction( "Fixed", this, SLOT(amp_on_ampfixed()) );
	a_ampfixed->setCheckable(true);

	a_auto = pAmpMenu->addAction( "Auto", this, SLOT(amp_on_auto()) );
	a_auto->setCheckable(true);
	a_auto->setChecked(true);

	a_auto0 =pAmpMenu->addAction( "Auto0", this, SLOT(amp_on_auto0()) );
	a_auto0->setCheckable(true);

	a_auto1 =pAmpMenu->addAction( "Auto1", this, SLOT(amp_on_auto1()) );
	a_auto1->setCheckable(true);

	a_autoA =pAmpMenu->addAction( "AutoA", this, SLOT(amp_on_autoA()) );
	a_autoA->setCheckable(true);

	a_cliponoff =pMenu->addAction( "Clip off", this, SLOT(traces_cliponoff()) );
	a_invert =pMenu->addAction( "Invert", this, SLOT(traces_invert()) );
	a_gain1 =pMenu->addAction( "Gain=1", this, SLOT(traces_gain1()) );

	pFilterMenu =menuBar()->addMenu( "Filters" ); 

	LoadFilterMenu(pFilterMenu);

	pMenu =menuBar()->addMenu( "Commands" ); 

	QMenu * pMenu1 = pMenu->addMenu("Fit");

    pMenu1->addAction( "Horizontal", this, SLOT(commands_fit()));
	pMenu1->addAction( "Vertical", this, SLOT(commands_fit_vertical()));

    pMenu->addAction( "Set time", this, SLOT(commands_settime()) );
	pMenu->addAction( "Go to end", this, SLOT(commands_toend()) );
	pMenu->addAction( "Go to begin", this, SLOT(commands_tobegin()) );
	pMenu->addAction( "Config", this, SLOT(commands_config()) );



	pMenu =menuBar()->addMenu( "Help" );
    pMenu->addAction( "About...", this, SLOT(help_about()) );





	scrollArea = new QScrollArea(this); 
	scrollArea->setFrameStyle(QFrame::NoFrame);

//	QLayout * vLayout = layout(); 

//	vLayout->addWidget(scrollArea);      

	m_TimeRuler = new SimpleTimeRuler(this, Qt::Widget);
	m_Display = new QWaveformsDisplay(m_TimeRuler,this, this, Qt::Widget);
	m_Display->show();
	m_Display->SetTWIN(twin);

	scrollArea->setWidget(m_Display);
	
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	m_Preview = new QWaveformPreview(m_Display, this, Qt::Widget);
	m_Preview->show();



	connect(this, SIGNAL(openWFDISC()), this, SLOT(file_open()));

	}

MainWindow::~MainWindow()
	{

	}

void MainWindow::StartOpen()
	{
	emit openWFDISC();
	}

void MainWindow::closeEvent(QCloseEvent *event)
	{
//	pMainFrame->SaveParameters();
	event->accept();
	}

void MainWindow::file_open()
	{
//	pMainFrame->QueryConfig();
	SelectWfdiscDlg dialog(this,0);

	QStringList fileNames;

	if (dialog.exec())
     fileNames = dialog.selectedFiles();
	if(fileNames.size()>0)
		{
		QDir dir = dialog.directory();
		QString s = dir.absolutePath();

		QDir::setCurrent(s);


		if(dir.cd(s))
			{
			dialog.LastDir = s;
			m_Display->LoadWfdiscFileInfo(fileNames[0]);
			m_Display->FillWaveformInfo();
			m_Display->FindMinMaxTimeInWaveformInfo();
			m_Display->SetStartTime();
			m_Display->RunSelectDlg();

			}
		else
			{
			QMessageBox::warning(this, "directory does not exist","");
			}
		}
	}

void MainWindow::traces_choose()
	{
//	QSize qsz = size();
//	AdjustWindowsSize(qsz);
	m_Display->RunSelectDlg(); 
	}

void MainWindow::resizeEvent ( QResizeEvent * event ) 
	{
	QSize qsz = event->size();
	AdjustWindowsSize(qsz);
	m_Display->DrawWaveforms();
	return;

//	QSize qsz = event->size();

	int xMW = width(); 
	int yMW = height();

	QRect rc = statusBar()->geometry();
	int yMSb = rc.height();

	rc = menuBar()->geometry();
	int yMmenu = rc.height()+1;
	
//	printf("\n %d %d", yMSb, yMmenu);

	m_Preview->setGeometry(0, yMW-40-yMSb, xMW, 40);


//	m_Display->setGeometry(0, 0, xMW, yMW-35-yMSb-1-yMmenu);

	scrollArea->setGeometry(0, yMmenu, xMW, yMW-80-yMSb-2-yMmenu);
	QSize scrollAreaSize     = scrollArea->size();

	scrollArea->setWidgetResizable(true);
	scrollArea->setWidgetResizable(false);


//	m_Display->setGeometry(0, 0, scrollAreaSize.width(), scrollAreaSize.height());

	QRect DisplayGeometry = m_Display->frameGeometry();
//	QSize scrollAreaSize     = scrollArea->size();
	m_TimeRuler->setGeometry(0, yMW-40-yMSb-40-1, DisplayGeometry.width(), 40);

	m_Display->AdjustWidgetSize();
	m_Display->DrawWaveforms();
	

	}

void MainWindow::amp_on_ampfixed() 
	{

	a_ampfixed->setChecked(true);
	a_gain1->setChecked(false);
	a_invert->setChecked(false);
	a_cliponoff->setChecked(false);
	a_autoA->setChecked(false);
	a_auto1->setChecked(false);
	a_auto0->setChecked(false);
	a_auto->setChecked(false);
	m_Display->SetAmpFlagFixed();

	}

void MainWindow::amp_on_auto()
	{
	a_ampfixed->setChecked(false);
	a_gain1->setChecked(false);
	a_invert->setChecked(false);
	a_cliponoff->setChecked(false);
	a_autoA->setChecked(false);
	a_auto1->setChecked(false);
	a_auto0->setChecked(false);
	a_auto->setChecked(true);
	m_Display->SetAmpFlagAuto();

	}

void MainWindow::amp_on_auto0()
	{
	a_ampfixed->setChecked(false);
	a_gain1->setChecked(false);
	a_invert->setChecked(false);
	a_cliponoff->setChecked(false);
	a_autoA->setChecked(false);
	a_auto1->setChecked(false);
	a_auto0->setChecked(true);
	a_auto->setChecked(false);
	m_Display->SetAmpFlagAuto0();

	}

void MainWindow::amp_on_auto1()
	{
	a_ampfixed->setChecked(false);
	a_gain1->setChecked(false);
	a_invert->setChecked(false);
	a_cliponoff->setChecked(false);
	a_autoA->setChecked(false);
	a_auto1->setChecked(true);
	a_auto0->setChecked(false);
	a_auto->setChecked(false);
	m_Display->SetAmpFlagAuto();

	}

void MainWindow::amp_on_autoA()
	{
	a_ampfixed->setChecked(false);
	a_gain1->setChecked(false);
	a_invert->setChecked(false);
	a_cliponoff->setChecked(false);
	a_autoA->setChecked(true);
	a_auto1->setChecked(false);
	a_auto0->setChecked(false);
	a_auto->setChecked(false);
	m_Display->SetAmpFlagFixed();

	}

void MainWindow::traces_select() 
	{
	// TODO: Add your command handler code here
	m_Display->SelectTraces();
	}

void MainWindow::traces_original()
	{
	m_Display->Original();
	}

void MainWindow::traces_select_all()
	{
	m_Display->SelectAll();
	}

void MainWindow::traces_delete_all() 
	{
	m_Display->DeleteAll();
	}

void MainWindow::traces_cliponoff()
	{
	bool bClip=m_Display->ClipOnOff();

	if(bClip)
		{
		a_cliponoff->setText("Clip Off");
		}
	else
		{
		a_cliponoff->setText("Clip On");
		}

	}

void MainWindow::traces_delete() 
	{
	m_Display->DeleteTraces();
	}

void MainWindow::traces_invert() 
	{
	m_Display->FlipInvertFlag();
	}

void MainWindow::traces_gain1() 
	{
	m_Display->ChangeGain(1.,0);
	}

void MainWindow::LoadFilterMenu(QMenu * pMenu)
	{
	QAction *a_filt; 
	QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(filter_selected(QAction *)));

	a_filt = grp->addAction("None");
	a_filt->setCheckable(true);
	a_filt->setChecked(true);

	for(int i=0; i<QWaveformsDisplay::Filters.size(); ++i)
		{
		QAction *a_filt = grp->addAction(QWaveformsDisplay::Filters[i].label);
		a_filt->setCheckable(true);
		filter_menus.push_back(a_filt);
		}
	pMenu->addActions(grp->actions());
	}

void MainWindow::filter_selected(QAction *pAA) 
	{
	int nActiveFilter = -1;

	for(int i=0; i<QWaveformsDisplay::Filters.size(); ++i)
		{
		if(pAA == filter_menus[i])
			{
			nActiveFilter = i;
			pAA->setChecked(true);
			m_Display->SetFilterNumber(i); 
			return;
			}
		else
			{
//			pAA->setChecked(true);
			}
		}
	m_Display->SetFilterNumber(-1);

	}

void MainWindow::commands_fit() 
	{
	m_Display->Fit();
	}
void MainWindow::commands_fit_vertical() 
	{
	QSize scrollAreaSize   = scrollArea->size();
	QRect qrc = m_Display->geometry();
	qrc.setHeight(scrollAreaSize.height());
	m_Display->setGeometry(qrc);
	m_Display->FitVertical();
	}

void MainWindow::commands_settime()
	{
	SetStartTimeDlg dlg(this, 0);
	dlg.setBegTime(m_Display->getBegTime());
	dlg.setEndTime(m_Display->getEndTime());
	if(dlg.exec()==1)
		{
		m_Display->setBegTime(dlg.getBegTime());
		m_Display->setEndTime(dlg.getEndTime());
		QSize qsz = size();
		AdjustWindowsSize(qsz);
		m_Display->DrawWaveforms();
		}
	}

void MainWindow::help_about()
	{
	itsvqtAbout dlg(this, 0);
	dlg.exec();

	}


void MainWindow::file_print()
	{
	WaveformsPrint print(m_Display);
	print.Print();
	}

void MainWindow::commands_tobegin()
	{
	m_Display->toBegin();
	}

void MainWindow::commands_toend()
	{
	m_Display->toEnd();
	}

bool MainWindow::setWfdisc(const char *pName)
	{
	QString fileName = QString(pName);
	QString sPath;
	QDir qDir;
	
	if(!fileName.endsWith(".wfdisc", Qt::CaseInsensitive)) fileName = fileName + ".wfdisc";
	if(!QFile::exists(fileName)) 
		{
		QString error = fileName;
		error += QString(": no such file\n");
		printf(error.toAscii().constData());
		return false;
		}

	
	QFileInfo fi(fileName);
	qDir = fi.absoluteDir();
	sPath = qDir.path();
	QDir::setCurrent(sPath);


	m_Display->LoadWfdiscFileInfo(fileName);
	m_Display->FillWaveformInfo();
	m_Display->FindMinMaxTimeInWaveformInfo();
	m_Display->SetStartTime();
	QSize qsz = size();
	AdjustWindowsSize(qsz);
	m_Display->DrawWaveforms();
	UpdatePreviewWindow();
	return true;
	}

void MainWindow::commands_config()
	{
	IWVConfig dlg(this);
	if(dlg.exec()==QDialog::Accepted)
		{
		AdjustWindowsSize();
		m_Display->AdjustWidgetSize();
		m_Display->DrawWaveforms();
		}
	}


void MainWindow::LoadDBpickrc()
	{
	QString value;
	char Param[64];
	QStringList listParam, listValue;
	QDir home_dir = QDir::homePath();
	QString sPath = home_dir.path();
	sPath += QString(QDir::separator());
	sPath += QString(".dbpickrc");
	QMap<QString, QString> map;
	struct gather_plot_filter_ gpf;


	FILE *f = NULL;

//	rcf.setFileName(sPath);
//	if(!rcf.open(QIODevice::ReadOnly)) return;

	f = fopen((const char *)sPath.toAscii(),"rt");
	if(f==NULL) return;

	while(1)
		{
		char cBuff[128];
		if(fgets(cBuff, 127, f)==NULL) break;

		QString sP, sV, sB;
		
		sB = QString(cBuff).trimmed();
		sB.replace('\t', ' ');
		sP = sB.section(' ',0,0);
		sV = (sB.right(sB.length()-sP.length())).trimmed();

		map.insert(sP,sV);
		}
	fclose(f);

	sprintf(Param, "dbpick.traceWindow.twin:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		twin = value.toFloat();
		}

	for(int i=1; i<64; ++i)
		{
		sprintf(Param, "dbpick.filter%d.label:", i);
		value = map.value(Param);
		if(value.isEmpty()) break;
		strcpy(gpf.label, (const char *)value.toAscii());
		sprintf(Param, "dbpick.filter%d.tpad:", i);
		value = map.value(Param);

		if(value.isEmpty()) break;
		gpf.start_pad = value.toDouble();
		sprintf(Param, "dbpick.filter%d.lcoFreq:", i);
		value = map.value(Param);
		if(value.isEmpty()) break;
		gpf.param1 = value.toFloat();
		sprintf(Param, "dbpick.filter%d.lcoOrder:", i);
		value = map.value(Param);
		if(value.isEmpty()) break;
		gpf.param2 = value.toFloat();
		sprintf(Param, "dbpick.filter%d.ucoFreq:", i);
		value = map.value(Param);
		if(value.isEmpty()) break;
		gpf.param3 = value.toFloat();
		sprintf(Param, "dbpick.filter%d.ucoOrder:", i);
		value = map.value(Param);
		if(value.isEmpty()) break;
		gpf.param4 = value.toFloat();
		gpf.end_pad = 0.0;
		gpf.type = GPL_BUTTERWORTH;
		QWaveformsDisplay::Filters.push_back(gpf);
		}



	}
void MainWindow::SetStatusBarText(const char *pText)
	{
	statusBar()->showMessage ( pText, 5000);
	}
void MainWindow::UpdatePreviewWindow()
	{
	m_Preview->DrawPreviewBitmap();
	m_Preview->update();
	}

void MainWindow::AdjustWindowsSize()
	{
	QSize qsz = size();
	AdjustWindowsSize(qsz);
	}
void MainWindow::AdjustWindowsSize(QSize &qsz)
	{



	int xMW = width(); 
	int yMW = height();

	QRect rc = statusBar()->geometry();
	int yMSb = rc.height();

	rc = menuBar()->geometry();
	int yMmenu = rc.height()+1;

	m_Preview->setGeometry(0, yMW-40-yMSb, xMW, 40);

	scrollArea->setGeometry(0, yMmenu, xMW, yMW-80-yMSb-2-yMmenu);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidgetResizable(false);



	int nWin=m_Display->pScreen->GetMaxWindow();

	if(nWin==0)	nWin=1;

	int MaxY1, MaxY = yMW-40-yMSb-40-yMmenu;
	int nW;

	if(nWin<IWVConfig::m_MaxWindow) 
		nW = nWin; 
	else 
		nW = IWVConfig::m_MaxWindow;

	int yHeight=(MaxY)/nW;

	MaxY1 = yHeight*nW;

	QRect qrc = m_Display->geometry();
	qrc.setHeight(yHeight*nWin);

	m_Display->setGeometry(qrc);



	int MaxYScrollArea = MaxY1;
	QRect scrollAreaGeometry = scrollArea->frameGeometry();
	QSize scrollAreaSize     = scrollArea->size();
	scrollAreaSize.setHeight(MaxYScrollArea);
	scrollArea->setGeometry(0, yMmenu, xMW, MaxYScrollArea/*+abs(scrollAreaSize.height()-scrollAreaGeometry.height())*/);

	QScrollBar *scroll = scrollArea->verticalScrollBar();
	int nMin = scroll->minimum();
	int nMax = scroll->maximum();
	scroll->setPageStep(yHeight);
	scroll->setSingleStep(yHeight);

	scrollAreaGeometry = scrollArea->frameGeometry();
	QRect m_DisplayGeometry  = m_Display->frameGeometry();
	m_TimeRuler->setGeometry(0, scrollAreaGeometry.bottom()+1, m_DisplayGeometry.width(), 40);
//	m_TimeRuler->hide();

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
 * $Log: MainWindow.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
