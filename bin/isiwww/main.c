#pragma ident "$Id: main.c,v 1.24 2015/12/08 21:18:22 dechavez Exp $"
/*======================================================================
 *
 *  Generate IDA latency web pages
 *
 *====================================================================*/
#include "isi.h"
#include "util.h"

extern char *VersionIdentString;

static time_t now;
static struct {
    struct tm tm;
    char buf[64];
} gm, local;

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "idahub.ucsd.edu"
#endif /* DEFAULT_SERVER */

#ifndef DEFAULT_WEBSERVER
#define DEFAULT_WEBSERVER "ida.ucsd.edu"
#endif /* DEFAULT_WEBSERVER */

#ifndef DEFAULT_WEBPATH
#define DEFAULT_WEBPATH "Telemetry/"
#endif /* DEFAULT_WEBPATH */

#ifndef DEFAULT_STATIONDIR
#define DEFAULT_STATIONDIR "Stations/"
#endif /* DEFAULT_STATIONDIR */

#ifndef DEFAULT_HTDOCPATH
#define DEFAULT_HTDOCPATH "/ida/web"
#endif

#ifndef SAMP_PER_PACKET
#define  SAMP_PER_PACKET 240
#endif /*  SAMP_PER_PACKET */

#define LIVE_LATENCY_THRESHOLD 21600  // 6 hrs

typedef struct {
    int value;
    int percent;
    int bg;
    int fg;
} COLOR_MAP;

#define NumColors 5
#define NumColorsNseg 3

#define PURPLE 0xCC99FF
#define HAPPY  PURPLE
#define BLUE   0x3399FF
#define GREEN  0x00FF00
#define AQUMAR 0x7FFFA5
#define YELLOW 0xEDE73B
#define TAN    0xFF9966
#define RED    0xEE4900
#define PINK   0xFFCCCC
#define LTGRAY 0xCCCCCC
#define MDGRAY 0x999999
#define DKGRAY 0x666666
#define BLACK  0x000000
#define WHITE  0xffffff

static COLOR_MAP ColorMap[] = {
    /* { 432000, 0,  DKGRAY, WHITE },   //  5 days */
    /* { 345600, 10, MDGRAY, WHITE },   //  4 days */
    /* { 259200, 25, LTGRAY, BLACK },   //  3 days */
    /* { 172800, 37, PINK,   BLACK },   //  2 day */
    {  86499, 50,  RED,    BLACK },   //  1 day
    {  21600, 66,  TAN,    BLACK },   //  6 hrs
    {   7200, 75,  YELLOW, BLACK },   //  2 hr
    {   3600, 87,  AQUMAR, BLACK },   //  1 hr
    {      0, 100, BLUE,   BLACK},
    {     -1, 0, 0, 0}
};

static COLOR_MAP ColorMapNseg[] = {
    {     10, 10,  AQUMAR, BLACK},    //  lt 10
    {     50, 50,  YELLOW, BLACK},    //  lt 50
    {     51, 51,  RED,    BLACK}     //  gt 50
};

static void PrintErrorMessageAndExit()
{
   fprintf(stderr, "Problem establishing Color Values \n");
   exit(0);
}

static int GetBg(UINT32 test)
{
int i, unreached;

    for (i = 0; ColorMap[i].value >= 0; i++) {
        if (test >= ColorMap[i].value) return ColorMap[i].bg;
    }
    PrintErrorMessageAndExit();
    return unreached; /* just to calm some compilers */
}

static int GetFg(UINT32 test)
{
int i, unreached;

    for (i = 0; ColorMap[i].value >= 0; i++) {
        if (test >= ColorMap[i].value) return ColorMap[i].fg;
    }
    PrintErrorMessageAndExit();
    return unreached; /* just to calm some compilers */
}

static int GetPercent(UINT32 test)
{
int i, unreached;

    for (i = 0; ColorMap[i].value >= 0; i++) {
        if (test >= ColorMap[i].value) return ColorMap[i].percent;
    }
    PrintErrorMessageAndExit();
    return unreached; /* just to calm some compilers */
}

static int GetPercentbg(UINT32 test)
{
int i, unreached;

    for (i = (NumColors-1); i >= 0; i--) {
        if (test >= ColorMap[i].percent) return ColorMap[i].bg;
    }
    PrintErrorMessageAndExit();
    return unreached; /* just to calm some compilers */
}

static int GetPercentfg(UINT32 test)
{
int i, unreached;

    for (i = (NumColors-1); i >= 0; i--) {
        if (test >= ColorMap[i].percent) return ColorMap[i].fg;
    }
    PrintErrorMessageAndExit();
    return unreached; /* just to calm some compilers */
}

static int GetNsegbg(UINT32 test)
{
int i, indexval, unreached;

    for (i = 0; (i < NumColorsNseg);  i++) {
        if (test < ColorMapNseg[i].percent) return ColorMapNseg[i].bg;
    }
    return ColorMapNseg[2].bg;
    return unreached; /* just to calm some compilers */
}

static int Getnsegfg(UINT32 test)
{
    if (test < 100) return ColorMapNseg[0].fg;
    return ColorMapNseg[2].fg;
}

static void UcaseStationAndLoc(ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf)
{
int i;

    for (i = 0; i < soh->nentry; i++) {
        util_ucase(soh->entry[i].name.sta);
        util_ucase(cnf->entry[i].name.sta);
        util_ucase(soh->entry[i].name.loc);
        util_ucase(cnf->entry[i].name.loc);
        util_ucase(soh->entry[i].name.chn);
        util_ucase(cnf->entry[i].name.chn);
        if (soh->entry[i].name.loc[0] == ' ') sprintf(soh->entry[i].name.loc, "%s", "--");
        if (cnf->entry[i].name.loc[0] == ' ') sprintf(cnf->entry[i].name.loc, "%s", "--");
    }
}

static int SortSohName(const void *a, const void *b)
{
int result;

    result = strcmp(((ISI_STREAM_SOH *) a)->name.sta, ((ISI_STREAM_SOH *) b)->name.sta);
    if (result != 0) return result;

    result = strcmp(((ISI_STREAM_SOH *) a)->name.loc, ((ISI_STREAM_SOH *) b)->name.loc);
    if (result != 0) return result;

    return strcmp(((ISI_STREAM_SOH *) a)->name.chn, ((ISI_STREAM_SOH *) b)->name.chn);
    /* return strcmp(((ISI_STREAM_SOH *) a)->name.loc, ((ISI_STREAM_SOH *) b)->name.loc); */
}

static VOID SortSoh(ISI_SOH_REPORT *soh)
{
    qsort(soh->entry, soh->nentry, sizeof(ISI_STREAM_SOH), SortSohName);
}

static int SortCnfName(const void *a, const void *b)
{
int result;

    result = strcmp(((ISI_STREAM_CNF *) a)->name.sta, ((ISI_STREAM_CNF *) b)->name.sta);
    if (result != 0) return result;

    result = strcmp(((ISI_STREAM_CNF *) a)->name.loc, ((ISI_STREAM_CNF *) b)->name.loc);
    if (result != 0) return result;

    return strcmp(((ISI_STREAM_CNF *) a)->name.chn, ((ISI_STREAM_CNF *) b)->name.chn);
    /* return strcmp(((ISI_STREAM_CNF *) a)->name.loc, ((ISI_STREAM_CNF *) b)->name.loc); */
}

static VOID SortCnf(ISI_CNF_REPORT *cnf)
{
    qsort(cnf->entry, cnf->nentry, sizeof(ISI_STREAM_CNF), SortCnfName);
}

static void PrintTimeStamp(FILE *fp)
{
    fprintf(fp,"<p class=\"font-italic text-center\">Page last updated at %s</p>", local.buf);
}

static void MakeLinkColorSS(FILE *fp)
{
    fprintf(fp, "<style type=\"text/css\" media=\"all\">\n");
    fprintf(fp, "a.broken:link {\n");
    fprintf(fp, "color: #fff; \n");
    fprintf(fp, "} \n");

    fprintf(fp, ".legendtbl th    {  \n");
    fprintf(fp, "text-align: center; \n");
    fprintf(fp, "} \n");

    fprintf(fp, ".legendtbl td    {  \n");
    fprintf(fp, "font-size: smaller; \n");
    fprintf(fp, "padding: .2rem;    \n");
    fprintf(fp, "text-align: center;\n");
    fprintf(fp, "} \n");

    fprintf(fp, ".stalisttbl td a { \n");
    fprintf(fp, "font-weight: bolder; \n");
    fprintf(fp, "font-size: 80%; \n");
    fprintf(fp, "} \n");

    fprintf(fp, ".resultstbl td, .resultstbl th { \n");
    fprintf(fp, "text-align: right; \n");
    fprintf(fp, "font-size: smaller;\n");
    fprintf(fp, "padding: 3px;\n");
    fprintf(fp, "} \n");

    fprintf(fp, ".resultstbl tr:hover { \n");
    fprintf(fp, "font-weight: 900; \n");
    /* fprintf(fp, "font-size: 103%;  \n"); */
    fprintf(fp, "} \n");

    fprintf(fp, ".resultstbl a.broken:visited { \n");
    fprintf(fp, "color: #fff; \n");
    fprintf(fp, "} \n");

    fprintf(fp, ".resultstbl a { \n");
    fprintf(fp, "color: #000; \n");
    fprintf(fp, "} \n");

    fprintf(fp, "</style>\n");
}

static void PrintChnPgLegend(FILE *fp)
{
char *phrase[NumColors];
int  percent[NumColors];
int bgcolor[NumColors];
int fgcolor[NumColors];
int  i=0;
int  j=0;

    fprintf(fp, "<TABLE class=\'legendtbl table table-sm\'>\n");
    fprintf(fp, "<thead><tr><TH COLSPAN=\"6\">Latency Indicators</TH></tr></thead>\n");
    fprintf(fp, "<tr>");
    for (i = 0; ColorMap[i].value >= 0; i++) {
      if (GetBg(i) > 0) {
        /* if (ColorMap[i].value / 432000) { phrase[i] = "gt;5 days"; */
        /* } else if (ColorMap[i].value / 345600) { phrase[i] = "gt;4 days"; */
        /* } else if (ColorMap[i].value / 259200) { phrase[i] = "gt;3 days"; */
        /* } else if (ColorMap[i].value / 172800) { phrase[i] = "gt;2 days"; */
        if (ColorMap[i].value / 86400)  { phrase[i] = "gt;1 day";
        } else if (ColorMap[i].value / 21600)  { phrase[i] = "gt;6 hrs";
        } else if (ColorMap[i].value / 7200)   { phrase[i] = "gt;2 hrs";
        } else if (ColorMap[i].value / 3600)   { phrase[i] = "gt;=1 hr";
        } else {
            phrase[i] = "lt;=1 hr";
        }
        bgcolor[i] = GetBg(ColorMap[i].value);
        fgcolor[i] = GetFg(ColorMap[i].value);
        percent[i] = GetPercent(ColorMap[i].value);
      }
    }
    fprintf(fp, "<td bgcolor=\"%06x\"><b>%s</b></td>", HAPPY, "not latent");
    for (j = (NumColors-1); j >= 0; j--) {
        fprintf(fp, "<td bgcolor=\"%06x\"><FONT COLOR=\"%06x\"><B>&%s</B></TD>\n",
             bgcolor[j],fgcolor[j],phrase[j]);
    }
    fprintf(fp, "</tr>");
    fprintf(fp, "</TABLE></CENTER>");
}

static void PrintStatusLegend(FILE *fp)
{
char *phrase[NumColors];
char *nsegphrase[NumColors];
int  percent[NumColors];
int bgcolor[NumColors];
int fgcolor[NumColors];
int  i=0;
int  j=0;

    fprintf(fp, "<TABLE class=\'legendtbl table table-sm\'>\n");
    fprintf(fp, "<thead><tr><TH COLSPAN=\"6\">Latency Indicators</TH></tr></thead>\n");
    fprintf(fp, "<tbody><tr>");
    for (i = 0; ColorMap[i].value >= 0; i++) {
      if (GetBg(i) > 0) {
        if (ColorMap[i].value / 86400)  {
            phrase[i] = "gt;1 day";
        } else if (ColorMap[i].value / 21600)  {
            phrase[i] = "gt;6 hrs";
        } else if (ColorMap[i].value / 7200)   {
            phrase[i] = "gt;2 hrs";
        } else if (ColorMap[i].value / 3600)   {
            phrase[i] = "gt;1 hr";
        } else {
            phrase[i] = "lt;=1 hr";
        }
        bgcolor[i] = GetBg(ColorMap[i].value);
        fgcolor[i] = GetFg(ColorMap[i].value);
        percent[i] = GetPercent(ColorMap[i].value);
      }
    }
    fprintf(fp, "<td style=\"width:%f%%\" bgcolor=\"%06x\"><b>&%s</b></td>", round(100/(NumColors+1)), HAPPY, "not latent");
    for (j = (NumColors-1); j >= 0; j--) {
        fprintf(fp, "<td style=\"width:%f%%\" bgcolor=\"%06x\"><FONT COLOR=\"%06x\"><b>&%s</b></TD>\n",
             round(100/(NumColors+1)), bgcolor[j],fgcolor[j],phrase[j]);
    }
    fprintf(fp, "</tr>\n");

    for (i = 0; (i < NumColorsNseg); i++) {
         if (ColorMapNseg[i].value <= 10) {
              nsegphrase[i] = "lt;10";
              bgcolor[i] = ColorMapNseg[i].bg;
              fgcolor[i] = ColorMapNseg[i].fg;
         } else if (ColorMapNseg[i].value <= 50) {
              nsegphrase[i] = "lt;50";
              bgcolor[i] = ColorMapNseg[i].bg;
              fgcolor[i] = ColorMapNseg[i].fg;
         } else {
              nsegphrase[i] = "gt;50";
              bgcolor[i] = ColorMapNseg[i].bg;
              fgcolor[i] = ColorMapNseg[i].fg;
         }
    }


    fprintf(fp, "<tr><TH COLSPAN=\"6\">Segment Indicators</TH></tr>\n");
    fprintf(fp, "<tr>");
    for (j = 0; (j < NumColorsNseg-1); j++) {
         fprintf(fp, "<td COLSPAN=\"2\" bgcolor=\"%06x\"><FONT COLOR=\"%06x\"><B>&%s</B></TD>\n",
              bgcolor[j],fgcolor[j],nsegphrase[j]);
    }
    fprintf(fp, "<td COLSPAN=\"2\" bgcolor=\"%06x\"><FONT COLOR=\"%06x\"><B>&%s</B></TD>\n", bgcolor[NumColorsNseg-1],fgcolor[NumColorsNseg-1],nsegphrase[NumColorsNseg-1]);
    fprintf(fp, "</tr>");
    fprintf(fp, "</tbody>");
    fprintf(fp, "</table>\n");
}

static void PrintResultTableHeader(FILE *fp)
{
    fprintf(fp, "<div class=\'col\'>\n");
    fprintf(fp, "\n<TABLE class=\'resultstbl sortable table table-sm\'>");
    fprintf(fp, "\n<thead class=\'thead-dark\'><tr>\n");
    fprintf(fp, "<th style=\"width:18%\">STA-LOC</th><th style=\"width:15%\">LIVE CHN</th><th style=\"width:13%\">AVG NSEG</th><th style=\"width:27%\"> DATA LATENCY </th><th style=\"width:27%\"> LINK LATENCY </TH>\n");
    fprintf(fp, "</tr></thead>\n");
    fprintf(fp, "<tbody>\n");
}

static void PrintResultTableFooter(FILE *fp)
{
    fprintf(fp, "</tbody>\n");
    fprintf(fp, "<thead class=\'thead-dark\'><tr><th>STA-LOC</th><th>LIVE CHN</th><th>AVG NSEG</th><th> DATA LATENCY </th><th> LINK LATENCY </TH></tr></thead>\n");
    fprintf(fp, "</table>\n");

    fprintf(fp, "</div> <!-- col -->\n");

}

static void PrintStatusTrailer(FILE *fp)
{
    // close results table(s) row
    fprintf(fp, "</div> <!-- row -->\n");

    fprintf(fp, "<div class=\'row\'>\n");
    fprintf(fp, "<div class=\'col-10 offset-1\'>\n");
    PrintStatusLegend(fp);
    PrintTimeStamp(fp);
    fprintf(fp, "</div> <!-- col -->\n");
    fprintf(fp, "</div> <!-- row -->\n");

    fprintf(fp, "</BODY>\n");
    fprintf(fp, "</html>\n");
}
static void PrintStatusHeader(FILE *fp, char *isiserver, char *htdocpath, char *webstuff)
{
char *parchstr="parchment.jpg";
char parchvar[MAXPATHLEN+1];
parchvar[0]=0;

    fprintf(fp, "<!DOCTYPE html>\n<html>\n<HEAD><META HTTP-EQUIV=\"Refresh\" CONTENT=\"60\">\n");
    fprintf(fp, "<title>IDA/NRTS (%s)</title>\n",isiserver);

    fprintf(fp, "<!-- Bootstrap core CSS -->");
    fprintf(fp, "   <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/css/bootstrap.min.css\" integrity=\"sha384-9gVQ4dYFwwWSjIDZnLEWnxCjeSWFphJiwGPXr1jddIhOegiu1FwO5qRGvFXOdJZ4\" crossorigin=\"anonymous\">\n");
    fprintf(fp, "</HEAD>\n");


    sprintf(parchvar,"https://%s/%s",webstuff,parchstr);
    fprintf(fp, "<BODY background=\"%s\">\n",parchvar);
    fprintf(fp, "<script src=/web/js/sorttable.js></script>\n");
    fprintf(fp, "<DIV class=\'container-fluid\'> <!-- container -->");
    fprintf(fp, "<div class=\'row\'>\n");
    fprintf(fp, "<div class=\'col-12 \'>\n");
    fprintf(fp, "<h3 class=\"text-center\">IDA/NRTS Status at %s</h3>\n", isiserver);

    PrintTimeStamp(fp);
    PrintStatusLegend(fp);

    fprintf(fp, "</div> <!-- col -->\n");
    fprintf(fp, "</div> <!-- row -->\n");
//    Accommodate multiple color links
    MakeLinkColorSS(fp);

    fprintf(fp, "\n<br/>\n");

    // open row for results table column(s)
    fprintf(fp, "<div class=\'row\'>\n");
}

static void PrintStatusResult(FILE *fp,
        char *sta, char *loc, char *isiserver, char *webserver, char *webpath, char *stationdir,
        REAL64 tols, REAL64 tslw, REAL64 allowed_latency, UINT32 livechn, UINT32 totchn, UINT32 nseg)
{
UINT32 data_latency;
UINT32 data_bg;
/* UINT32 data_fg = BLACK; */
UINT32 link_latency;
UINT32 link_bg;
/* UINT32 link_fg = BLACK; */
char tbuf[1024];

char *str2 = "chan.html";
char stahomelink[MAXPATHLEN+1];
char chanlink[MAXPATHLEN+1];
int i;
stahomelink[0]=0;
chanlink[0]=0;

    sprintf(stahomelink, "https://%s/?q=station/%s", webserver, util_lcase(sta));
    sprintf(chanlink, "https://%s/%s/%s/%s-%s-%s", webserver, webpath, isiserver, util_ucase(sta), util_ucase(loc), str2);

    fprintf(fp, "<tr  class=\'text-monospace\'>");
    if (tols != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
        data_latency = time(NULL) - (UINT32) tols;
        link_latency = (UINT32) tslw;
        data_bg = (data_latency <= allowed_latency) ? HAPPY : GetBg(data_latency);
        link_bg = (tslw <= allowed_latency) ? HAPPY : GetBg(tslw);

        fprintf(fp, "<td bgcolor=\"%06x\">", data_bg);
        if (stationdir != NULL) {
            fprintf(fp, "\n<a href=\"%s\"><b>%s-%s</b></a>\n", stahomelink, sta, loc);
        } else {
            fprintf(fp, "<b>%s-%s</b>", sta, loc);
        }
        fprintf(fp, "</td>\n");
        if (livechn == totchn) {
            fprintf(fp, "<td bgcolor=\"%06x\"><a href=\"%s\">%i/%i</a></td>", HAPPY, chanlink, livechn, totchn);
        } else {
            fprintf(fp, "<td bgcolor=\"%06x\"><a href=\"%s\"><font color=\"%06x\"><b>%i/%i</b></a></td>",
                    RED, chanlink, WHITE, livechn, totchn);
        }
        fprintf(fp, "<td bgcolor=\"%06x\">\n",GetNsegbg(nseg));
        fprintf(fp, "<font color=\"%06x\">%i</td>\n",Getnsegfg(nseg),nseg);
        fprintf(fp, "<td bgcolor=\"%06x\">%s</td>\n", data_bg, utilLttostr((INT32)data_latency,8,tbuf));
        fprintf(fp, "<td bgcolor=\"%06x\">%s</td>\n", link_bg, utilLttostr((INT32)tslw,8,tbuf));
    } else {
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">",DKGRAY,WHITE);
        if (stationdir != NULL) {
            fprintf(fp, "\n<A HREF=\"%s\" class=\"broken\">",stahomelink);
            fprintf(fp, "%s-%s</A></font></td>\n", sta, loc);        }
        else {
            fprintf(fp,"\n%s</font></td>",sta);
        }
        fprintf(fp, "<td bgcolor=\"%06x\">\n",DKGRAY);
        fprintf(fp, "<A HREF=\"%s\">",chanlink);
        fprintf(fp, "<font color=\"%06x\">%i</font></a></td>\n",WHITE,livechn);
        fprintf(fp, "<td>n/a</td>\n");
        /* fprintf(fp, "<td>n/a</td>\n"); */
        fprintf(fp, "<td>n/a</td>\n");
        fprintf(fp, "<td>n/a</td>\n");
    }
    fprintf(fp, "</tr>\n");
}

static REAL32 CalcPacketLatencyThreshold(ISI_STREAM_SOH *chn_soh, ISI_STREAM_CNF *chn_cnf, UINT16 samples_per_packet)
{
    /* Calculate and return the number of seconds after which a packet is considered late */

    REAL32 packet_live_threshold;
    REAL32 packet_delay; // time required to fill a packet at a given sample rate
    REAL32 delay_constant = 15.0; // minimum acceptable delay in seconds for any packet */
    REAL32 delay_factor = 1.01;   // Additional latency 'allowance' as a multiplier of the time it takes to fill a packet channel late */

    // how long does it taek to fill this packet...
    packet_delay = samples_per_packet * isiSrateToSint(&chn_cnf->srate); // use util func to pull sample rate out of ISI_SRATE struct

    // crude hack to deal with triggered EN? channels
    if (chn_soh->name.chn[0] == 'E') {
        // E?? channels should have a short pulse once a day, so should never be more than 24 hours latent
        packet_live_threshold = 24 * 60 * 60.0;
    } else {
        packet_live_threshold = packet_delay * delay_factor + delay_constant;
    }

    return packet_live_threshold;
}

static void PrintStatus(FILE *fp, ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf, char *isiserver, char *webserver, char *htdocpath, char *webpath, char *stationdir)
{
int i;
int livechn,totchn;
int sumnseg,totnseg, nsegval;
REAL64 tols;
REAL64 tslw;
REAL64 pktlat;
REAL64 allowed_latency; // this is the maximum 'live' latency for each sta-loc group
                        //overall based on IDA10 packets and channel srates
                        // this goes down with higher sample rate
UINT32 nseg;
char *sta;
char *loc;
BOOL two_cols = FALSE;
int col_ndx = 0;
int sta_loc_cnt;
int sta_loc_ndx = 0;
BOOL new_sta = TRUE;
BOOL new_loc = TRUE;
static char *Blank = "    ";
static BOOL First = TRUE;
char webstuff[MAXPATHLEN+1];
webstuff[0]=0;

    sta_loc_ndx = 0;
    sta_loc_cnt = 0;
    sta = Blank;
    loc =  Blank;
    for (i = 0; i < soh->nentry; i++) {
        new_sta = strcmp(sta, soh->entry[i].name.sta) != 0;
        new_loc = strcmp(loc, soh->entry[i].name.loc) != 0;
        if (new_sta || new_loc) {
            sta_loc_cnt++;
            sta = soh->entry[i].name.sta;
            loc = soh->entry[i].name.loc;
        }
    }
    two_cols = (sta_loc_cnt >  40);

    sprintf(webstuff,"%s/%s",webserver,webpath);
    PrintStatusHeader(fp, isiserver, htdocpath, webstuff);
    PrintResultTableHeader(fp);
    sta = Blank;
    loc =  Blank;
    for (i = 0; i < soh->nentry; i++) {
        new_sta = strcmp(sta, soh->entry[i].name.sta) != 0;
        new_loc = strcmp(loc, soh->entry[i].name.loc) != 0;
        if (new_sta || new_loc) {
            sta_loc_ndx++;
            if (!First) {
                nsegval = (totnseg > 0) ? (sumnseg / totnseg) : 0;
                PrintStatusResult(fp,
                        sta, loc, isiserver, webserver, webpath,
                        stationdir, tols, tslw, allowed_latency, livechn, totchn, nsegval);
                if (two_cols && (col_ndx == 0) && (sta_loc_ndx > sta_loc_cnt/2)) {
                    col_ndx = 1;;
                    PrintResultTableFooter(fp);
                    PrintResultTableHeader(fp);
                }
            }
            sta = soh->entry[i].name.sta;
            loc = soh->entry[i].name.loc;
            tols = (REAL64) ISI_UNDEFINED_TIMESTAMP;
            tslw = (REAL64) ISI_UNDEFINED_TIMESTAMP;
            nseg = 0;
            livechn = 0;
            totchn = 0;
            totnseg = 0;
            sumnseg = 0;
            allowed_latency = INFINITY;
        }
        if (soh->entry[i].tols.value != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
            if (tols == (REAL64) ISI_UNDEFINED_TIMESTAMP || soh->entry[i].tols.value > tols) {
                tols = soh->entry[i].tols.value;
            }
        }
        nseg = soh->entry[i].nseg;
        sumnseg = sumnseg + nseg;

        if (soh->entry[i].tslw != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
            if (tslw == (REAL64) ISI_UNDEFINED_TIMESTAMP || soh->entry[i].tslw < tslw) {
                tslw = soh->entry[i].tslw;
            }
        } else {
            printf("Channel tslw undefind:%s-%s: %s\n", soh->entry[i].name.sta, soh->entry[i].name.loc, soh->entry[i].name.chn);
        }
        First = FALSE;

        if ((pktlat = CalcPacketLatencyThreshold(&soh->entry[i], &cnf->entry[i], SAMP_PER_PACKET)) < allowed_latency) allowed_latency = pktlat;

        /* if ((soh->entry[i].nrec != 0) && ((UINT32) soh->entry[i].tslw <= pktlat)) { */
        if ((UINT32) soh->entry[i].tslw <= pktlat) livechn++; // if channel is reporting, count it
        totchn++;    // count total channels

        if (nseg > 0) totnseg++;    // count total nseg if value > 0
    }

    nsegval = (totnseg > 0) ? (sumnseg / totnseg) : 0;

    PrintStatusResult(fp, sta, loc, isiserver, webserver, webpath, stationdir, tols, tslw, allowed_latency, livechn, totchn, nsegval);
    PrintResultTableFooter(fp);
    PrintStatusTrailer(fp);
}

static int FindStaIndex(LNKLST *slist, char *sta, char *loc)
{
int i;
char *name;
char staloc[MAXPATHLEN];

    sprintf(staloc, "%s-%s", sta, loc);
    for (i = 0; i < slist->count; i++) {
        name = (char *) slist->array[i];
        if (!strcmp(name, staloc)) return i;
    }
    return -1;
}

static void MakeNextButtons(FILE *fp, LNKLST *slist, char *sta, char *loc, char *isiserver, char *webstuff)
{
int idx=0;
char *str2 = "chan.html";
char stalocchanfn[MAXPATHLEN+1];
char *prevstaloc, *nextstaloc;
stalocchanfn[0]=0;

    if ((idx = FindStaIndex(slist, sta, loc)) < 0) {
         perror("MakeNextButtons");
         exit(0);
    }
    if (idx == 0) {
        prevstaloc = (char *) slist->array[slist->count-1];
    } else {
        prevstaloc = (char *) slist->array[idx-1];
    }
    sprintf(stalocchanfn,"https://%s/%s/%s-%s",webstuff,isiserver,prevstaloc,str2);
    fprintf(fp,"<p  class=\"text-center font-weight-bold\"><A HREF=\"%s\" class=\"working\"><== %s </A> | \n",stalocchanfn, prevstaloc);
    if (idx == ((slist->count)-1)) {
        nextstaloc = (char *) slist->array[0];
    } else {
        nextstaloc = (char *) slist->array[idx+1];
    }
    stalocchanfn[0] = 0;
    sprintf(stalocchanfn,"https://%s/%s/%s-%s",webstuff,isiserver,nextstaloc,str2);
    fprintf(fp,"<A HREF=\"%s\" class=\"working\"> %s ==></A></p>\n",stalocchanfn, nextstaloc);
}

static void MakeChanPageHeader(FILE *fp, LNKLST *slist, char *isiserver, char *htdocpath, char *webpath, char *webstuff, char *sta, char *loc)
{
char *name;
int i;
char *parchstr="parchment.jpg";
char parchvar[MAXPATHLEN+1];
parchvar[0]=0;
char *str2 = "chan.html";
char stalocchanfn[MAXPATHLEN+1];
char weblink[MAXPATHLEN+1];
weblink[0]=0;
stalocchanfn[0]=0;

    sprintf(weblink, "%s/%s",webstuff,isiserver);
    fprintf(fp, "<!DOCTYPE html><html><HEAD><META HTTP-EQUIV=\"Refresh\" CONTENT=\"60\">\n");
    fprintf(fp, "<title>\nIDA/NRTS (%s)\n</title>\n",isiserver);
    fprintf(fp, "<!-- Bootstrap core CSS -->");
    fprintf(fp, "   <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.0/css/bootstrap.min.css\" integrity=\"sha384-9gVQ4dYFwwWSjIDZnLEWnxCjeSWFphJiwGPXr1jddIhOegiu1FwO5qRGvFXOdJZ4\" crossorigin=\"anonymous\">\n");

    fprintf(fp, "\n</HEAD>\n");
    sprintf(parchvar,"https://%s/%s",webstuff,parchstr);
    fprintf(fp, "<BODY background=\"%s\">\n",parchvar);
    fprintf(fp, "<script src=/web/js/sorttable.js></script>\n");
    fprintf(fp, "<DIV class=\'container\'> <!-- container -->");
    fprintf(fp, "<div class=\'row\'>\n");
    fprintf(fp, "<div class=\'col\'>\n");
    fprintf(fp, "<h3 class=\"text-center\">IDA/NRTS Status for %s-%s at %s</h3>\n", sta, loc, isiserver);
    PrintTimeStamp(fp);
    fprintf(fp, "<p class=\"text-center\"><A HREF=\"https://%s/index.html \" class=\"working\">Network Summary Page</A></p>\n", weblink);
    MakeNextButtons(fp, slist, sta, loc, isiserver, webstuff);
    fprintf(fp, "</div> <!-- end column-->");
    fprintf(fp, "</div> <!-- end upper row -->");

//    Print links to the other stations' pages here
    fprintf(fp, "<div class=\'row\'>\n");
    fprintf(fp, "<div class=\'col-12\'>");
    fprintf(fp, "<TABLE class=\'stalisttbl table table-sm\'>");
    int col;
    int colcnt = 9;
    int row;
    int rowcnt = ceil(slist->count / (float)colcnt);
    for (row=0; row < rowcnt; row++) {
        /* name = (char *) slist->array[ndx]; */
        /* sprintf(stalocchanfn,"https://%s/%s-%s",weblink,name,str2); */
        /* fprintf(fp, "<tr><td><a href=\"%s\">%s</a></td></tr>\n",stalocchanfn,name); */

        for (col=0; col < colcnt; col++) {
            int ndx = col * rowcnt + row;
            if (ndx < slist->count) {
                name = (char *) slist->array[ndx];
                sprintf(stalocchanfn,"https://%s/%s-%s",weblink,name,str2);
                fprintf(fp, "<TD><A HREF=\"%s\">%s</A></TD>\n",stalocchanfn,name);
            } else {
                fprintf(fp, "<td></td>");
            }
        }

        fprintf(fp, "\n</TR>\n");
    }
    fprintf(fp, "</TABLE>");
    fprintf(fp, "</div> <!-- end column-->");
    fprintf(fp, "</div> <!-- end station index table row -->");

    fprintf(fp, "\n<br/>\n");

//    Accommodate multiple color links
    MakeLinkColorSS(fp);

    fprintf(fp, "<div class=\'row\'>\n");
    fprintf(fp, "<div class=\'col-12\'>\n");

    PrintChnPgLegend(fp);

    fprintf(fp, "<TABLE class=\'resultstbl sortable table table-sm\'>");
    fprintf(fp, "<thead class=\'thead-dark\'><tr><th>STA</th><th>CHAN</th><th>RATE</th><th>NSEG</th><th>LAST DATUM</th><th>DATA LATENCY</TH><TH>LINK LATENCY</B></th></tr></thead>\n");
    fprintf(fp, "<tbody>");
}

static void MakeChanPageTrailer(FILE *fp, LNKLST *slist, char *sta, char *loc, char *isiserver, char *webstuff)
{
    fprintf(fp, "</tbody>");
    fprintf(fp, "<thead class=\'thead-dark\'><tr><th>STA</th><th>CHAN</th><th>RATE</th><th>NSEG</th><th>LAST DATUM</th><th>DATA LATENCY</TH><TH>LINK LATENCY</B></th></tr></thead>\n");

    fprintf(fp, "</table>\n");
    /* fprintf(fp,"<BR>\n"); */
    PrintChnPgLegend(fp);
    /* fprintf(fp,"<BR>\n"); */
    MakeNextButtons(fp, slist, sta, loc, isiserver, webstuff);
    fprintf(fp, "<p class=\"text-center\"><A HREF=\"https://%s/%s/index.html \" class=\"working\">Network Summary Page</A></p>\n", webstuff, isiserver);
    PrintTimeStamp(fp);
    fprintf(fp, "</div> <!-- end column-->");
    fprintf(fp, "</div> <!-- main body and footer row -->");
    fprintf(fp, "</div> <!-- container -->");
    fprintf(fp, "</BODY>\n</html>\n");
}


static void MakeChanPageResult(FILE *fp, char *sta, char *chn, char *loc, REAL64 srate, UINT32 nseg, REAL64 tols, REAL64 tslw, REAL64 pktlat_threshold)
{
UINT32 latency, link_latency, data_latency;
UINT32 colorlatency;
UINT32 link_bg, data_bg;
ISI_SITECHANFLAG dest;
char tbuf[1024];
char chanloc[ISI_CHNLOCLEN+1];
chanloc[0]=0;
UINT32 samprate=0;


    samprate = SAMP_PER_PACKET / srate;
    fprintf(fp, "\n<tr class=\'text-monospace\'>");
    sprintf(chanloc,"%s%s",util_ucase(chn),loc);
    if (tols != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
        link_latency = (UINT32) tslw;
        data_latency = time(NULL) - (UINT32) tols;
        link_bg = (link_latency <= pktlat_threshold) ? HAPPY : GetBg(link_latency);
        data_bg = (data_latency <= pktlat_threshold) ? HAPPY : GetBg(data_latency);

        fprintf(fp, "<td bgcolor=\"%06x\">%s</td>\n", data_bg, sta);
        fprintf(fp, "<td bgcolor=\"%06x\">%s</td>\n", data_bg, chanloc);
        fprintf(fp, "<td bgcolor=\"%06x\">%4.2f</td>\n", data_bg, srate);
        fprintf(fp, "<td bgcolor=\"%06x\">%i</td>\n", GetNsegbg(nseg), nseg);

        if (tols != (REAL64) ISI_UNDEFINED_TIMESTAMP) {
            fprintf(fp,"<td>%s</td>\n", utilLttostr((INT32)tols, 0, tbuf));
            fprintf(fp,"<td bgcolor=\"%06x\">%s</td>\n", data_bg, utilLttostr((INT32)data_latency, 8, tbuf));
        } else {
            fprintf(fp, "<td></td>");
            fprintf(fp, "<td></td>");
        }
        fprintf(fp,"<td bgcolor=\"%06x\">%s</td>\n", link_bg, utilLttostr((INT32)tslw, 8, tbuf));
    } else {
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">%s</font></TD>", DKGRAY,WHITE,sta);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">%s</font></TD>", DKGRAY,WHITE,chanloc);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">%4.2f</font></td>\n", DKGRAY,WHITE,srate);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">%i</font></td>\n", DKGRAY,WHITE,nseg);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">n/a</font></td>\n", DKGRAY,WHITE);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">n/a</font></td>\n", DKGRAY,WHITE);
        fprintf(fp, "<td bgcolor=\"%06x\"><font color=\"%06x\">n/a</font></td>\n", DKGRAY,WHITE);
    }
    fprintf(fp, "</tr>\n");
}


static void AppendUniquePublicStationLoc(LNKLST *slist, char *sta, char *loc)
{
LNKLST_NODE *crnt;
static char prevstaloc[255] = "";
char newstaloc[255];
char *sname;

    /* crnt = listFirstNode(slist); */
    /* while (crnt != NULL) { */
    /*     sname = (char *) crnt->payload; */
    /*     if (strcasecmp(name, sname) == 0) return; */
    /*     crnt = listNextNode(crnt); */
    /* } */
    sprintf(newstaloc, "%s-%s", sta, loc);
    if (strcmp(newstaloc, prevstaloc) != 0) {
        if (!listAppend(slist, newstaloc, strlen(newstaloc)+1)) {
            perror("listAppend");
            exit(1);
        }
        sprintf(prevstaloc, "%s", newstaloc);
    }
}

static LNKLST *CreateStationLocList(ISI_SOH_REPORT *soh)
{
int i;
LNKLST *slist;

    if ((slist = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }
    for (i = 0; i < soh->nentry; i++) AppendUniquePublicStationLoc(slist, soh->entry[i].name.sta, soh->entry[i].name.loc);
    if (!listSetArrayView(slist)) {
        perror("listSetArrayView");
        exit(1);
    }

    for (i = 0; i < slist->count; i++) util_ucase((char *) slist->array[i]);

    return slist;
}

static FILE *OpenChanPageOutput(char *isiserver, char *sta, char *loc, char *htdocpath, char *webpath)
{
char *str2 = "chan.html";
char ofname[MAXPATHLEN+1];
char odirname[MAXPATHLEN+1];
FILE *ofp;

    sprintf(odirname,"%s/%s/%s", htdocpath, webpath, isiserver);
    if (!utilDirectoryExists(odirname)) {
        fprintf(stdout, "FATAL ERROR:   No output directory exists \n");
        exit(1);
    }
    sprintf(ofname, "%s/%s/%s/%s-%s-%s", htdocpath, webpath, isiserver, sta, loc, str2);
    if ((ofp = fopen(ofname, "w")) == NULL) {
        perror("OpenChanPageOutput");
    }
    return ofp;
}

static void MakeChanPage(FILE *fp, ISI_SOH_REPORT *soh, ISI_CNF_REPORT *cnf, char *isiserver, char *webserver, char *htdocpath, char *webpath)
{
int i;
REAL64 tols;
REAL64 tslw;
REAL64 ssrate;
REAL64 pktlat;
char ofname[MAXPATHLEN+1];
char webstuff[MAXPATHLEN+1];
webstuff[0]=0;
FILE *ofp;
static char *PrevSta, *PrevLoc, *Blank = "    ";
PrevSta = Blank;
PrevLoc = Blank;
ofname[0] = 0;
LNKLST *slist;

    slist = CreateStationLocList(soh);  // this is a list of upper case public stations

    sprintf(webstuff, "%s/%s", webserver,webpath);
    for (i = 0; i <= soh->nentry; i++) {
        if ((strcmp(PrevSta, soh->entry[i].name.sta) != 0) || (strcmp(PrevLoc, soh->entry[i].name.loc) != 0)) {                     //  new station or loc
            if (i > 0) {
                MakeChanPageTrailer(ofp, slist, PrevSta, PrevLoc, isiserver, webstuff);
                fclose(ofp);
                if (i == soh->nentry) continue;
            }
            ofp = OpenChanPageOutput(isiserver, soh->entry[i].name.sta, soh->entry[i].name.loc, htdocpath, webpath);
            MakeChanPageHeader(ofp, slist, isiserver, htdocpath, webpath, webstuff,
                               soh->entry[i].name.sta, soh->entry[i].name.loc);
            PrevSta = util_ucase(soh->entry[i].name.sta);
            PrevLoc = util_ucase(soh->entry[i].name.loc);
        }
        ssrate = 1.0 / isiSrateToSint(&cnf->entry[i].srate);
        pktlat = CalcPacketLatencyThreshold(&soh->entry[i], &cnf->entry[i], SAMP_PER_PACKET);
        MakeChanPageResult(ofp, soh->entry[i].name.sta, soh->entry[i].name.chn, soh->entry[i].name.loc, ssrate,
            soh->entry[i].nseg, soh->entry[i].tols.value, soh->entry[i].tslw, pktlat);
    }
    listDestroy(slist);

}
static void help(char *myname)
{
static char *VerboseHelp =
"The arguments in [ square brackets ] are optional:\n"
"\n"
"isiserver=string  default: idahub.ucsd.edu \n"
"webserver=string  default: ida.ucsd.edu \n"
"webpath=string    default: Telemetry \n"
"htdocpath=string  default: /ida/web \n"
"stationdir=string default: Stations \n"
;
    fprintf(stderr, "%s %s\n\n", myname, VersionIdentString);
    fprintf(stderr,"usage: %s ", myname);
    fprintf(stderr,"[ isiserver=string webserver=string webpath=string htdocpath=string ] ");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", VerboseHelp);
    exit(1);

}

int main (int argc, char **argv)
{
int i;
ISI_PARAM par;
BOOL verbose = FALSE;
ISI_SOH_REPORT *soh;
ISI_CNF_REPORT *cnf;
char *webpath =   DEFAULT_WEBPATH;
char *htdocpath = DEFAULT_HTDOCPATH;
char *isiserver = DEFAULT_SERVER;
char *webserver = DEFAULT_WEBSERVER;
char *stationdir = DEFAULT_STATIONDIR;
char *base[MAXPATHLEN+1];
char *Tpath;
char ofile[MAXPATHLEN+1];
FILE *ofp;
char *mainpage="index.html";

    utilNetworkInit();
    isiInitDefaultPar(&par);
    now = time(NULL);
    gmtime_r(&now, &gm.tm);
    asctime_r(&gm.tm, gm.buf);
    gm.buf[strlen(gm.buf)-1] = 0;

    putenv("TZ=America/Los_Angeles");
    tzset();
    localtime_r(&now, &local.tm);
    strftime(local.buf, 64, "%a, %Y-%m-%d (%j) %H:%M:%S (%Z)\n", &local.tm);
    putenv("TZ=");
    /* localtime_r(&now, &local.tm); */
    /* asctime_r(&local.tm, local.buf); */
    /* local.buf[strlen(local.buf)-1] = 0; */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "webserver=", strlen("webserver=")) == 0) {
            webserver = argv[i] + strlen("webserver=");
        } else if (strncmp(argv[i], "isiserver=", strlen("isiserver=")) == 0) {
            isiserver = argv[i] + strlen("isiserver=");
        } else if (strncmp(argv[i], "webpath=", strlen("webpath=")) == 0) {
            webpath = argv[i] + strlen("webpath=");
        } else if (strncmp(argv[i], "stationdir=", strlen("stationdir=")) == 0) {
            stationdir = argv[i] + strlen("stationdir=");
        } else if (strncmp(argv[i], "htdocpath=", strlen("htdocpath=")) == 0) {
            htdocpath = argv[i] + strlen("htdocpath=");
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else {
           fprintf(stderr, "%s: unexpected argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }
    if (verbose) fprintf(stdout, "%s %s\n", argv[0], VersionIdentString);

    if ((soh = isiSoh(isiserver, &par)) == NULL) {
        perror("isiSoh");
        exit(1);
    }
    if ((cnf = isiCnf(isiserver, &par)) == NULL) {
        perror("isiCnf");
        exit(1);
    }

    sprintf(ofile, "%s/%s/%s/%s",htdocpath,webpath,isiserver,mainpage);

    if ((ofp = fopen(ofile, "w")) == NULL) {
        fprintf(stdout,"Can\'t open %s/%s/%s/%s - FATAL ERROR \n",htdocpath,webpath,isiserver,mainpage);
        perror("Main");
        exit(1);
    }
    UcaseStationAndLoc(soh, cnf);
    SortSoh(soh);
    SortCnf(cnf);

    PrintStatus(ofp, soh, cnf, isiserver, webserver, htdocpath, webpath, stationdir);
    fclose(ofp);
    MakeChanPage(stdout, soh, cnf, isiserver, webserver, htdocpath, webpath);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.24  2015/12/08 21:18:22  dechavez
 * various minor tweaks to get clean Mac OS X builds
 *
 * Revision 1.23  2014/01/27 18:19:45  dechavez
 * Changes to accomodate move to new (NetOps) VM server.  Removed some commented debug code.
 *
 * Revision 1.22  2013/12/17 15:46:18  dechavez
 * Removed MySQL dependencies, added update timestamp to header
 *
 * Revision 1.21  2007/12/11 19:25:04  judy
 * added nseg to main page
 *
 * Revision 1.20  2007/10/03 21:14:00  judy
 * Cleaned up help detail.
 *
 * Revision 1.19  2007/06/20 18:49:34  judy
 * Fixed Safari browser incompatibilities
 *
 * Revision 1.18  2007/06/19 18:12:01  judy
 * Color for link latency in channel view independent of the other columns
 *
 * Revision 1.16  2007/06/11 22:23:38  judy
 * Replaced page1,2,3 design to station/channel pages only (no more "page 1").
 * User specifies webserver, isi server and htdocs path
 *
 * Revision 1.15  2007/06/04 19:14:47  judy
 * Added page 3 option (channel pages and nrts_mon style navigation between stations)
 *
 * Revision 1.14  2007/02/01 19:41:05  judy
 * Fixed parchment display
 *
 * Revision 1.13  2007/01/25 22:21:12  judy
 * Added links to FDSN station pages, used CSS style sheets for link font color
 *
 * Revision 1.12  2007/01/11 18:05:47  dechavez
 * reverted to 1.3.3 since 1.4.0 was prematurely committed
 *
 * Revision 1.11  2006/11/13 17:21:18  judy
 * added db option and used (new) system flags to select which stations to ignore
 *
 * Revision 1.10  2006/10/19 23:52:51  dechavez
 * Added server name to title, moved isiwww version down by the timestamp at the bottom of the page
 *
 * Revision 1.9  2006/10/19 23:09:46  judy
 * Added parchment background
 *
 * Revision 1.8  2006/08/17 19:50:32  judy
 * Re-title and add refresh every 40 seconds
 *
 * Revision 1.7  2006/08/16 21:44:33  judy
 * Adopt BUD style color conventions and added data latency and channel counts
 *
 * Revision 1.6  2005/10/19 23:25:16  dechavez
 * Show latencies of more than one hour and less than one day in orange
 *
 * Revision 1.5  2005/06/10 15:38:10  dechavez
 * Rename isiSetLogging() to isiStartLogging()
 *
 * Revision 1.4  2005/03/23 20:45:40  dechavez
 * Print UTC time in YYYY:DDD-HH:MM:SS format
 *
 * Revision 1.3  2005/02/23 17:29:48  dechavez
 * truncate seconds from page 1 display
 *
 * Revision 1.2  2005/02/23 01:42:08  dechavez
 * display local time correctly
 *
 * Revision 1.1  2005/02/23 00:41:23  dechavez
 * initial release
 *
 */
