#include <gd.h>
#include "platform.h"
#include "util.h"
#include "isi.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>


using namespace std;

static void draw(REAL64 begtime, gdImagePtr paint, int xw1, int xw2, int yw1, int yw2, int nMaxY);
static REAL32 findAverage(REAL64 begtime);
static void drawAxes(REAL64 begtime, gdImagePtr paint, int &xw1, int &xw2, int &yw1, int &yw2, int nMaxY);

static char face_path[] = "./FreeSansBold.ttf";

const int nMaxX = 1000;
const int nMaxY = 600;
int nTimeRuller = 50;

string qserver;
string qsta, qchan, qlcode;
string qFileName;
string sThumbnail;
REAL32 ratio = 0.003;
int port = 39136;
bool bVerbose = false;
int nTraceSize = 22;
REAL32 calib=-1., calper=-1.;
string cacheFileName;
const REAL64 pi = 3.14159265358979323846264338327950288;
int cliplevel = 3;
#define FONT_HEIGHT 11
#define THUMBNAIL_SCALE_FACTOR 0.2
typedef struct
{
	char sta[8];
	char chan[5];
	char lcode[5];
	REAL32 sint;
	REAL64 tbeg;
	int ncounts;
} DATAHEAD;

typedef struct
{
	REAL32 sint;
	REAL64 tbeg;
	int ncounts;
	REAL32 *fData;
}
DataBlock;

vector <DataBlock *> DataArray;

REAL32 GetVelocity(REAL32 count)
{
	REAL32 a;
	if(calib>0. && calper>0. ) a = count * calib * 2. *pi/calper; else a = count;
	return a;
}


bool ParseCmd(int argc, char *argv[]);
static void data(const char *server, REAL64 begtime, REAL64 endtime, char *StreamSpec);
static int findNwindow(REAL64 begtime, REAL64 t);
void RemoveOldData(REAL64 t);
void LoadCache();
void SaveCache();
REAL64 GetLastCountTime();

int xMid[24];

int white;
int black;
int red;



static int CalcTextSize(gdImagePtr &paint, char *pT, int *width, int *height)
{
    int nHW;
    int brect[8];
    char *err;

    err = gdImageStringFT(NULL,&brect[0], 0, (char *)face_path, FONT_HEIGHT,0., 0, 0, pT);
    if (err) {fprintf(stderr,err); return 1;}

    *width  = brect[2]-brect[6];
    *height = brect[3]-brect[7];

    return 0;

}

static void Usage(char *argv0)
{
	printf("Usage: %s parameters\n", argv0);
	printf("Parameters:\n");
	printf("file=string			-output bitmap file name. PNG format. *required*\n");
	printf("thumbnail=string	-thumbnail name. PNG format. *if required*\n");
	printf("server=string		-sets the name of the server to the specified string. Default value is 127.0.0.1\n");
	printf("port=int			-sets the port number. Default value is 39136\n"); 
	printf("sta=string			-station name. *required*\n");
	printf("chan=string			-channel name. *required*\n");
	printf("lcode=string		-location code. *if required*\n");
	printf("ratio=real32		-pixel/count ratio. Default value is 0.003\n");
	printf("calib=real32		-calibration value.\n");
	printf("calper=real32	-calibration period.\n");
	printf("cache=file name		-file to keep downloaded data which will be used in the next session\n");
	printf("clip=int			- trace clip level. Default value is 3\n");
	printf("-v					-verbose\n");

}





int main(int argc, char *argv[])
{
	int yr, da, hr, mn, sc, ms;
	time_t ltime, ltime1, ltime2;
	
	int i;
	int xw1, xw2, yw1, yw2;
	

	
	if(!ParseCmd(argc, argv)) 
    {
		Usage(argv[0]);
		return -1;
    }



    gdImagePtr paint;

    paint = gdImageCreate(nMaxX, nMaxY);

    black	= gdImageColorAllocate(paint,   0,   0,   0);
    white	= gdImageColorAllocate(paint, 255, 255, 255);
    red		= gdImageColorAllocate(paint, 255,   0,   0);


	time(&ltime);
	ltime1 = ltime - 23*3600;
	utilTsplit( (REAL64) ltime1, &yr, &da, &hr, &mn, &sc, &ms);
	ltime1=(time_t)utilYdhmsmtod(yr, da, hr, 0, 0, 0);	



	REAL64 begtime, endtime, begtime1;
	begtime = ltime1;
	endtime = ltime;


	drawAxes(begtime, paint, xw1, xw2, yw1, yw2, nMaxY);

	string sstr;

	sstr="+";
	sstr += qsta;
	sstr += ".";
	sstr += qchan;
	sstr += ".";
	sstr +=qlcode;

	transform(sstr.begin(), sstr.end(), sstr.begin(), ::tolower);
	LoadCache();
	RemoveOldData(begtime);
	begtime1 = GetLastCountTime();
	if(begtime1<0) begtime1 = begtime;
	data((const char *)qserver.c_str(), begtime1, endtime, (char *)sstr.c_str());
	SaveCache();


	draw(begtime, paint, xw1, xw2, yw1, yw2, nMaxY);	


    FILE *f = fopen(qFileName.c_str(), "wb");

    if(f==NULL)
    {
        printf("\n image file open error: %s", qFileName.c_str() );
        return false;

    }


    gdImageGif(paint, f);
    fclose(f);

	if(!sThumbnail.empty())
    {
        gdImagePtr paint_thumb;
        paint_thumb = gdImageCreateTrueColor(nMaxX*THUMBNAIL_SCALE_FACTOR, nMaxY*THUMBNAIL_SCALE_FACTOR);
        gdImageCopyResampled (paint_thumb, paint, 0, 0, 0, 0, nMaxX*THUMBNAIL_SCALE_FACTOR, nMaxY*THUMBNAIL_SCALE_FACTOR, nMaxX, nMaxY);

        f = fopen((char *)sThumbnail.c_str(), "wb");

        if(f!=NULL)
        {
            gdImageGif(paint_thumb, f);
            fclose(f);

        }
        else
        {
            printf("\n image file open error: %s", sThumbnail.c_str() );
        }



    }
	



	}
static void drawAxes(REAL64 begtime, gdImagePtr paint, int &xw1, int &xw2, int &yw1, int &yw2, int nMaxY)
	{
	int yr, da, daym, mno, hr, mn, sc, ms;
	char tmp_buff[1024];
    int brect[8];
	string sTimeTick;
	int i;
	int nMinLabelH, nMinLabelW;

    gdImageFilledRectangle(paint, 0, 0, 1000, 600, white);

	sprintf(tmp_buff, "XXMin");
    nMinLabelH = FONT_HEIGHT;

    CalcTextSize( paint, tmp_buff, &nMinLabelW, &nMinLabelH );

	sprintf(tmp_buff, "00:00:00");
    int TimeRectWidth ;
	int TimeRectHeight = FONT_HEIGHT;
    CalcTextSize( paint, tmp_buff, &TimeRectWidth, &TimeRectHeight );


	nTimeRuller = TimeRectWidth + 5;



	xw1 = nTimeRuller+1;
	xw2 = nMaxX-2;
	yw1 = nMaxY - nTraceSize * 26;
	yw2 = nMaxY - nTraceSize;


    gdImageLine(paint, xw1, yw1, xw2,yw1, black);
    gdImageLine(paint, xw2,yw1, xw2, yw2, black);
    gdImageLine(paint, xw2,yw2, xw1,yw2, black);
    gdImageLine(paint, xw1,yw2, xw1,yw1, black);

	utilTsplit( (REAL64) begtime, &yr, &da, &hr, &mn, &sc, &ms);


	int hour = hr;
    int nY = 2*nTraceSize;

	for(i=0; i<24; ++i)
		{

		sprintf(tmp_buff,"%02d:00:00", hour);
        gdImageStringFT(paint, &brect[0], 0, face_path, FONT_HEIGHT, 0., 1, nY + TimeRectHeight/2 , tmp_buff);



        gdImageLine(paint, TimeRectWidth, nY, nTimeRuller, nY, black);

        xMid[i] = nMaxY- nY;

        nY += nTraceSize;


		++hour;
		if(hour==24) hour = 0;

		
		}

	for(i=10; i<60; i+=10)
    {

		int ix =(int) ((i)*(xw2-xw1)/(60.)+xw1);
        gdImageLine(paint, ix, yw1, ix, yw2, black);
		sprintf(tmp_buff,"%02dMin", i);
        gdImageStringFT(paint, &brect[0], 0, face_path, FONT_HEIGHT, 0., ix-nMinLabelW/2, yw1-FONT_HEIGHT , tmp_buff);

    }
	

	utilJdtomd(yr, da, &mno, &daym);
	sprintf(tmp_buff,"%s::%s::%s Start date %04d.%02d.%02d", qsta.c_str() , qchan.c_str(), qlcode.c_str(), yr, mno, daym);
    gdImageStringFT(paint, &brect[0], 0, face_path, FONT_HEIGHT, 0., xw1, yw2+FONT_HEIGHT+4, tmp_buff);



	}
static void draw(REAL64 begtime, gdImagePtr paint, int xw1, int xw2, int yw1, int yw2, int nMaxY)
	{
	int k = 0, i, j;
	REAL64 t1, t2, tt, ttold=-1.;
	INT32 NN = DataArray.size();
	INT32 MM;
	REAL32 a;
	int xold = -1, yold = -1;
	REAL32 sint;
	int ix, iy, yma, ymi;

		t1 = begtime;
		t2 = t1 + 3600;
	

		REAL32 amid = findAverage(t1);

		for(i=0; i<NN; ++i)
			{
			tt =  DataArray[i]->tbeg;
			MM   = DataArray[i]->ncounts;
			sint = DataArray[i]->sint;

			if(fabs(ttold-tt)>2.*sint) xold=-1; 

			ttold = tt+MM*sint;

			for(j=0; j<MM; ++j)
				{
				tt += sint;
				if(tt<t1) continue;
				if(tt>t2)
					{
					int nW = findNwindow(begtime, tt);
					if(nW<0) break;
					k = nW;
					t1 = begtime+3600*k;
					t2 = t1 + 3600;
					xold = -1; yold = -1;
					amid = findAverage(t1);
					}
				a = GetVelocity(DataArray[i]->fData[j]);
				ix =(int)( (xw2-xw1)*(tt-t1)/(t2-t1)+xw1);
				iy =(int)( (a-amid)*ratio + xMid[k]);

				INT32 ddy = cliplevel*nTraceSize;
				if(iy>xMid[k]+ddy) iy=xMid[k]+ddy; else
				if(iy<xMid[k]-ddy) iy=xMid[k]-ddy;

				if(iy<xMid[23]-nTraceSize) iy=xMid[23]-nTraceSize;

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
                                gdImageLine(paint, xold, nMaxY-yma, ix, nMaxY-iy, black);
							}
                        gdImageLine(paint, xold, nMaxY - yma, xold, nMaxY - ymi, black);
						yma=ymi=yold=iy;
						xold=ix;
						continue;
						}
//					paint.line(xold, yold, ix, iy, 0, 0, 0);
					xold=ix;
					yold=iy;
					}
				}
                gdImageLine(paint, xold, nMaxY - yma, xold, nMaxY - ymi, black);
		}

	}
int findNwindow(REAL64 begtime, REAL64 t)
	{
	int i;
	REAL64 t1,t2;

	if( t < begtime || t> begtime+3600*24) return -1;

	t1 = begtime;
	t2 = t1 +3600;

	for(i=0; i<24; ++i)
		{
		if( (t>=t1) && (t<t2) ) return i;
		t1 += 3600;
		t2 += 3600;
		}
	return -1;
	}
static REAL32 findAverage(REAL64 begtime)
	{
	INT32 NN = DataArray.size();
	INT32 MM, nTotalCounts = 0;
	INT32 i,j;
	REAL64 t, tt;
	REAL64 a = 0.;
	REAL64 sint;

	for(i=0; i<NN; ++i)
		{
		tt = t    = DataArray[i]->tbeg;
		MM   = DataArray[i]->ncounts;
		sint = DataArray[i]->sint;

		for(j=0; j<MM; ++j)
			{
			tt += sint/1000.;
			if(tt<begtime) continue;
			if(tt>begtime+3600) break;
			a += GetVelocity(DataArray[i]->fData[j]);
			++nTotalCounts;
			}
		if(tt>begtime+3600) break;
		}
	a = a/nTotalCounts;
	return a;
	}

bool ParseCmd(int argc, char *argv[])
	{
	string s1, s;
	char *p;

	for(int i=1; i<argc; ++i)
		{
		p=argv[i];
		
		if(strncasecmp("PORT=", p, 5) ==0)
			{
			port=atoi(p+5);
			continue;
			}

		if(strncasecmp("clip=", p, 5) ==0)
			{
			cliplevel=atoi(p+5);
			continue;
			}

		if(strncasecmp("STA=", p, 4) ==0)
			{
			qsta=string(p+4);
			continue;
			}

		if(strncasecmp("CHAN=", p, 5) ==0)
			{
			qchan=string(p+5);
			continue;
			}

		if(strncasecmp("SERVER=", p, 7) ==0)
			{
			qserver=string(p+7);
			continue;
			}
		if(strncasecmp("LCODE=", p, 6) ==0)
			{
			qlcode=string(p+6);
			continue;
			}
		if(strncasecmp("FILE=", p, 5) ==0)
			{
			qFileName=string(p+5);
			continue;
			}
		if(strncasecmp("RATIO=", p, 6) ==0)
			{
			ratio=atof(p+6);
			continue;
			}

		if(strncasecmp("CALIB=", p, 6) ==0)
			{
			calib=atof(p+6);
			continue;
			}

		if(strncasecmp("CALPER=", p, 7) ==0)
			{
			calper=atof(p+7);
			continue;
			}

		if(strncasecmp("CACHE=", p, 6) ==0)
			{
			cacheFileName=string(p+6);
			continue;
			}


		if(strncasecmp("-V", p, 2) ==0)
			{
			bVerbose = true;
			continue;
			}


		if(strncasecmp("THUMBNAIL=", p, 10) ==0)
			{
			sThumbnail=string(p+10);
			continue;
			}


		return false;
		}
	if(qsta.empty()) return false;
	if(qchan.empty()) return false;
	if(qserver.empty()) qserver = "127.0.0.1";
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

static void data(const char *server, REAL64 begtime, REAL64 endtime, char *StreamSpec)
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
        printf("%s\n", isiGenericTsHdrString(&ts->hdr, (char *) buf));
		}

		DataBlock *p = new DataBlock();

		p->sint    = isiSrateToSint(&ts->hdr.srate); 
		p->tbeg    = ts->hdr.tofs.value;
		p->ncounts = ts->hdr.nsamp;
		p->fData   = new REAL32[ts->hdr.nsamp];

		INT16 *shData = (INT16 *)ts->data;
		INT32  *lData  = (INT32 *)ts->data;


		for(i=0; i<p->ncounts; ++i)
			{
			if(ts->hdr.desc.type==ISI_TYPE_INT16)
				{
				p->fData[i]=(REAL32)shData[i];
				}
			if(ts->hdr.desc.type==ISI_TYPE_INT32)
				{
				p->fData[i]=(REAL32)lData[i];
				}

			}

	DataArray.push_back(p);

    }

    if (status != ISI_DONE) perror("isiReadGenericTS");
} 

void LoadCache()
	{
	DATAHEAD dh;
	char sta[8];
	char chan[5];
	char lcode[5];
	REAL32 sint;
	REAL64 tbeg;
	int ncounts;

	char buff[128];

	if(cacheFileName.empty()) return;

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
		p->fData   = new REAL32[dh.ncounts];


		int nSz = sizeof(REAL32)*dh.ncounts;
		if(read(ff,p->fData,nSz)!=nSz)
			{
			delete p;
			close(ff); return;
			}
		DataArray.push_back(p);
		}
		while(1);
	}
void SaveCache()
	{
	DATAHEAD dh;
	char sta[8];
	char chan[5];
	char lcode[5];
	REAL32 sint;
	REAL64 tbeg;
	int ncounts;
	int i;

	char buff[128];

	if(cacheFileName.empty()) return;

	int headerlen = sizeof(DATAHEAD);
	int ff = open(cacheFileName.c_str(),O_BINARY|O_WRONLY|O_TRUNC);
	if(ff<0) 
		{
		ff = open(cacheFileName.c_str(),O_BINARY|O_WRONLY|O_CREAT|O_TRUNC,0666);
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

		if(write(ff, p->fData, sizeof(REAL32)*p->ncounts)!=sizeof(REAL32)*p->ncounts) break;
		}
	close(ff);
	}

void RemoveOldData(REAL64 t)
	{
	DataBlock *p;
	int i = 0;

	while(i<DataArray.size())
		{
		p = DataArray[i];
		REAL64 tt = p->tbeg+p->sint*p->ncounts;

		if(tt<t)
			{
			delete p;
			DataArray.erase(DataArray.begin()+i);
			}
		else
			++i;
		}
	}
REAL64 GetLastCountTime()
	{
	REAL64 t=-1.;
	DataBlock *p;
	int nn = DataArray.size();

	if(nn>0)
		{
		p = DataArray[nn-1];
		t = p->tbeg+p->sint*p->ncounts;
		}
	return t;

	}
