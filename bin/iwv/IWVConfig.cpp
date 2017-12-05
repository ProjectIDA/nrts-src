#pragma ident "$Id: IWVConfig.cpp,v 1.1 2010/05/11 18:24:59 akimov Exp $"
#include <QDir>
#include "IWVConfig.h"
#include <QGroupBox>
#include "QWaveformsDisplay.h"
#include <QApplication>



int IWVConfig::display_width      = 500;
int IWVConfig::display_height     = 500; 
int IWVConfig::m_MaxWindow        = 9;
QString IWVConfig::FontName       = "Arial";
int IWVConfig::nFontSize          = 8;

IWVConfig::IWVConfig(QWidget* parent, Qt::WFlags fl):QDialog(parent, fl)
	{
	this->setWindowTitle("IWV Config");
	QHBoxLayout *HLayout0 = new QHBoxLayout(this);

	QGridLayout *GLayout0 = new QGridLayout();
	QGroupBox   *MainGroupBox = new QGroupBox("");
	QDialogButtonBox *GroupBox1 = new QDialogButtonBox(Qt::Vertical);

	HLayout0->addLayout(GLayout0);

	MaxTraces = new QLineEdit;
	QLabel    *label0     = new QLabel("Traces per window:");
	GLayout0->addWidget(label0,0,0);
	GLayout0->addWidget(MaxTraces,0,1);
	MaxTraces->setInputMask("99");

	MaxTraces->setMinimumWidth(100);

	QString s;
	s.sprintf("%2d", m_MaxWindow);

	MaxTraces->setText(s);

	displaywidth = new QLineEdit;
	QLabel    *label1     = new QLabel("Initial display width:");
	GLayout0->addWidget(label1,1,0);
	GLayout0->addWidget(displaywidth,1,1);
	displaywidth->setInputMask("9999");
	s.sprintf("%04d", display_width);
	displaywidth->setText(s);

	displayheight = new QLineEdit;
	QLabel    *label2     = new QLabel("Initial display height:");
	GLayout0->addWidget(label2,2,0);
	GLayout0->addWidget(displayheight,2,1);
	displayheight->setInputMask("9999");
	s.sprintf("%04d", display_height);
	displayheight->setText(s);


	font = new QLineEdit;
	QLabel    *label3     = new QLabel("Font:");
	GLayout0->addWidget(label3,3,0);
	GLayout0->addWidget(font,3,1);
	font->setText(FontName);


	fontsize = new QLineEdit;
	QLabel    *label4     = new QLabel("Font size:");
	GLayout0->addWidget(label4,4,0);
	GLayout0->addWidget(fontsize,4,1);
	fontsize->setInputMask("99");
	s.sprintf("%2d",nFontSize);
	fontsize->setText(s);



	HLayout0->addWidget(GroupBox1);


	QPushButton *ok_button     = new QPushButton("Ok", this);
	QPushButton *cancel_button = new QPushButton("Cancel", this);
	connect(ok_button, SIGNAL(released()), this, SLOT(on_ok_button()));
	connect(cancel_button, SIGNAL(released()), this, SLOT(on_cancel_button()));

	GroupBox1->addButton(ok_button, QDialogButtonBox::ActionRole);
	GroupBox1->addButton(cancel_button, QDialogButtonBox::ActionRole);

	resize(minimumSize ());

	}

IWVConfig::~IWVConfig(void)
	{
	}
bool IWVConfig::CheckValues()
	{
	QString s = MaxTraces->text();
	int nV = s.toInt();
	if( !(nV>0 && nV<100) ) 
		{
		MaxTraces->setFocus();
		return false;
		}

	s = fontsize->text();
	nV = s.toInt();
	if( !(nV>5 && nV<25) ) 
		{
		fontsize->setFocus();
		return false;
		}

	s = displayheight->text();
	nV = s.toInt();
	if( !(nV>100 && nV<9999) ) 
		{
		displayheight->setFocus();
		return false;
		}

	s = displaywidth->text();
	nV = s.toInt();
	if( !(nV>100 && nV<9999) )
		{
		displaywidth->setFocus();
		return false;
		}

	s = font->text();
	s = s.trimmed();
	if(s.length()<3)
		{
		font->setFocus();
		return false;
		}

	return true;
	}
void IWVConfig::on_ok_button()
	{
	if(!CheckValues()) 
		{
		QApplication::beep();
		return;
		}

	QString s = MaxTraces->text();
	m_MaxWindow = s.toInt();

	s = fontsize->text();
	IWVConfig::nFontSize = s.toInt();
	
	s = displayheight->text();
	display_height = s.toInt();

	s = displaywidth->text();
	display_width = s.toInt();

	s = font->text();
	FontName = s;


	Saveiwvrc();
	done(QDialog::Accepted);

	}
void IWVConfig::on_cancel_button()
	{	
	done(QDialog::Rejected);
	}
void IWVConfig::Saveiwvrc()
	{
	QDir home_dir = QDir::homePath();
	QString sPath = home_dir.path();
	sPath += QString(QDir::separator());
	sPath += QString(".iwvrc");

	FILE *f = NULL;

	f = fopen((const char *)sPath.toAscii(),"wt");
	if(f==NULL) return;

	fprintf(f, ".iwv.display.width:     %d\n", display_width );
	fprintf(f, ".iwv.display.height:    %d\n", display_height);
	fprintf(f, ".iwv.display.font:      %s\n", FontName.toAscii().constData());
	fprintf(f, ".iwv.display.fontsize:  %d\n", nFontSize     );
	fprintf(f, ".iwv.display.maxwindow: %d\n", m_MaxWindow   );

	fclose(f);


	}
void IWVConfig::Loadiwvrc()
	{
	QString value;
	char Param[64];
	QStringList listParam, listValue;
	QDir home_dir = QDir::homePath();
	QString sPath = home_dir.path();
	sPath += QString(QDir::separator());
	sPath += QString(".iwvrc");
	QMap<QString, QString> map;


	FILE *f = NULL;

	f = fopen((const char *)sPath.toAscii(),"rt");
	if(f==NULL) return;

	while(1)
		{
		char cBuff[128];
		if(fgets(cBuff, 127, f)==NULL) break;

		QString sP, sV, sB;
		
		sB = QString(cBuff).trimmed();
		sP = sB.section(' ', 0, 0);
		sV = (sB.right(sB.length()-sP.length())).trimmed();

		map.insert(sP,sV);
		}
	fclose(f);

	sprintf(Param, ".iwv.display.width:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		display_width = value.toInt();
		}
	sprintf(Param, ".iwv.display.height:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		display_height = value.toInt();
		}
	strcpy(Param, ".iwv.display.font:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		FontName = value;
		}

	sprintf(Param, ".iwv.display.fontsize:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		nFontSize = value.toInt();
		}
	sprintf(Param, ".iwv.display.maxwindow:");
	value = map.value(Param);
	if(!value.isEmpty())
		{
		int nMaxW = value.toInt();
		if(nMaxW>0) m_MaxWindow = nMaxW;
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
 * $Log: IWVConfig.cpp,v $
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
