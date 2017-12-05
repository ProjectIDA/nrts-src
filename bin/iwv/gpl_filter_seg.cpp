#pragma ident "$Id: gpl_filter_seg.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "filter.h"


int gpl_filter_seg(double tstart, double  dt, int nsamps, float *data, struct gather_plot_filter_ *filter,
float *data_filtered)
	{
    int             i;
    double           data_mean = 0.;
    int             itaper;
    double           durstr,
                    durend,
                    tstap;
    float          dttap;
    int             nttap;
    int             ol,
                    ou;
    int             isave = 1;

    /* De-mean and taper the data. */
    /*data_mean = 0.0;*/
    /* for (i=0; i<nsamps; i++) data_mean += data[i]; data_mean /= nsamps; */
    for (i = 0; i < nsamps; i++)
	data_filtered[i] = (float) (data[i] - data_mean);
    itaper = 1;
    durstr = 0.8 * filter->start_pad;
    durend = 0.8 * filter->end_pad;
    tstap = 0.0;
    dttap = (float)dt;
    nttap = nsamps;
    /* taper_ (&itaper, &durstr, &durend, &tstap, &dttap, &nttap,
     * data_filtered); */
    /* Filter the data. */
    switch (filter->type)
      {
      case GPL_BUTTERWORTH:
		ol = (int)(filter->param2 + 0.5);
		ou = (int)(filter->param4 + 0.5);
		setbfl_(&filter->param1, &ol, &filter->param3, &ou, &dttap);
		inifil_(data_filtered);
		filrec_(&nttap, data_filtered, &isave, data_filtered);
		freefl_();
		break;
      default:
		break;
      }
    return (1);
}



int _open_filter(double  dt, struct gather_plot_filter_ *filter)
	{
    int             itaper;
    double           durstr,
                    durend,
                    tstap;
    float           dttap;
    int             ol,
                    ou;


    /* De-mean and taper the data. */
    /*data_mean = 0.0;*/
    /* for (i=0; i<nsamps; i++) data_mean += data[i]; data_mean /= nsamps; */
    itaper = 1;
    durstr = 0.8 * filter->start_pad;
    durend = 0.8 * filter->end_pad;
    tstap = 0.0;
    dttap = (float)dt;
    /* taper_ (&itaper, &durstr, &durend, &tstap, &dttap, &nttap,
     * data_filtered); */
    /* Filter the data. */
    switch (filter->type)
      {
      case GPL_BUTTERWORTH:
		ol = (int)(filter->param2 + 0.5);
		ou = (int)(filter->param4 + 0.5);
		setbfl_(&filter->param1, &ol, &filter->param3, &ou, &dttap);
		break;
      default:
		break;
      }
    return 1;
	}
void _init_filter(float *data)
	{
	inifil_(data);
	}
void _do_filter(float *data)
	{
    int isave = 1;
    int nttap = 1;
	filrec_(&nttap, data, &isave, data);
	}
void _close_filter()
	{
	freefl_();
	}

/* Revision History
 *
 * $Log: gpl_filter_seg.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */