#pragma ident $Id: isipng.cpp,v 1.1 2010/02/23 21:58:02 dechavez Exp $
//
// Generate png images of data from selected stations
//
// Author: Andrey Akimov, GSRAS
//
#include <pngwriter.h>
#include "platform.h"
#include "util.h"
#include "isi.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>


using namespace std;

static float findAverage();

int nMaxX = 2000;
int nMaxY = 1000;
int nTimeRuler = 50;
int nHeaderWidth = 100;
int nTMSpace     = 20; // minutes

string qserver;
string qsta, qchan, qlcode;
string qFileName, qIlist;
string sThumbnail;
float ratio = 0.003;
int port = 39136;
int ti = 0;
bool bVerbose = false;
int nTraceSize = 22;
float calib=-1., calper=-1.;
string cacheFileName;
const double pi = 3.14159265358979323846264338327950288;

#define FONT_HEIGHT 11
#define TM_FONT_HEIGHT 8

typedef struct
	{
	char sta[8];
	char chan[5];
	char lcode[5];
	float sint;
	double tbeg;
	int ncounts;
	} DATAHEAD;

typedef struct
	{
	char sta[8];
	char chan[5];
	char lcode[5];
	char cache[256];
	float ratio;
	}
	Waveform;

typedef struct
	{
	float sint;
	double tbeg;
	int ncounts;
	float *fData;
	}
	DataBlock;

vector <DataBlock *> DataArray;
vector <Waveform *> WaveformArray;

inline float GetVelocity(float count)
	{
	float a;
	if(calib>0. && calper>0. ) a = count * calib * 2. *pi/calper; else a = count;
	return a;
	}
static void CleanDataArray()
	{
	long NN = DataArray.size();
	while(DataArray.size()>0)
		{
		DataBlock *p = DataArray[0];
		DataArray.erase(DataArray.begin());
		delete p;
		}
	}



static bool ParseCmd(int argc, char *argv[]);
static void data(const char *server, double begtime, double endtime, char *StreamSpec);
static int findNwindow(double begtime, double t);
static void RemoveOldData(double t);
static float findAverage();
static void draw(Waveform *pw, double begtime, double endtime, pngwriter &paint, int xw1, int xw2, int yw1, int yw2, int nMaxY);

static void LoadCache(const char *pFileName);
static void SaveCache(const char *pFileName);
static int CalcHeaderWidth(pngwriter &paint);
static int GetTimeRulerHeight();
static double GetLastCountTime();
static void DrawTimeRuler(pngwriter &paint, double begtime, double endtime);


static void Usage(char *argv0)
	{
	printf("Usage: %s parameters\n", argv0);
	printf("Parameters:\n");
	printf("file=string	-output bitmap file name. PNG format. *required*\n");
	printf("ilist=string -input station/channel list file name. Default value is 'stalist'\n");
	printf("server=string -sets the name of the server to the specified string. Default value is 127.0.0.1\n");
	printf("port=int -sets the port number. Default value is 39136\n");
	printf("ti=int -time interval in hours (>1). Default value is 24\n");
	printf("height=int -output bitmap height (>1). Default value is 1000\n");
	printf("width =int -output bitmap width (>1). Default value is 2000\n");
	printf("-v -verbose\n");

	}
const char *face_path = "./FreeSansBold.ttf";

int main(int argc, char *argv[])
	{
	int yr, da, hr, mn, sc, ms;
	time_t ltime, ltime1, ltime2;
	
	int i, nwf;
	int xw1, xw2, yw1, yw2;

	char sta[8];
	char chan[5];
	char lcode[5];
	char cache[256];
	float ratio0;

	

	
	if(!ParseCmd(argc, argv)) 
		{
		Usage(argv[0]);
		return -1;
		}

	FILE *flist = fopen(qIlist.c_str(), "r");

	if(flist==NULL) 
		{
		printf("\n input list file '%s' not found", qIlist.c_str());
		return -1;
		}

	while(1)
		{
		int nRet = fscanf(flist,"%s %s %s %s %f", sta, chan, lcode, cache, &ratio0);
		if(nRet==EOF) break;
		if(nRet<0) break;
		if(nRet<5 && nRet>0) continue;

		Waveform *pwf = new Waveform;
		if(pwf==NULL) exit(-1);
		strcpy(pwf->sta,   sta);
		strcpy(pwf->chan,  chan);
		if( strcmp(lcode,"-")==0) 
			*pwf->lcode=0; 
		else 
			strcpy(pwf->lcode, lcode);

		if( strcmp(cache,"-")==0) 
			*pwf->cache=0; 
		else 
			strcpy(pwf->cache, cache);

		pwf->ratio = ratio0;
		if(ratio<=0) continue;
		WaveformArray.push_back(pwf);
		}
	fclose(flist);

	if(WaveformArray.size()==0)
		{
		printf("\n input list station/channel (%s) is empty or has wrong format. Nothing to do. ", qIlist.c_str());
		return -1;
		}

	pngwriter paint(nMaxX, nMaxY, 0, qFileName.c_str());

	paint.setcompressionlevel(9);

	paint.filledsquare(0, 0, nMaxX-1, nMaxY-1, 65535, 65535, 65535);


	nHeaderWidth = CalcHeaderWidth(paint);
	nTimeRuler = (int)(GetTimeRulerHeight()*1.5);

	time(&ltime);
	ltime1 = ltime - ti*3600;


	int YWsize = (nMaxY-nTimeRuler)/WaveformArray.size();
	double begtime, endtime, begtime1;
	begtime = ltime1;
	endtime = ltime;


	for(nwf=0; nwf<WaveformArray.size(); ++nwf)
		{
		CleanDataArray();
		Waveform *pwf = WaveformArray[nwf];

		yw1=nwf*YWsize+nTimeRuler;
		yw2=(nwf+1)*YWsize+nTimeRuler;

		xw1 = 0;
		xw2 = nMaxX-1;



		string sstr;

		sstr="+";
		sstr += pwf->sta;
		sstr += ".";
		sstr += pwf->chan;
		sstr += ".";
		sstr += pwf->lcode;

		transform(sstr.begin(), sstr.end(), sstr.begin(), ::tolower);
		LoadCache(pwf->cache);

		RemoveOldData(begtime);
		begtime1 = GetLastCountTime();
		if(begtime1<0) begtime1 = begtime;
		data((const char *)qserver.c_str(), begtime1, endtime, (char *)sstr.c_str());
		SaveCache(pwf->cache);
		ratio = pwf->ratio;
		draw(pwf, begtime, endtime, paint, xw1, xw2, yw1, yw2, nMaxY);	
		}
	DrawTimeRuler(paint, begtime, endtime);
	paint.close();
	}
static int CalcHeaderWidth(pngwriter &paint)
	{
	int nHW;
	char tmp_buff[1024];
	sprintf(tmp_buff, "XXXXX:XXX:XXX");
	nHW = paint.get_text_width((char *)face_path, FONT_HEIGHT,tmp_buff);
	return nHW; 
	}

static int GetTimeRulerHeight()
	{
	int TimeRectHeight = TM_FONT_HEIGHT+4;
	return TimeRectHeight;
	}

static int GetTimeRulerLabelWidth(pngwriter &paint)
	{
	static char *p = "XXXX:XXX XX:XX:XX";
	return paint.get_text_width((char *)face_path, TM_FONT_HEIGHT, p);
	}



static void drawHeader(Waveform *pw, pngwriter &paint, int xw1, int xw2, int yw1, int yw2, int nMaxY)
	{
	char ctmp[64];

	sprintf(ctmp,"%s:%s:%s", pw->sta, pw->chan, pw->lcode);
	paint.plot_text((char *)face_path, FONT_HEIGHT, 1, abs(yw2+yw1)/2 , 0, ctmp, 0, 0, 0);

	paint.line(xw1+nHeaderWidth,yw1, xw1+nHeaderWidth,yw2, 0, 0, 0);


	}
static void draw(Waveform *pw, double begtime, double endtime, pngwriter &paint, int xw1, int xw2, int yw1, int yw2, int nMaxY)
	{
	int k = 0, i, j;
	double t1, t2, tt, ttold=-1.;
	long NN = DataArray.size();
	long MM;
	float a;
	int xold = -1, yold = -1;
	float sint;
	int ix, iy, yma, ymi;

	t1 = begtime;
	t2 = endtime;
	
//	paint.line(xw1,yw1, xw2,yw1, 0, 0, 0);
//	paint.line(xw2,yw1, xw2,yw2, 0, 0, 0);
//	paint.line(xw2,yw2, xw1,yw2, 0, 0, 0);
//	paint.line(xw1,yw2, xw1,yw1, 0, 0, 0);

	drawHeader(pw, paint, xw1, xw2, yw1, yw2, nMaxY);

	float amid = findAverage();

	for(i=0; i<NN; ++i)
		{
		tt =  DataArray[i]->tbeg;
		MM   = DataArray[i]->ncounts;
		sint = DataArray[i]->sint;

		if(fabs(ttold-tt)>2.*sint) xold=-1; 

		ttold = tt+MM*sint;

		for(j=0; j<MM; ++j,	tt += sint)
			{
			if(tt<t1) continue;
			a = GetVelocity(DataArray[i]->fData[j]);
			ix =(int)( (xw2-xw1-nHeaderWidth)*(tt-t1)/(t2-t1)+xw1+nHeaderWidth);
			iy =(int)( (a-amid)*ratio + (yw2+yw1)/2);

			if(iy>yw2) iy=yw2; else
			if(iy<yw1) iy=yw1;


			if(xold<0)
				{
				xold=ix;
				yma=ymi=yold=iy;
				}
			else
				{
				if(ix==xold)
					{
					if(iy>yma) yma=iy;
					if(iy<ymi) ymi=iy;
					continue;
					}
				else
					{
					if(abs(xold-ix)>=1)
						{
						paint.line(xold, yma, ix, iy, 0, 0, 0);
						}
					paint.line(xold, yma, xold, ymi, 0, 0, 0);
					yma=ymi=yold=iy;
					xold=ix;
					continue;
					}
//					paint.line(xold, yold, ix, iy, 0, 0, 0);
				xold=ix;
				yold=iy;
				}
			}
			paint.line(xold, yma, xold, ymi, 0, 0, 0);
		}

	}

static float findAverage()
	{
	long NN = DataArray.size();
	long MM, nTotalCounts = 0;
	long i,j;
	double t, tt;
	long double a = 0., a1 = 0.;
	double sint;

	for(i=0; i<NN; ++i)
		{
		MM   = DataArray[i]->ncounts;
		a = 0;
		for(j=0; j<MM; ++j)
			{
			a += GetVelocity(DataArray[i]->fData[j]);
			++nTotalCounts;
			}
		a1 = a1*((long double)(nTotalCounts-MM)/(long double)nTotalCounts)+a/(long double)nTotalCounts;
		}
//	a = a/nTotalCounts;
	return a1;
	}



static bool ParseCmd(int argc, char *argv[])
	{
	string s1, s;
	char *p;

	for(int i=1; i<argc; ++i)
		{
		p=argv[i];

		if(strncasecmp("HEIGHT=", p, 7) ==0)
			{
			nMaxY=atoi(p+7);
			continue;
			}

		if(strncasecmp("TMSPACE=", p, 8) ==0)
			{
			nTMSpace=atoi(p+8);
			continue;
			}


		if(strncasecmp("WIDTH=", p, 6) ==0)
			{
			nMaxX=atoi(p+6);
			continue;
			}

		if(strncasecmp("PORT=", p, 5) ==0)
			{
			port=atoi(p+5);
			continue;
			}

		if(strncasecmp("TI=", p, 3) ==0)
			{
			ti=atoi(p+3);
			continue;
			}

		if(strncasecmp("SERVER=", p, 7) ==0)
			{
			qserver=string(p+7);
			continue;
			}

		if(strncasecmp("FILE=", p, 5) ==0)
			{
			qFileName=string(p+5);
			continue;
			}


		if(strncasecmp("ILIST=", p, 6) ==0)
			{
			qIlist=string(p+6);
			continue;
			}

		if(strncasecmp("-V", p, 2) ==0)
			{
			bVerbose = true;
			continue;
			}

		return false;
		}


	if( (nMaxX<=0) || (nMaxY<=0) || (nMaxX>5000) || (nMaxY>5000)) return false;

	if( (ti==0)) ti=24;
	if( (ti<0) || (ti>48) ) return false;
	if(qserver.empty()) qserver = "127.0.0.1";
	if(qIlist.empty()) qIlist   = "stalist";
	if(qFileName.empty()) return false;
	return true;
	}

#define LOCALBUFLEN 1024 


static ISI_DATA_REQUEST *BuildDataRequest(int format, int compress, REAL64 begtime, REAL64 endtime, char *StreamSpec)
{
ISI_DATA_REQUEST *dreq;

    if ((dreq = isiAllocSimpleDataRequest(begtime, endtime, StreamSpec)) == NULL) {
        fprintf(stderr, "isiAllocSimpleDataRequest: %s\n", strerror(errno));
        exit(1);
    }
    isiSetDatreqFormat(dreq, format);
    isiSetDatreqCompress(dreq, compress);

    return dreq;
} 

static void data(const char *server, double begtime, double endtime, char *StreamSpec)
{
ISI *isi;
int status;
ISI_GENERIC_TS *ts;
ISI_DATA_REQUEST *dreq;
UINT8 buf[LOCALBUFLEN];
ISI_PARAM par; 
int i;
int compress  = ISI_COMP_IDA;
int format    = ISI_FORMAT_GENERIC; 

	utilNetworkInit();
    isiInitDefaultPar(&par); 

    dreq = BuildDataRequest(format, compress, begtime, endtime, StreamSpec);

    if (bVerbose) {
        fprintf(stderr, "Client side data request\n");
        isiPrintDatreq(stderr, dreq);
   }

    if ((isi = isiInitiateDataRequest((char *)server, &par, dreq)) == NULL) {
        if (errno == ENOENT) {
            fprintf(stderr, "can't connect to server %s, port %d\n", server, par.port);
        } else {
            perror("isiInitiateDataRequest");
        }
        return;
    }

    isiFreeDataRequest(dreq);

    if (bVerbose) {
        fprintf(stderr, "Server expanded data request\n");
        isiPrintDatreq(stderr, &isi->datreq);
    }

    while ((ts = isiReadGenericTS(isi, &status)) != NULL) {

		if(status != ISI_OK) break;
	
		if (bVerbose) {
        printf("%s", isiGenericTsHdrString(&ts->hdr, (char *) buf));
		}

		DataBlock *p = new DataBlock();

		p->sint    = isiSrateToSint(&ts->hdr.srate); 
		p->tbeg    = ts->hdr.tofs.value;
		p->ncounts = ts->hdr.nsamp;
		p->fData   = new float[ts->hdr.nsamp];

		short *shData = (short *)ts->data;
		long  *lData  = (long *)ts->data;


		for(i=0; i<p->ncounts; ++i)
			{
			if(ts->hdr.desc.type==ISI_TYPE_INT16)
				{
				p->fData[i]=(float)shData[i];
				}
			if(ts->hdr.desc.type==ISI_TYPE_INT32)
				{
				p->fData[i]=(float)lData[i];
				}

			}

	DataArray.push_back(p);

    }

    if (status != ISI_DONE) perror("isiReadGenericTS");
} 

static void LoadCache(const char *pFileName)
	{
	DATAHEAD dh;
	char sta[8];
	char chan[5];
	char lcode[5];
	float sint;
	double tbeg;
	int ncounts;

	char buff[128];

	if(*pFileName==0) return;

	int headerlen = sizeof(DATAHEAD);
	int ff = open(cacheFileName.c_str(),O_BINARY|O_RDONLY);
	if(ff<0) return;

	do
		{
		if(read(ff,&dh,headerlen)!=headerlen)
			{
			close(ff); return;
			}
		if(strcasecmp(dh.sta, qsta.c_str())!=0) 
				{
				close(ff); return;
				}
		if(strcasecmp(dh.chan, qchan.c_str())!=0)
				{
				close(ff); return;
				}
		DataBlock *p = new DataBlock();
		if(p==NULL)
			{
			close(ff); return;
			}
		p->sint    = dh.sint; 
		p->tbeg    = dh.tbeg;
		p->ncounts = dh.ncounts;
		p->fData   = new float[dh.ncounts];


		int nSz = sizeof(float)*dh.ncounts;
		if(read(ff,p->fData,nSz)!=nSz)
			{
			delete p;
			close(ff); return;
			}
		DataArray.push_back(p);
		}
		while(1);
	}
static void SaveCache(const char *pFileName)
	{
	DATAHEAD dh;
	char sta[8];
	char chan[5];
	char lcode[5];
	float sint;
	double tbeg;
	int ncounts;
	int i;

	char buff[128];

	if(*pFileName==0) return;

	int headerlen = sizeof(DATAHEAD);
	int ff = open(pFileName,O_BINARY|O_WRONLY|O_TRUNC);
	if(ff<0) 
		{
		ff = open(pFileName,O_BINARY|O_WRONLY|O_CREAT|O_TRUNC,0666);
		if(ff<0) 
			{
			printf("\n write open error");
			return;
			}
		}

	int nBlocks = DataArray.size();

	for(i=0; i<nBlocks; ++i)
		{
		DataBlock *p = DataArray[i];
		memset(dh.sta,0,sizeof(dh.sta));
		strcpy(dh.sta, qsta.c_str());

		memset(dh.chan,0,sizeof(dh.chan));
		strcpy(dh.chan, qchan.c_str());

		memset(dh.lcode,0,sizeof(dh.chan));
		strcpy(dh.lcode, qlcode.c_str());

		dh.ncounts =	p->ncounts;
		dh.sint  =  p->sint;
		dh.tbeg  =  p->tbeg;

		write(ff, &dh, sizeof(dh));

		if(write(ff, p->fData, sizeof(float)*p->ncounts)!=sizeof(float)*p->ncounts) break;
		}
	close(ff);
	}

static void RemoveOldData(double t)
	{
	DataBlock *p;
	int i = 0;

	while(i<DataArray.size())
		{
		p = DataArray[i];
		double tt = p->tbeg+p->sint*p->ncounts;

		if(tt<t)
			{
			delete p;
			DataArray.erase(DataArray.begin()+i);
			}
		else
			++i;
		}
	}
static double GetLastCountTime()
	{
	double t=-1.;
	DataBlock *p;
	int nn = DataArray.size();

	if(nn>0)
		{
		p = DataArray[nn-1];
		t = p->tbeg+p->sint*p->ncounts;
		}
	return t;

	}
static void DrawTimeRuler(pngwriter &paint, double begtime, double endtime)
	{
	int yr, da, hr, mn, sc, ms;
	char tmp_buff[128];
	int xw1, xw2, yw1, yw2, ix;
	int lastLabelPos=-1;
	long nSp = nTMSpace * 60;

	int nTimeRulerLabelWidth = GetTimeRulerLabelWidth(paint);

	xw1 = nHeaderWidth;
	xw2 = nMaxX-1;
	yw2 = nTimeRuler;
	yw1 = 1;


	paint.line(xw1,yw1, xw2,yw1, 0, 0, 0);
	paint.line(xw2,yw1, xw2,yw2, 0, 0, 0);
	paint.line(xw2,yw2, xw1,yw2, 0, 0, 0);
//	paint.line(xw1,yw2, xw1,yw1, 65535, 65535, 65535);


	long l, ttstart=(long)(begtime)-(long)(begtime)%nSp;

//	ttstart = (ttstart+1) * 60 * 60;
	for(l=ttstart; l<endtime; l += nSp)
		{
		if(l<begtime) continue;
		ix =(int)( ((double)xw2-(double)xw1)*((double)l-(double)begtime)/(endtime-begtime)+xw1);



		if(lastLabelPos>0)
			{
			if( (lastLabelPos+nTimeRulerLabelWidth/2)>(ix-nTimeRulerLabelWidth/2) ) continue;
			}
//		if((ix-nTimeRulerLabelWidth/2)<xw1 || (ix+nTimeRulerLabelWidth/2)>xw2) continue;
		if((ix-nTimeRulerLabelWidth/2)<0 || (ix)>xw2) continue;

		lastLabelPos = ix;
		paint.line(ix, nTimeRuler, ix, nMaxY, 0, 0, 0);
		utilTsplit( (double) l, &yr, &da, &hr, &mn, &sc, &ms);
		sprintf(tmp_buff, "%4d.%03d %02d:%02d:%02d", yr, da, hr, mn, sc);
		paint.plot_text((char *)face_path, TM_FONT_HEIGHT, ix-nTimeRulerLabelWidth/2, yw1+2, 0, tmp_buff, 0, 0, 0);
		}





	}

/* Revision History
 *
 * $Log: isipng.cpp,v $
 * Revision 1.1  2010/02/23 21:58:02  dechavez
 * Initial release - aap
 *
 */
