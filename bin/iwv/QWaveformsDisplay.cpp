#pragma ident "$Id: QWaveformsDisplay.cpp,v 1.2 2010/06/07 18:32:51 akimov Exp $"

#include "QWaveformsDisplay.h"
#include "IWVConfig.h"
#include <QMessageBox>
#include <QApplication>
#include <QPainterPath>
#include <QMainWindow>
#include <QStatusBar>
#include <stdio.h>
#include "filter.h"
#include "SelectStaChanDlg.h"
#include "util.h"
#include <algorithm>

void _close_filter();
void _do_filter(float *data);
void _init_filter(float *data);
int _open_filter(double  dt, struct gather_plot_filter_ *filter);



typedef struct 
	{
	double t;
	double a;
	bool flag;
	}
	OneCount;



std::vector<struct gather_plot_filter_>QWaveformsDisplay::Filters;


QWaveformsDisplay::QWaveformsDisplay(SimpleTimeRuler *pTR, MainWindowInterface *mwi, QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
	{
	pTimeRuler = pTR;
	MainWindow = mwi;
	nAmpFlag=0;
	nActiveScreen=-1;
	t1=t2=tB1=tB2=-1;
	bBox=false;
	bBoxExpandX=false;
	bBoxContractX=false;
	bBoxExpandY=false;
	bBoxContractY=false;
	bMiddleButtonDrag=false;
	pScreen=NULL;
	pScreen=new CScreen();
	bClip=false;
	bPanelSelectionActive=false;
	DisplayMode=mAuto;
	bInvert=false;
	dGain=1.;
	mbuttonx=0;
	mbuttony=0;
	xX1=xX2=0;
	nFilterType=-1;
	setMouseTracking(true);
	bSelectionActive = false;

	pTimeRuler->SetInfoWidth(WindowInfoXsize);
	twin = 600;
	}

QWaveformsDisplay::~QWaveformsDisplay(void)
	{
	}

void QWaveformsDisplay::paintEvent( QPaintEvent * event )
	{
	unsigned int i;
	event->accept();


	QPainter painter;
	ScreenBitmapCriticalSection.lock();
	painter.begin(this);

	if(!leftPannel.isNull())
		{
		painter.drawPixmap(0,0,leftPannel);
		painter.setBackgroundMode(Qt::TransparentMode);

		int nMaxWin=pScreen->GetMaxWindow();
		for(int i=0; i<nMaxWin; ++i)
			{
			if(pScreen->IsSelected(i))	DrawFocused(i, &painter);
			}

		}

	if(bMiddleButtonDrag)
		{
		DrawDraggedBitmap(painter);
		}
	else
		{
		if(!Bitmap.isNull())
			{
//			painter.drawPixmap(WindowInfoXsize, 0, Bitmap);
			painter.drawImage(WindowInfoXsize, 0, Bitmap);
			if(bSelectionActive)
				{
//				DrawInverted(painter);
				if(!selectedReg.isNull())
					{
//					painter.drawPixmap(InvRect.x(), InvRect.y(), selectedReg);
					painter.drawImage(InvRect.x(), InvRect.y(), selectedReg);
					}
				}
			}
		}


	QPen dotPen(QColor(255,255,255), 1, Qt::DotLine);
	painter.setPen(dotPen);

	for(i=0; i<pTimeRuler->tmmarks.size(); ++i)
		{
		int x=pTimeRuler->tmmarks[i];
		painter.drawLine(x - WindowInfoXsize,MaxY,x - WindowInfoXsize,1);
		}
	painter.end();
	ScreenBitmapCriticalSection.unlock();



	}
void QWaveformsDisplay::DrawDraggedBitmap(QPainter &painter)
	{
	int delta=mouse_x-mbuttonx;
	int width  = Bitmap.width();
	int height = Bitmap.height();


	if(delta>0)
		{
		painter.fillRect(WindowInfoXsize, 1, MaxX-WindowInfoXsize, height, QColor(128, 128, 128));
//		painter.drawPixmap(WindowInfoXsize+delta, 0, width-delta, height,Bitmap, 0, 0,width-delta, height);
		painter.drawImage(WindowInfoXsize+delta, 0, Bitmap, 0, 0,width-delta, height);
		}
	else
		{
		painter.fillRect(WindowInfoXsize+width+delta, 1, -delta, height, QColor(128, 128, 128));
//		painter.drawPixmap(WindowInfoXsize,0, width+delta, height, Bitmap, -delta, 0, width+delta, height);
		painter.drawImage(WindowInfoXsize,0, Bitmap, -delta, 0, width+delta, height);
		}
	}

void QWaveformsDisplay::DrawInverted(QPainter &painter)
	{
	QImage Bitmap1(InvRect.width(), InvRect.height(), QImage::Format_RGB32);
	QPainter pntr;

	Bitmap1.invertPixels(QImage::InvertRgb);
	painter.drawImage(InvRect.x(), 0, Bitmap1);
	}

void QWaveformsDisplay::LoadWfdiscFileInfo(QString &FileName)
	{
	char line[WFDISC_SIZE+2];
	wfdisc wfd;
	wfdisc *pwfdisc;
	pwfdisc=&wfd;
	FILE *fp=fopen((const char *)FileName.toLocal8Bit(),"rt");

	nActiveScreen=-1;
	t1=t2=tB1=tB2=-1;

	AllStaChan.Clear();
	ClearWaveformInfo();
	wfdarray.Clear();

	if(fp==NULL)
		{
		QMessageBox::critical ( this, "IWV", "Wfdisc file open error", QMessageBox::Close, QMessageBox::Close );
		return;
		}

	while(1)
		{
		if (fgets(line, WFDISC_SIZE+1, fp) == NULL) break;
		if (strlen(line) == WFDISC_SIZE) 
			{
			CWfdiscRecordsArray::ScanWfdLine(pwfdisc, line);
            WFDISC_TRM(pwfdisc);

			AllStaChan.AddInfo(*pwfdisc);
			wfdarray.Add(*pwfdisc);
			}
		}
	fclose(fp);
	WfdFileName=FileName;
	} 

void QWaveformsDisplay::ClearWaveformInfo()
	{
	while(WaveformInfo.size()>0)
		{
		CMWindow *wfi=WaveformInfo[0];
		WaveformInfo.erase(WaveformInfo.begin());
		delete wfi;
		}
	} 

void QWaveformsDisplay::RunSelectDlg()
	{
	SelectStaChanDlg SelDlg(this, 0);

	SelDlg.SetStaInfo(AllStaChan);
	if(SelDlg.exec())
		{
		SelDlg.GetStaInfo(AllStaChan);
//		FillWaveformInfo();
//		FindMinMaxTimeInWaveformInfo();
//		SetStartTime();
		MakeScreenInfoFromWaveformInfo();
		MainWindow->AdjustWindowsSize();
		DrawWaveforms();
		RedrawPreview();
		}
	} 

/*void QWaveformsDisplay::FillWaveformInfo()
	{
	unsigned int i,j,k,l;
	QString Sta,Chan;
	double dSpS;
	for(i=0; i<AllStaChan.AllSta.size();++i)
		{
		Sta=AllStaChan.AllSta[i]->Sta;
		for(j=0; j<AllStaChan.AllSta[i]->ChanInfo.size(); ++j)
			{
			Chan=AllStaChan.AllSta[i]->ChanInfo[j]->Chan;
			dSpS=AllStaChan.AllSta[i]->ChanInfo[j]->dSpS;
			if(AllStaChan.AllSta[i]->ChanInfo[j]->bSelected)
				{
				bool bFound=FALSE;
				for(l=0; l<WaveformInfo.size(); ++l)
					{
					if( (Sta.compare(WaveformInfo[l]->StaName, Qt::CaseInsensitive)==0) && (Chan.compare(WaveformInfo[l]->ChanName, Qt::CaseInsensitive)==0))
						{
						bFound=TRUE;
						break;
						}
					}

				if(!bFound)
					{
					CMWindow *mw=new CMWindow(Sta, Chan, dSpS);
					WaveformInfo.push_back(mw);
					for(k=0; k<wfdarray.GetSize(); ++k)
						{
						if( (Sta.compare(wfdarray[k]->wfd.sta, Qt::CaseInsensitive)==0) && (Chan.compare(wfdarray[k]->wfd.chan, Qt::CaseInsensitive)==0))
							{
							mw->AddWfdRecord(wfdarray[k]->wfd);
							}
						}
//					mw->AutoScale(t1,t2);
					}
				}
			else
				{
				for(l=0; l<WaveformInfo.size(); ++l)
					{
					if( (Sta.compare(WaveformInfo[l]->StaName, Qt::CaseInsensitive)==0) && (Chan.compare(WaveformInfo[l]->ChanName, Qt::CaseInsensitive)==0))
						{
						CMWindow *wfi=WaveformInfo[l];
						WaveformInfo.erase(WaveformInfo.begin()+l);
						delete wfi;
						}

					}
				}
			}
		}

	pScreen->Clear();
	for(l=0; l<WaveformInfo.size(); ++l)
		{
		pScreen->AddElement(l);
		}

	} */



void QWaveformsDisplay::FillWaveformInfo()
	{
	unsigned int i,j,k,l;
	QString Sta,Chan;
	double dSpS;
	for(i=0; i<AllStaChan.AllSta.size();++i)
		{
		Sta=AllStaChan.AllSta[i]->Sta;
		for(j=0; j<AllStaChan.AllSta[i]->ChanInfo.size(); ++j)
			{
			Chan = AllStaChan.AllSta[i]->ChanInfo[j]->Chan;
			CMWindow *mw=new CMWindow(AllStaChan.AllSta[i], AllStaChan.AllSta[i]->ChanInfo[j]);
			WaveformInfo.push_back(mw);
			for(k=0; k<wfdarray.GetSize(); ++k)
					{
					if( (Sta.compare(wfdarray[k]->wfd.sta, Qt::CaseInsensitive)==0) && (Chan.compare(wfdarray[k]->wfd.chan, Qt::CaseInsensitive)==0))
						{
						mw->AddWfdRecord(wfdarray[k]->wfd);
						}
					}
				
			}
		}

	pScreen->Clear();
	for(l=0; l<WaveformInfo.size(); ++l)
		{
		pScreen->AddElement(l);
		}

	} 



void QWaveformsDisplay::RedrawPreview()
	{
	MainWindow->UpdatePreviewWindow();
	} 



void QWaveformsDisplay::DrawWaveforms()
	{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));



	ApplyScale();

//	AdjustWidgetSize();


    QBrush BlueBrush(QColor(0,0,128));
	
	QRect rc;

	QSize qsz = size();

	rc.setRect(0,0, qsz.width()-WindowInfoXsize, qsz.height());


	MaxX= qsz.width()-1;
	MaxY= qsz.height();


	xcursor=-1;
	ycursor=-1;

	QImage Bitmap1(qsz.width()-WindowInfoXsize-1,  MaxY, QImage::Format_RGB32);
	QPainter painter;

	painter.begin(&Bitmap1);


	painter.fillRect(rc, BlueBrush); 

	if(CalculateMWindowsSize()==0)
		{

		painter.end();

		ScreenBitmapCriticalSection.lock();
//		Bitmap = QPixmap::fromImage(Bitmap1);
		Bitmap = Bitmap1;
		ScreenBitmapCriticalSection.unlock();

		pTimeRuler->SetTime(t1,t2);
		pTimeRuler->DrawTimePannel();

		update();
		QApplication::restoreOverrideCursor();
		return;
		}

	DrawLeftPannel();
	
	pTimeRuler->SetTime(t1,t2);
	pTimeRuler->DrawTimePannel();


    QBrush WhiteBrush(QColor(255,255,255));
    QBrush GrayBrush(QColor(192,192,192));

    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));
    QPen BluePen(QColor(0,0,255));
    QPen YellowPen(QColor(0,255,255));

	painter.setBackgroundMode(Qt::TransparentMode);


	painter.fillRect(rc, BlueBrush);


	painter.setPen(DarkPen);
	painter.drawLine(0, 0, 0, MaxY);

	painter.end();

	ScreenBitmapCriticalSection.lock();
//	Bitmap = QPixmap::fromImage(Bitmap1);
	Bitmap = Bitmap1;
	ScreenBitmapCriticalSection.unlock();

	update();

	int nMaxWin=pScreen->GetMaxWindow();
	for(int i=0; i<nMaxWin ; ++i)
		{

		int nWf=pScreen->GetWaveformN(i);

		int xw1=WaveformInfo[nWf]->xw1;
		int xw2=WaveformInfo[nWf]->xw2;
		int yw1=WaveformInfo[nWf]->yw1;
		int yw2=WaveformInfo[nWf]->yw2;

		WaveformInfo[nWf]->x1=WindowInfoXsize;
		WaveformInfo[nWf]->x2=qsz.width()-1;
		int y1=WaveformInfo[nWf]->y1;
		int y2=WaveformInfo[nWf]->y2;

		int x1=0;
		int x2=qsz.width()-1-WindowInfoXsize;


		QRect rc(xw1,MaxY-yw2, (xw2-xw1), (yw2-yw1));

		ScreenBitmapCriticalSection.lock();


		painter.begin(&Bitmap);
		DrawWindow(i, &painter, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
//*******************************************************
		painter.end();
		ScreenBitmapCriticalSection.unlock();

		update();

		}



/********************************************************************/
	QApplication::restoreOverrideCursor();
	}
void QWaveformsDisplay::GetTextRectangle(QPainter *painter, const char *p, QRect &qrc)
	{
	QFontMetrics fm = painter->fontMetrics();
	int textWidthInPixels = fm.width(p);
	int textHeightInPixels = fm.height();
	qrc.setRect(0, 0, textWidthInPixels, textHeightInPixels);
	}
void QWaveformsDisplay::DrawTimeMark(int x, double tt, QPainter *painter)
	{
	int yw1=1;

	char cBuff1[64], cBuff2[64];


	strcpy(cBuff1, util_dttostr(tt, 15));
	QRect rcc1,rcc2;
	GetTextRectangle(painter, cBuff1,rcc1);

	rcc1.moveTo( x - rcc1.height()/2, MaxY - (yw1+rcc1.height()));

	strcpy(cBuff2, util_dttostr(tt, 4));
	GetTextRectangle(painter, cBuff2, rcc2);


	rcc2.moveTo( x - rcc2.height()/2, MaxY - yw1);

	if( nLastMarkRightPos<min(rcc2.x(),rcc1.x()))
		{
		painter->drawText( rcc1, Qt::AlignBottom, cBuff1 );
		painter->drawText( rcc2, Qt::AlignBottom, cBuff2 );
		tmmarks.push_back(x);
		}

	}
typedef struct
	{
	int ymax, ymin;
	int xold, yold;
	bool bNeedSomethingToDraw;
	}
	WaveformSegment;

void QWaveformsDisplay::DrawWindow(int nWin, QPainter *painter, int x1, int x2, int y1, int y2,
   int xw1, int xw2, int yw1, int yw2, int MaxY, int MaxX, bool bPrint)
	{
	bool bNeedSomethingToDraw = false;
	bool bfirstcount=true, bfilterinit=false;
	int x_x=0, y_y=0;

	int xxold=-1, yymin, yymax;
	int nWf=pScreen->GetWaveformN(nWin);
	WaveformSegment WSeg;

	WSeg.bNeedSomethingToDraw = false;
	WSeg.xold = -1;

    QBrush WhiteBrush(QColor(255,255,255));
    QBrush GrayBrush(QColor(192,192,192));
    QBrush BlueBrush(QColor(0,0,128));

    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));
    QPen BluePen(QColor(0,0,255));
    QPen YellowPen(QColor(255,255,0));

	QPainterPath path;

	OneCount beforeFirst = {0.,0.,false}, afterLast;

	WSeg.bNeedSomethingToDraw = false;


	long amin=WaveformInfo[nWf]->amin;
	long amax=WaveformInfo[nWf]->amax;


	if(!bPrint)
	if(!bClip)
		{
		QRect rc(xw1, yw1, xw2-xw1, yw2-yw1);
		painter->fillRect(rc, BlueBrush);
		}


	int nDataBlocks=WaveformInfo[nWf]->wfdrec.size();
	double ttt=-1.;
	double dSpS=WaveformInfo[nWf]->dSpS;

	if(bPrint)
		{
		DarkPen.setWidth (1);
		DarkPen.setStyle(Qt::SolidLine);
		painter->setPen(DarkPen);
		}
	else
		painter->setPen(YellowPen);

	if(nFilterType>=0)
		{
		_open_filter((1./dSpS), &Filters[nFilterType]);
		}
	double t1extnd = t1 - 2./dSpS-1;
	double t2extnd = t2 + 2./dSpS+1;
	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=WaveformInfo[nWf]->wfdrec[j]->wfd.time;
		unsigned long nsamp=WaveformInfo[nWf]->wfdrec[j]->wfd.nsamp;
		double t2packet=t1packet+nsamp/dSpS;
		unsigned long nStartSamp,nCnts;
		double tbeg1,tend1;

		if( !( (t1extnd>=t1packet && t1extnd<=t2packet) || (t1extnd<t1packet && t2extnd>=t1packet)) ) continue;

		tbeg1=max(t1extnd,t1packet);
		tend1=min(t2extnd,t2packet);

		if( t1extnd>t1packet)
			{
			nStartSamp=(unsigned long)((t1extnd-t1packet)*dSpS);
			tbeg1=t1packet+nStartSamp/dSpS;
			}
		else
			{
			nStartSamp=0;
			tbeg1=t1packet;
			}

		
		if(t2extnd>t2packet)
			{
			nCnts=(unsigned long)((t2packet-tbeg1)*dSpS+0.5);
			}
		else
			{
			nCnts=(unsigned long)((t2extnd-tbeg1)*dSpS+0.5);
			}

		CWfdiscRecord *wfdr=WaveformInfo[nWf]->wfdrec[j];
		wfdr->OpenDataSet();
		double a;
		unsigned long nSamples=0;

		bool bContinue=true;
		while(bContinue)
			{
			if(nCnts==0)
				{
				break;
				}
			nStartSamp+=nSamples;
			nSamples=wfdr->LoadData(nStartSamp,nCnts);
			nCnts-=nSamples;
			if(nSamples==0) break;
			long *lData=wfdr->lData;


			for(unsigned long ii=0; ii<nSamples; ++ii,++lData)
				{
				a=*lData;
				if(bInvert) a=-a;

				if(nFilterType>=0)
					{
					float aaa=(float)a;
					if(!bfilterinit) {_init_filter(&aaa);bfilterinit=true;}
					_do_filter(&aaa);
					a=aaa;
					}

				double tt=t1packet+(1./dSpS)*(ii+nStartSamp);
				int x,y;


				if(tt>t2)
					{
					bContinue=false;

					if(WSeg.bNeedSomethingToDraw)
						{
//						painter->drawLine(x_x, y_y, xxold,yymin);
						painter->drawLine(WSeg.xold, WSeg.ymin, WSeg.xold, WSeg.ymax);
						WSeg.bNeedSomethingToDraw=false;

						double aa = Interpolate(beforeFirst.t, beforeFirst.a, tt, a, t2);
						int y0 =(int)((double)((aa-amin)*(y2-y1)/(amax-amin)+y1+0.5));
						y0 = y2+y1-y0;

						if(y0<y1)y0=y1;
						if(y0>y2)y0=y2;

						painter->drawLine(x2, y0, WSeg.xold, WSeg.yold);
						}



					bNeedSomethingToDraw = false;
					break;
					}


				if(tt<t1)
					{
					beforeFirst.t = tt;
					beforeFirst.a = a;
					beforeFirst.flag = true;
					continue;
					}

				x=(int)((tt-t1)*(x2-x1)/(t2-t1)+x1+0.5);



				if(!bClip)
					{
					if(a>amax) a=amax;
					if(a<amin) a=amin;
					}

				y=(int)((double)((a-amin)*(y2-y1)/(amax-amin)+y1+0.5));

				y = y2+y1-y ;
				if(bClip)
					{
					if(y<0)y=0;
					if(y>MaxY)y=MaxY;
					}


				if(bfirstcount)
					{
					bfirstcount=false;
					if(beforeFirst.flag)
						{
						double aa = Interpolate(beforeFirst.t, beforeFirst.a, tt, a, t1);
						int y0 =(int)((double)((aa-amin)*(y2-y1)/(amax-amin)+y1+0.5));
						y0 = y2+y1-y0;

						if(y0<y1)y0=y1;
						if(y0>y2)y0=y2;

						painter->drawLine(x1, y0, x, y);
						}
					}

				beforeFirst.t = tt;
				beforeFirst.a = a;


				if( (x==WSeg.xold) && WSeg.bNeedSomethingToDraw)
					{
					if(WSeg.ymax<y) WSeg.ymax=y;
					if(WSeg.ymin>y) WSeg.ymin=y;
					WSeg.yold = y;
					ttt=tt;
					continue;
					}
				else
					{
					if(WSeg.bNeedSomethingToDraw)
						{


						painter->drawLine(WSeg.xold,WSeg.ymin, WSeg.xold,WSeg.ymax);

						if( (tt-ttt)<1.2/dSpS)
						painter->drawLine(WSeg.xold,WSeg.yold, x, y);


						
						}

					}
					WSeg.xold = x;
					WSeg.ymax = WSeg.ymin = WSeg.yold = y;
					WSeg.bNeedSomethingToDraw = true;

					ttt=tt;
					}
			}
		wfdr->CloseDataSet();
		}
	_close_filter();
//	painter->drawPath(path);
	}


 void QWaveformsDisplay::ApplyScale()
	{
	switch(DisplayMode)
		{
		case mFx:
			break;
		case mAuto:
			ApplyAuto();
			break;
		case mAuto0:
			ApplyAuto0();
			break;
		case mAuto1:
			ApplyAuto1();
			break;
		case mAutoA:
			ApplyAutoA();
			break;
		}
	ApplyGain();
	} 

 void QWaveformsDisplay::SetStartTime()
	{
	t2=t1+twin;
	} 

 int QWaveformsDisplay::CalculateMWindowsSize()
	{
	QRect rc;
	int i;
	QSize qsz = size();

	MaxY= qsz.height()-1;
	MaxX= qsz.width()-1;
	int nWin=pScreen->GetMaxWindow();

	if(nWin==0)
		{
		nActiveScreen=-1;
		return nWin;
		}


	int yHeight=(MaxY+1)/nWin;


	xX1=WindowInfoXsize;
	xX2=MaxX;

	for(int j=nWin-1; j>=0; --j)
		{
		i=pScreen->GetWaveformN(j);

		WaveformInfo[i]->xp1=0;
		WaveformInfo[i]->xp2=WindowInfoXsize-1;
		WaveformInfo[i]->yp1=yHeight*j;
		WaveformInfo[i]->yp2=(yHeight*(j+1))-2;


		WaveformInfo[i]->xw1=WindowInfoXsize;
		WaveformInfo[i]->xw2=MaxX-1;
		WaveformInfo[i]->yw1=WaveformInfo[i]->yp1;
		WaveformInfo[i]->yw2=WaveformInfo[i]->yp2;

		WaveformInfo[i]->x1=0;
		WaveformInfo[i]->x2=MaxX-1-WindowInfoXsize;

		WaveformInfo[i]->y1=WaveformInfo[i]->yw1;
		WaveformInfo[i]->y2=WaveformInfo[i]->yw2;

		}
	if(nWin-1<nActiveScreen) nActiveScreen=0;


	return nWin;
	} 

void QWaveformsDisplay::FindMinMaxTimeInWaveformInfo()
	{
	unsigned int i,k;

	tB1=-1;
	tB2=-1;
	t1=-1;
	t2=-1;
	for(i=0; i<WaveformInfo.size(); ++i)
		{
		for(k=0; k<WaveformInfo[i]->wfdrec.size(); ++k)
			{
			double tt1=WaveformInfo[i]->wfdrec[k]->wfd.time;
			double tt2=WaveformInfo[i]->wfdrec[k]->wfd.endtime;

			if(tB1==-1)
				{
				tB1=tt1;
				}
			else
				{
				if(tB1>tt1)tB1=tt1;
				}
			if(tB2==-1)
				{
				tB2=tt2;
				}
			else
				{
				if(tB2<tt2) tB2=tt2;
				}
			}
		}

	tB1-=1;
	tB2+=1;

	if( (t1<tB1)||(t1>tB2 ))
		{
		t1=tB1;
		}
	
	if( (t2<tB1)||(t2>tB2 ))
		{
		t2=tB2;
		}

	} 

void QWaveformsDisplay::DrawLeftPannelWindow(int nScreen, QPainter *painter )
	{

	QPainterPath path;

	QRect rcText;
	char cText[64];

    static QBrush BlueBrush(QColor(0,0,192));
    static QBrush WhiteBrush(QColor(255,255,255));
    static QBrush GrayBrush(QColor(192,192,192));

    static QPen WhitePen(QColor(255,255,255));
	static QPen DarkPen(QColor(0,0,0));
    static QPen BluePen(QColor(0,0, 192));
    static QPen YellowPen(QColor(0,255,255));


	int nWf=pScreen->GetWaveformN(nScreen);

	int xw1=WaveformInfo[nWf]->xp1;
	int xw2=WaveformInfo[nWf]->xp2;
	int yw1=WaveformInfo[nWf]->yp1;
	int yw2=WaveformInfo[nWf]->yp2;

	int y1 = WaveformInfo[nWf]->y1; 
	int y2 = WaveformInfo[nWf]->y2;


	long amin=WaveformInfo[nWf]->amin;
	long amax=WaveformInfo[nWf]->amax;


	painter->setPen(BluePen);

	QRect rc(0, yw1, WindowInfoXsize, yw2-yw1);
	painter->fillRect(rc, BlueBrush);

	painter->setPen(DarkPen);

	QRect rc1(xw1, yw1, WindowInfoXsize, yw2-yw1+1);
	painter->fillRect(rc1, GrayBrush);


	if( abs(y2-y1)>1.2 * painter->fontMetrics().height())
		{
		path.moveTo(xw1+WindowInfoXsize-8,(y1+1));		
		path.lineTo(xw1+WindowInfoXsize-8,(y2-1));

		path.moveTo(xw1+WindowInfoXsize-8,(y1+1));
		path.lineTo(xw1+WindowInfoXsize-8+4,(y1+1));

		path.moveTo(xw1+WindowInfoXsize-8,(y2-1));
		path.lineTo(xw1+WindowInfoXsize-8+4,(y2-1));
		painter->drawPath(path);
		}


	painter->setPen(DarkPen);

	if( abs(y2-y1)>3.1 * painter->fontMetrics().height())
		{

		sprintf(cText,"%d",amax);
		QRect rcc1(xw1,(yw1+1), WindowInfoXsize-8-2, (yw2-yw1));
		painter->setBackgroundMode(Qt::TransparentMode);
		painter->drawText(rcc1, Qt::AlignRight | Qt::AlignTop, cText);

		sprintf(cText,"%d",amin);
		GetTextRectangle(painter, cText, rcText);

		QRect rcc2(xw1,yw1+1, WindowInfoXsize-8-2, (yw2-yw1));
		painter->drawText(rcc2, Qt::AlignRight | Qt::AlignBottom , cText);
		}




	QRect rcc3(xw1, yw1, WindowInfoXsize-8-2, (yw2-yw1));
	sprintf(cText,"%s::%s",(const char *)WaveformInfo[nWf]->StaName.toAscii(),(const char *) WaveformInfo[nWf]->ChanName.toAscii());
	painter->drawText(rcc3, Qt::TextSingleLine|Qt::AlignVCenter|Qt::AlignHCenter , cText);



	} 

void QWaveformsDisplay::DrawLeftPannel()
	{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QPainter painter;
	QImage Bitmap1(WindowInfoXsize, MaxY, QImage::Format_ARGB32_Premultiplied);
    QBrush BlueBrush(QColor(0,0,192));

	painter.begin(&Bitmap1);

/********************************************************************/

	painter.setBackgroundMode(Qt::TransparentMode);

	QRect rc(0,0,WindowInfoXsize,MaxY);
	painter.fillRect(rc, BlueBrush);

	int nMaxWin=pScreen->GetMaxWindow();
	for(int i=0; i<nMaxWin ; ++i)
		{
		DrawLeftPannelWindow(i,&painter);
		}

	painter.end();
	ScreenBitmapCriticalSection.lock();
	leftPannel = QPixmap::fromImage(Bitmap1);
	ScreenBitmapCriticalSection.unlock();

	QApplication::restoreOverrideCursor();
	} 


void QWaveformsDisplay::ApplyGain()
	{
	int nMaxWin=pScreen->GetMaxWindow();
	if(nMaxWin<1) return;
	long amax, amin, delta, mid;
	if(dGain==1.) return;
	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		amax=WaveformInfo[nWf]->amax_1;
		amin=WaveformInfo[nWf]->amin_1;
		mid=(long)((amax+amin)/2.+0.5);
		delta=(long)((amax-amin)/2.+0.5);

		delta=(long)(delta/dGain);

		amax=mid+delta;
		amin=mid-delta;

		WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin=amin;
		}
	}
void QWaveformsDisplay::ApplyAutoA()
	{
	long amax, amin, ampl=0;
	unsigned int i;
	unsigned int nMaxWin=pScreen->GetMaxWindow();

	for(i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		GetMinMaxForTrace(nWf, amax, amin);
		ampl=max(ampl,abs(amin));
		ampl=max(ampl,abs(amax));
		}
	for(i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		WaveformInfo[nWf]->amax=ampl;
		WaveformInfo[nWf]->amin=-ampl;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;
		}


	}
void QWaveformsDisplay::ApplyAuto0()
	{
	long amax, amin;
	int nMaxWin=pScreen->GetMaxWindow();

	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		GetMinMaxForTrace(nWf, amax, amin);

		long ampl=max(abs(amax),abs(amin));

		WaveformInfo[nWf]->amax=ampl;
		WaveformInfo[nWf]->amin=-ampl;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;

		}


	}
void QWaveformsDisplay::ApplyAuto1()
	{
	long amax, amin;
	unsigned int i;
	unsigned int nMaxWin=pScreen->GetMaxWindow();

	if(nMaxWin<1) return;
	int nWf=pScreen->GetWaveformN(0);
	GetMinMaxForTrace(nWf, amax, amin);

	for(i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin=amin;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;
		}


	}
void QWaveformsDisplay::ApplyAuto()
	{
	long amax, amin;
	unsigned int i;
	unsigned int nMaxWin=pScreen->GetMaxWindow();

	for(i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		GetMinMaxForTrace(nWf, amax, amin);
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin=amin;
		}


	}
void QWaveformsDisplay::GetMinMaxForTrace(int nWf, long &amax, long &amin)
	{

	double dSpS=WaveformInfo[nWf]->dSpS;
	unsigned long nDataBlocks=WaveformInfo[nWf]->wfdrec.size();
	bool bfirstcount=TRUE;
	amax=1;
	amin=-1;

	if(nFilterType>=0)
		{
		_open_filter((1./dSpS), &Filters[nFilterType]);
		}


	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=WaveformInfo[nWf]->wfdrec[j]->wfd.time;
		unsigned long nsamp=WaveformInfo[nWf]->wfdrec[j]->wfd.nsamp;
		double t2packet=t1packet+nsamp/dSpS;
		unsigned long nStartSamp,nCnts;
		double tbeg1,tend1;

		if( !( (t1>=t1packet && t1<=t2packet) || (t1<t1packet && t2>=t1packet)) ) continue;

		tbeg1=max(t1,t1packet);
		tend1=min(t2,t2packet);

		if( t1>t1packet)
			{
			nStartSamp=(unsigned long)((t1-t1packet)*dSpS);
			tbeg1=t1;
			}
		else
			{
			nStartSamp=0;
			tbeg1=t1packet;
			}

		
		if(t2>t2packet)
			{
			nCnts=(unsigned long)((t2packet-tbeg1)*dSpS);
			}
		else
			{
			nCnts=(unsigned long)((t2-tbeg1)*dSpS);
			}

		CWfdiscRecord *wfdr=WaveformInfo[nWf]->wfdrec[j];
		wfdr->OpenDataSet();
		long a;
		unsigned long nSamples=0;

		bool bContinue=true;
		while(bContinue)
			{
			if(nCnts==0)
				{
				break;
				}
			nStartSamp+=nSamples;
			nSamples=wfdr->LoadData(nStartSamp,nCnts);
			nCnts-=nSamples;
			if(nSamples==0) break;
			long *lData=wfdr->lData;


			for(unsigned long ii=0; ii<nSamples; ++ii,++lData)
				{
				a=*lData;
				if(bInvert) a=-a;
				double tt=t1packet+(1./dSpS)*(ii+nStartSamp);

				if(tt<t1) continue;
				if(tt>t2)
					{
					bContinue=false;
					break;
					}
				if(nFilterType>=0)
					{
					float aaa=(float)a;
					if(bfirstcount) _init_filter(&aaa);
					_do_filter(&aaa);
					a=aaa;
					}


				if(!bfirstcount)
					{
					if(a>amax) amax=a;
					if(a<amin) amin=a;
					}
				else
					{
					bfirstcount=FALSE;
					amax=a;
					amin=a;
					}

				}
			}
		wfdr->CloseDataSet();
		}
	if(amax==amin) 
		{
		amax=amax+1;
		amin=amin-1;
		}
	if(nFilterType>=0) _close_filter();
	}

void QWaveformsDisplay::DrawFocused(int nScr, QPainter *painter)
	{
	int nWfm=pScreen->GetWaveformN(nScr);
	int xw1=WaveformInfo[nWfm]->xp1+4;
	int xw2=WaveformInfo[nWfm]->xp1+WindowInfoXsize-4;
	int yw1=WaveformInfo[nWfm]->yp1+2;
	int yw2=WaveformInfo[nWfm]->yp2-2;

    static QPen   RedPen(QColor(255,0,0));
    static QPen   WhitePen(QColor(255,255,255));
    static QPen   GrayPen(QColor(128,128,128));
	static QBrush RedBrush(QColor(255,0,0));

	painter->setPen(GrayPen);
	painter->drawLine(xw1, yw2, xw1, yw1);		
	painter->drawLine(xw2, yw1, xw1, yw1);		



	painter->setPen(WhitePen);
	painter->drawLine(xw2, yw1, xw2,yw2);		
	painter->drawLine(xw1, yw2, xw2, yw2);		




	char cBuff[16];
	sprintf(cBuff, "%d", pScreen->GetSelectionNumber(nScr)+1);
	QRect rcc;
	GetTextRectangle(painter, cBuff, rcc);

	rcc.moveLeft(xw1+3);
	rcc.moveTop(yw1+3);

	painter->setPen(RedPen);
	painter->setBackgroundMode(Qt::TransparentMode);
	

	painter->drawText(rcc, Qt::AlignLeft|Qt::TextSingleLine,cBuff);


	}
void QWaveformsDisplay::mouseReleaseEvent ( QMouseEvent * pevent ) 
	{

		if(pevent->button()==Qt::LeftButton)
			{
			OnLButtonUp(pevent);
			return;
			}
		if(pevent->button()==Qt::RightButton)
			{
			OnRButtonUp(pevent);
			return;
			}
		if(pevent->button()==Qt::MidButton)
			{
			OnMButtonUp(pevent);
			return;
			}

	}
void QWaveformsDisplay::OnLButtonUp(QMouseEvent * pevent) 
	{

	bPanelSelectionActive=false;

	}

void QWaveformsDisplay::OnRButtonUp(QMouseEvent * pevent) 
	{

	bPanelSelectionActive=false;

	}

void QWaveformsDisplay::mousePressEvent ( QMouseEvent * pevent )
	{
		if(pevent->button()==Qt::LeftButton)
			{
			OnLButtonDown(pevent);
			return;
			}
		if(pevent->button()==Qt::RightButton)
			{
			OnRButtonDown(pevent);
			return;
			}
		if(pevent->button()==Qt::MidButton)
			{
			OnMButtonDown(pevent);
			return;
			}

	}
void QWaveformsDisplay::OnMButtonDown(QMouseEvent * pevent)
	{
	mbuttonx=pevent->x();
	mbuttony=pevent->y();
	bMiddleButtonDrag=true;
	setCursor(Qt::ClosedHandCursor);
	}
void QWaveformsDisplay::OnMButtonUp(QMouseEvent * pevent)
	{
	if(bMiddleButtonDrag)
		{
		setCursor(Qt::ArrowCursor);
		bMiddleButtonDrag=false;
		int x=pevent->x();
		int x1=WindowInfoXsize+2;
		int x2=MaxX-2;

		double dttt=(x-mbuttonx)*(t2-t1)/(x2-x1);
		t1-=dttt;
		t2-=dttt;
		if(dttt!=0.)
			{
			DrawWaveforms();
			RedrawPreview();
			}
		else
			{
			update();
			}
		}
	}
void QWaveformsDisplay::OnLButtonDown(QMouseEvent * pevent) 
	{
	int x=pevent->x();
	int y=pevent->y();

//
// Change Focus
//


	int nScr=GetLeftPanelNumberFromPoint( x, y);
	if(nScr>=0)
		{
		bPanelSelectionActive=true;
		oldPanelAct=nScr;


		pScreen->SelectUnselectElement(nScr);


		update(0, 0, WindowInfoXsize+4, MaxY);
		return;
		}



    if( !(bBoxContractX || bBoxExpandX || bBoxExpandY || bBoxContractY))
		{
		if(pevent->modifiers() & Qt::ShiftModifier)
			{
		    mouse_x_old=mouse_x1=mouse_x=x;

			bBoxExpandX=true;
//			nBoxWindow=i;
			}
		if(pevent->modifiers() & Qt::ControlModifier)
			{
			nBoxWindow=GetWindowFromPoint(x, y);
			if(nBoxWindow>=0)
				{
				bBoxExpandY=true;
				mouse_y= y;
				}
			}

		}
	else
		{
		bSelectionActive = false;
		if(bBoxContractY) {ContractY(y);return;	}
		if(bBoxContractX) {ContractX(x);return;	}
		if(bBoxExpandX)	{ExpandX(x);return;	}
		if(bBoxExpandY) {ExpandY(y);return;	}
		}


	int nW=GetWindowFromPoint(x, y);
	if(nW<0) return;

	if( (WaveformInfo.size()>0) && ((pevent->modifiers() & Qt::ShiftModifier)==0) && ((pevent->modifiers() & Qt::ControlModifier)==0))
		{

		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double delta=ttt-t1;

		t2+=delta;
		t1+=delta;

		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}

	}
int QWaveformsDisplay::GetWindowFromPoint(int x, int y)
	{
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWin=0; nWin<nMaxWindow; ++nWin)
		{
		int nW=pScreen->GetWaveformN(nWin);
		int x1=WaveformInfo[nW]->x1;
		int x2=WaveformInfo[nW]->x2;
		int y1=WaveformInfo[nW]->y1;
		int y2=WaveformInfo[nW]->y2;
		int tmp;

		if(x1>x2) {tmp=x1; x1=x2; x2=tmp; }
		if(y1>y2) {tmp=y1; y1=y2; y2=tmp; }

		if( y<=y2 && y>=y1 && x>=x1 && x<=x2) return nWin;
		}
	return -1;
	}

int QWaveformsDisplay::GetLeftPanelNumberFromPoint(int x, int y)
	{
	if(x>WindowInfoXsize) return -1;
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWin=0; nWin<nMaxWindow; ++nWin)
		{
		int nW=pScreen->GetWaveformN(nWin);

		int y1= WaveformInfo[nW]->yp1;
		int y2= WaveformInfo[nW]->yp2;

		if( y<=y2 && y>=y1 && x>0 && x<=WindowInfoXsize) return nWin;
		}
	return -1;
	}
void QWaveformsDisplay::ExpandX(int x)
	{
	int mouse_x1, mouse_x2;

	if(!bBoxExpandX) return;

	bBoxExpandX=false;


	if(x!=mouse_x)
		{
		if(mouse_x>x)
			{
			mouse_x1=x;
			mouse_x2=mouse_x;
			}
		else
			{
			mouse_x2=x;
			mouse_x1=mouse_x;
			}
		double ttt1=(mouse_x1-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double ttt2=(mouse_x2-xX1)*(t2-t1)/(xX2-xX1)+t1;
		t1=ttt1;
		t2=ttt2;
		DrawWaveforms();
		RedrawPreview();
		}

	}
void QWaveformsDisplay::ContractX(int x)
	{
	int mouse_x1, mouse_x2;
	if(!bBoxContractX) return;
	bBoxContractX=false;

	double ttt0=t1;
	double ttt1=t2;


	if(x!=mouse_x)
		{
		if(mouse_x>x)
			{
			mouse_x1=x;
			mouse_x2=mouse_x;
			}
		else
			{
			mouse_x2=x;
			mouse_x1=mouse_x;
			}
		double ttt3=(mouse_x1-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double ttt4=(mouse_x2-xX1)*(t2-t1)/(xX2-xX1)+t1;


		double delta_s=(ttt1-ttt0)*(ttt1-ttt0)/(ttt4-ttt3);
		t1=ttt0-(ttt3-ttt0)*(ttt1-ttt0)/(ttt4-ttt3);
		t2=t1+delta_s;


		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}

	}
void QWaveformsDisplay::ExpandY(int y)
	{
	if(!bBoxExpandY) return;
	bBoxExpandY=false;
	int mouse_y1,mouse_y2;
	bBoxExpandY=false;

	int nWfm=pScreen->GetWaveformN(nBoxWindow);


	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;

	if(y==mouse_y) { update();return;}

//	mouse_y = Bitmap.height() - mouse_y;
//	y       = Bitmap.height() - y;

	if(mouse_y>y)
		{
		mouse_y1=y;
		mouse_y2=mouse_y;
		}
	else
		{
		mouse_y2=y;
		mouse_y1=mouse_y;
		}

	long amax=WaveformInfo[nWfm]->amax;
	long amin=WaveformInfo[nWfm]->amin;
	long aaa1=(long)((double)(mouse_y1-y1)*(amax-amin)/(y2-y1)+amin+0.5);
	long aaa2=(long)((double)(mouse_y2-y1)*(amax-amin)/(y2-y1)+amin+0.5);


	WaveformInfo[nWfm]->amax=aaa2;
	WaveformInfo[nWfm]->amin=aaa1;

	RedrawWindow(nBoxWindow);
	DrawLeftPannel();

	}

void QWaveformsDisplay::ContractY(int y)
	{
	if(!bBoxContractY) return;
	bBoxContractY=false;
	int mouse_y1, mouse_y2;

	int nWfm=pScreen->GetWaveformN(nBoxWindow);

	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;


	if(y==mouse_y) { update();return;}

	if(mouse_y>y)
		{
		mouse_y1=y;
		mouse_y2=mouse_y;
		}
	else
		{
		mouse_y2=y;
		mouse_y1=mouse_y;
		}

	long amax=WaveformInfo[nWfm]->amax;
	long amin=WaveformInfo[nWfm]->amin;
	double aaa0=amin;
	double aaa1=amax;
	double aaa3=(mouse_y1-y1)*(amax-amin)/(y2-y1)+amin;
	double aaa4=(mouse_y2-y1)*(amax-amin)/(y2-y1)+amin;


	double delta_a=(aaa1-aaa0)*(aaa1-aaa0)/(aaa4-aaa3);
	long a1=(long)(aaa0-(aaa3-aaa0)*(aaa1-aaa0)/(aaa4-aaa3));
	long a2=(long)(a1+delta_a);


	WaveformInfo[nWfm]->amax=a2;
	WaveformInfo[nWfm]->amin=a1;


	RedrawWindow(nBoxWindow);
	DrawLeftPannel();

	}

void QWaveformsDisplay::RedrawWindow(int nWindow)
	{

	if(bClip)
		{
		DrawWaveforms();
		return;
		}

	ScreenBitmapCriticalSection.lock();
	QPainter painter;
	int nWfm=pScreen->GetWaveformN(nWindow);

	int xw1=WaveformInfo[nWfm]->xw1;
	int xw2=WaveformInfo[nWfm]->xw2;
	int yw1=WaveformInfo[nWfm]->yw1;
	int yw2=WaveformInfo[nWfm]->yw2;

	int x1=WaveformInfo[nWfm]->x1;
	int x2=WaveformInfo[nWfm]->x2;
	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;


	painter.begin(&Bitmap);
	painter.fillRect(x1, y1, (x2-x1), (y2-y1), QColor( 0,0,192));
	DrawWindow(nWindow, &painter, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
	painter.end();
	ScreenBitmapCriticalSection.unlock();



	update();
	}


void QWaveformsDisplay::OnRButtonDown( QMouseEvent * pevent ) 
	{
	int x=pevent->x();
	int y=pevent->y();
	
	if( (bBoxContractX || bBoxExpandX || bBoxExpandY || bBoxContractY))
		{
		bSelectionActive = false;
		if(bBoxContractY) {ContractY(y);return;	}
		if(bBoxContractX) {ContractX(x);return;	}
		if(bBoxExpandX)	{ExpandX(x);return;	}
		if(bBoxExpandY) {ExpandY(y);return;	}
		}



	if( (WaveformInfo.size()>0) && ((pevent->modifiers() & Qt::ShiftModifier)==0) && ((pevent->modifiers() & Qt::ControlModifier)==0))
		{

		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double delta=ttt-t1;

		t1-=delta;
		t2-=delta;

		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}


    if( !(bBoxContractX || bBoxExpandX || bBoxContractY || bBoxExpandY))
		{
		if(pevent->modifiers() & Qt::ShiftModifier)
			{
		    mouse_x_old=mouse_x1=mouse_x=x;
			bBoxContractX=true;
			nBoxWindow=GetWindowFromPoint(x, y);
			}
		else
		if(pevent->modifiers() & Qt::ControlModifier)
			{
			bBoxContractY=true;
			nBoxWindow=GetWindowFromPoint(x, y);
		    mouse_y=y;
			}
		}
	else
		{
		}

	}
void QWaveformsDisplay::SetAmpFlagFixed()
	{
	DisplayMode=mFx;
	DrawWaveforms();
	}
void QWaveformsDisplay::SetAmpFlagAuto()
	{
	DisplayMode=mAuto;
	DrawWaveforms();
	}
void QWaveformsDisplay::SetAmpFlagAuto0()
	{
	DisplayMode=mAuto0;
	DrawWaveforms();
	}
void QWaveformsDisplay::SetAmpFlagAuto1()
	{
	DisplayMode=mAuto1;
	DrawWaveforms();
	}
void QWaveformsDisplay::SetAmpFlagAutoA()
	{
	DisplayMode=mAutoA;
	DrawWaveforms();
	}
void QWaveformsDisplay::SelectTraces()
	{
	pScreen->SelectTraces();
	SyncStaInfo();
	MainWindow->AdjustWindowsSize();
	DrawWaveforms();
	}
void QWaveformsDisplay::Original()
	{
	pScreen->Clear();
	for(unsigned int l=0; l<WaveformInfo.size(); ++l)
		{
		pScreen->AddElement(l);
		}
	MainWindow->AdjustWindowsSize();
	DrawWaveforms();
	}

void QWaveformsDisplay::SelectAll()
	{
	pScreen->SelectAll();
	SyncStaInfo();
	update();
	}
void QWaveformsDisplay::DeleteAll()
	{
	pScreen->UnSelectAll();
	update();
	}

bool QWaveformsDisplay::ClipOnOff()
	{
	if(bClip)
		{
		bClip=false;
		}
	else
		{
		bClip=true;
		}
	DrawWaveforms();
	return bClip;
	}
void QWaveformsDisplay::DeleteTraces()
	{
	pScreen->DeleteTraces();
	DrawWaveforms();
	}

void QWaveformsDisplay::FlipInvertFlag()
	{
	bInvert=!bInvert;
	DrawWaveforms();
	}

void QWaveformsDisplay::ChangeGain(double nGain, int flag)
	{
	switch(flag)
		{
		case 0:
			dGain=1.;
			break;
		case 1:
			dGain*=nGain;
			break;
		default:;
						
		}

	DrawWaveforms();
	}

void QWaveformsDisplay::mouseMoveEvent ( QMouseEvent * pevent ) 
	{
	int x=pevent->x();
	int y=pevent->y();

	if(pevent->buttons()&Qt::MidButton)
		{
		mouse_x = x;
		bMiddleButtonDrag=true;
		update();
		}

	if(bPanelSelectionActive)
		{
		int nScr=GetLeftPanelNumberFromPoint(x,y);
		if(nScr!=oldPanelAct)
			{
			if(nScr!=-1)
				{
				pScreen->SelectUnselectElement(nScr);
				update(0, 0, WindowInfoXsize+4, MaxY);
				}
			}
		oldPanelAct=nScr;
		return;
		}


	int nWindow=nWindowFromPoint(x,y);

	if(nWindow!=-1)
		{
		char cBuff[80];
		int nWfm=pScreen->GetWaveformN(nWindow);
		long amax=WaveformInfo[nWfm]->amax;
		long amin=WaveformInfo[nWfm]->amin;
		int y1=WaveformInfo[nWfm]->y1;
		int y2=WaveformInfo[nWfm]->y2;


		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		long aaa=(long)((double)( y2+y1-y -y1)*(amax-amin)/(y2-y1)+amin+0.5);

		sprintf(cBuff,"A:%d T:%s %d %d",(long)aaa, util_dttostr(ttt, 1), x, y);

		MainWindow->SetStatusBarText(cBuff);

		}

	if( (pevent->modifiers() & Qt::ShiftModifier) && (!(bBoxContractX || bBoxExpandX || bBoxExpandY|| bBoxContractY)))
		{
		ProcessCursorAction(x, y, pevent);
		}
	else
		{
		if(amplinfwnd.isVisible())amplinfwnd.hide();
		if(infwnd.isVisible())infwnd.hide();
		}

	if(bBoxExpandY||bBoxContractY)
		{
		int x1=WindowInfoXsize+1;
		int x2=MaxX;


		int mousey = y;

		if( (x>x1) && (x<x2))
			{
			if(mousey>mouse_y)
				{
				InvertRect(x1, mouse_y, x2, mousey);
				}
			else
				{
				InvertRect(x1, mousey, x2, mouse_y);
				}

			}
		}

    if(bBoxExpandX || bBoxContractX)
		{
		int x1=mouse_x_old;
		int x2=x;


		if(mouse_x_old>x)
			{
			x1=x;x2=mouse_x_old;
			}
		else
			{
			x1=mouse_x_old; x2=x;
			}
		if(mouse_x1>x)
			InvertRect(x, 0, mouse_x1, Bitmap.height());
		else
			InvertRect(mouse_x1, 0, x, Bitmap.height());

		mouse_x_old=x;
		}

	
	}
void QWaveformsDisplay::InvertRect(int x1, int y1, int x2, int y2)
	{
	QRect InvRect0;
	InvRect0.setCoords(x1, y1, x2, y2);



	QImage Bitmap2 = Bitmap.copy(x1-WindowInfoXsize, y1, InvRect0.width(), InvRect0.height());
	Bitmap2.invertPixels(QImage::InvertRgb);


	ScreenBitmapCriticalSection.lock();
	bSelectionActive = true;
	InvRect = InvRect0;
//	selectedReg = QPixmap::fromImage(Bitmap2);
	selectedReg = Bitmap2;
	ScreenBitmapCriticalSection.unlock();


//	update(x1, y1, x2, y2);
	update();

	}

void QWaveformsDisplay::ProcessCursorAction(int x, int y, QMouseEvent * pevent)
	{

	}

int QWaveformsDisplay::nWindowFromPoint(int x, int y)
	{
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWindow=0; nWindow<nMaxWindow; ++nWindow)
		{
		int nWfm=pScreen->GetWaveformN(nWindow);
		int x1=WaveformInfo[nWfm]->x1;
		int x2=WaveformInfo[nWfm]->x2;
		int y1=WaveformInfo[nWfm]->y1;
		int y2=WaveformInfo[nWfm]->y2;
		if( (y<=y2) && (y>=y1) && (x>=x1) && (x<=x2)) return nWindow;
		}
	return -1;
	}

void QWaveformsDisplay::SetFilterNumber(int nFilt)
	{
	nFilterType=nFilt;
	DrawWaveforms();
	} 

void QWaveformsDisplay::Fit()
	{
	FindMinMaxTimeInWaveformInfo();
	DrawWaveforms();
	RedrawPreview();
	} 
void QWaveformsDisplay::FitVertical()
	{
	DrawWaveforms();
	RedrawPreview();
	} 


void QWaveformsDisplay::setBegTime(double tt)
	{
	t1=tt;
	}

void QWaveformsDisplay::setEndTime(double tt)
	{
	t2=tt;
	}

double QWaveformsDisplay::getBegTime()
	{
	return t1;
	}

double QWaveformsDisplay::getEndTime()
	{
	return t2;
	}

void QWaveformsDisplay::toEnd()
	{
	unsigned int i,k;
	double tMax=-1;

//	tB1=-1;
//	tB2=-1;
//	t1=-1;
//	t2=-1;

	for(i=0; i<WaveformInfo.size(); ++i)
		{
		for(k=0; k<WaveformInfo[i]->wfdrec.size(); ++k)
			{
			double tt2=WaveformInfo[i]->wfdrec[k]->wfd.endtime;
			if( (tMax==-1) || (tMax<tt2)) tMax = tt2; 
			}
		}

	double dqty = t2-t1;

	t2 = tMax;
	t1 = t2 - dqty;


	DrawWaveforms();
	RedrawPreview();
	} 

void QWaveformsDisplay::toBegin()
	{
	unsigned int i,k;
	double tMin=-1;

//	tB1=-1;
//	tB2=-1;
//	t1=-1;
//	t2=-1;

	for(i=0; i<WaveformInfo.size(); ++i)
		{
		for(k=0; k<WaveformInfo[i]->wfdrec.size(); ++k)
			{
			double tt1=WaveformInfo[i]->wfdrec[k]->wfd.time;
			double tt2=WaveformInfo[i]->wfdrec[k]->wfd.endtime;
			if(tMin==-1 || tMin>tt1) tMin = tt1; 
			}
		}

	double dqty = t2-t1;

	t1 = tMin;
	t2 = t1 + dqty;


	DrawWaveforms();
	RedrawPreview();

	}

void QWaveformsDisplay::AdjustWidgetSize()
	{
	int nW;
	QWidget *pW = parentWidget();
	QSize qsz = pW->size();

//	QSize qsz = size();

	long MaxY= qsz.height();
	long MaxX= qsz.width()-1;
	
	int nWin=pScreen->GetMaxWindow();

	if(nWin==0)	return;

	if(nWin<IWVConfig::m_MaxWindow) 
		nW = nWin; 
	else 
		nW = IWVConfig::m_MaxWindow;

	int yHeight=(MaxY)/nW;

//	if(yHeight<50) yHeight = 50; else return;

	MaxY = yHeight*nWin+2;
	QRect qrc = this->geometry();
	qrc.setHeight(MaxY);

	this->setGeometry(qrc);


	}

void QWaveformsDisplay::adjusth()
	{
	QWidget *pW = parentWidget();
	QSize qsz = pW->size();
	setGeometry(0,0, qsz.width(), qsz.height());
	}

void QWaveformsDisplay::SetTWIN(double t)
	{
	twin = t;
	}

double QWaveformsDisplay::Interpolate(double tt1, double aa1, double tt2, double aa2, double tt)
	{
	double a = (aa2-aa1)/(tt2-tt1);
	return a*(tt-tt1)+aa1;
	}

bool QWaveformsDisplay::GetVisibleTraces(int &nFirst, int &nLast)
	{

	if(pScreen->GetMaxWindow()==0) return false;

	QRegion qReg = visibleRegion();
	QRect VisibleRect = qReg.boundingRect();

	int nWf=pScreen->GetWaveformN(0);
	int nHeight = abs(WaveformInfo[nWf]->yw1 - WaveformInfo[nWf]->yw2);

	nFirst = VisibleRect.top() / nHeight;
	nLast  = VisibleRect.height()/ nHeight + nFirst-1;


	return true;
	}

void QWaveformsDisplay::SyncStaInfo()
	{
	int i,j;
	int nMaxWindow=pScreen->GetMaxWindow();

	for(i=0; i<AllStaChan.AllSta.size();++i)
		{
		for(j=0; j<AllStaChan.AllSta[i]->ChanInfo.size(); ++j)
			{
			AllStaChan.AllSta[i]->ChanInfo[j]->bSelected = false;
			for(int nWin=0; nWin<nMaxWindow; ++nWin)
				{
				
				int nW=pScreen->GetWaveformN(nWin);

				if(WaveformInfo[nW]->pSta == AllStaChan.AllSta[i])
					if(AllStaChan.AllSta[i]->ChanInfo[j] == WaveformInfo[nW]->pChan)
						AllStaChan.AllSta[i]->ChanInfo[j]->bSelected = true;
				
				}

				
			}
		}
	}

void QWaveformsDisplay::MakeScreenInfoFromWaveformInfo()
	{
	int nMaxWindow=WaveformInfo.size();

	pScreen->Clear();

	for(int nWin=0; nWin<nMaxWindow; ++nWin)
		{

		if(WaveformInfo[nWin]->pChan->bSelected)
			{
			pScreen->AddElement(nWin);
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
 * $Log: QWaveformsDisplay.cpp,v $
 * Revision 1.2  2010/06/07 18:32:51  akimov
 * Changed pen width and style in order to correct problem with first trace in PDF output
 *
 * Revision 1.1  2010/05/11 18:24:59  akimov
 * initial production release
 *
 */
