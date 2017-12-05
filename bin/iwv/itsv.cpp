#pragma ident "$Id: itsv.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include <QApplication>
#include <QMainWindow> 
#include <QMenuBar>
#include <QLineEdit> 
#include <QSettings>
#include <QDir>
#include <QMap>
#include "platform.h"
#include "MainWindow.h"
#include "IWVConfig.h"



bool CheckWfdisc(const char *pName);


int main(int argc,char *argv[])
	{

	QApplication a( argc, argv );




	IWVConfig::Loadiwvrc();

	QFont f(IWVConfig::FontName, IWVConfig::nFontSize);
	a.setFont (f);

	if(argc>1)
		{
		if(!CheckWfdisc(argv[1]))
			{
			return -1;
			}
		}


	MainWindow dlg;
    dlg.show();
	dlg.resize( QSize(IWVConfig::display_width, IWVConfig::display_height).expandedTo(dlg.minimumSizeHint()) );
	a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );


	if(argc>1) 
		dlg.setWfdisc(argv[1]);
	else
		{
		dlg.StartOpen();
		}

	a.exec();

	return 0;
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
 * $Log: itsv.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
