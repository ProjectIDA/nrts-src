#pragma "$Id: iwv.pro,v 1.1 2010/05/11 18:25:00 akimov Exp $"

CONFIG += qt  thread warn_off
TARGET		 = iwv


#  CONFIG(release, debug|release):message(Release build!) #will print
#  CONFIG(debug, debug|release):message(Debug build!) #no print

SOURCES		 = itsv.cpp
SOURCES		+= MainWindow.cpp
SOURCES		+= butwth.cpp
SOURCES		+= ChanInfo.cpp
SOURCES		+= Dataview_misc.cpp
SOURCES		+= gpl_filter_seg.cpp
SOURCES		+= hook.cpp
SOURCES		+= InfoWindow.cpp
SOURCES		+= itsvqtAbout.cpp
SOURCES		+= MWindow.cpp
SOURCES		+= QWaveformPreview.cpp
SOURCES		+= QWaveformsDisplay.cpp
SOURCES		+= Screen.cpp
SOURCES		+= ScreenElem.cpp
SOURCES		+= SelectStaChanDlg.cpp
SOURCES		+= SelectWfdiscDlg.cpp
SOURCES		+= Sta.cpp
SOURCES		+= StaInfo.cpp
SOURCES		+= WfdiscRecord.cpp
SOURCES		+= WfdiscRecordsArray.cpp
SOURCES		+= WaveformsPrint.cpp
SOURCES		+= SetStartTimeDlg.cpp
SOURCES		+= SelectPDFfileDlg.cpp
SOURCES		+= SimpleTimeRuler.cpp
SOURCES		+= CheckWfdisc.cpp
SOURCES		+= IWVConfig.cpp


HEADERS		=  MainWindow.h
HEADERS		+= ChanInfo.h
HEADERS		+= filter.h
HEADERS		+= InfoWindow.h
HEADERS		+= itsvqtAbout.h
HEADERS		+= MWindow.h
HEADERS		+= QWaveformPreview.h
HEADERS		+= QWaveformsDisplay.h
HEADERS		+= Screen.h
HEADERS		+= ScreenElem.h
HEADERS		+= SelectStaChanDlg.h
HEADERS		+= SelectWfdiscDlg.h
HEADERS		+= Sta.h
HEADERS		+= StaInfo.h
HEADERS		+= stock.h
HEADERS		+= WfdiscRecord.h
HEADERS		+= WfdiscRecordsArray.h
HEADERS		+= WaveformsPrint.h
HEADERS		+= SetStartTimeDlg.h
HEADERS		+= SelectPDFfileDlg.h
HEADERS		+= SimpleTimeRuler.h
HEADERS		+= IWVConfig.h

unix	{
LIBS    = -L ../../../lib/$(PLATFORM)
LIBS   += -lstdc++
LIBS   += -lcssio
LIBS   += -lutil
LIBS   += -lz
LIBS   += -lm
LIBS   += $(MTLIBS)
LIBS   += $(POSIX4LIB)

DEFINES += $(OSNAME) 
DEFINES += $(PLATFORM)
DEFINES += $(MACHTYPE)
DEFINES += $(OSTYPE)
INCLUDEPATH = ../../include
QMAKE_CFLAGS += -W
QMAKE_CXXFLAGS += -W
}

win32 {


CONFIG(debug, debug|release) {  
	LIBS +=   ..\..\windows\lib\util\Debug\util.lib ..\..\windows\lib\cssio\Debug\cssio.lib ..\..\windows\lib\win32\Debug\win32.lib   
	}

CONFIG(release, debug|release) { CONFIG += staticlib
  LIBS +=   ..\..\windows\lib\util\Release\util.lib ..\..\windows\lib\cssio\Release\cssio.lib ..\..\windows\lib\win32\Release\win32.lib  
  }

INCLUDEPATH = ..\..\include ..\..\..\include
DEFINES += _CRT_SECURE_NO_DEPRECATE
 }

# +-----------------------------------------------------------------------+
# |                                                                       |
# | Copyright (C) 2010 Regents of the University of California            |
# |                                                                       |
# | This software is provided 'as-is', without any express or implied     |
# | warranty.  In no event will the authors be held liable for any        |
# | damages arising from the use of this software.                        |
# |                                                                       |
# | Permission is granted to anyone to use this software for any purpose, |
# | including commercial applications, and to alter it and redistribute   |
# | it freely, subject to the following restrictions:                     |
# |                                                                       |
# | 1. The origin of this software must not be misrepresented; you must   |
# |    not claim that you wrote the original software. If you use this    |
# |    software in a product, an acknowledgment in the product            |
# |    documentation of the contribution by Project IDA, UCSD would be    |
# |    appreciated but is not required.                                   |
# | 2. Altered source versions must be plainly marked as such, and must   |
# |    not be misrepresented as being the original software.              |
# | 3. This notice may not be removed or altered from any source          |
# |    distribution.                                                      |
# |                                                                       |
# +-----------------------------------------------------------------------+
