#pragma ident "$Id: strings.c,v 1.2 2015/03/06 21:44:15 dechavez Exp $"
/*======================================================================
 *
 *  Useful strings
 *
 *====================================================================*/
#include "convertdb.h"

char *sitechanstr(OLD_SITECHAN *sitechan)
{
static char buf[1024], timstr1[1024], timstr2[1024];

    sprintf(buf, "sta=%s, chan=%s, chanid=%d, time=%19.7lf=%s, endtime=%19.7lf=%s",
        sitechan->sta,
        sitechan->chan,
        sitechan->chanid,
        sitechan->begt,
        utilDttostr(sitechan->begt, 0, timstr1),
        sitechan->endt,
        utilDttostr(sitechan->endt, 0, timstr2)
    );

    return buf;
}

char *seedlocstr(OLD_SEEDLOC *seedloc)
{
static char buf[1024], timstr1[1024], timstr2[1024];

    sprintf(buf, "sta=%s, chan=%s, time=%19.7lf=%s, endtime=%19.7lf=%s, newchan=%s, loccode=%s",
        seedloc->sta,
        seedloc->chan,
        seedloc->time,
        utilDttostr(seedloc->time, 0, timstr1),
        seedloc->endtime,
        utilDttostr(seedloc->endtime, 0, timstr2),
        seedloc->newchan,
        seedloc->loccode
    );

    return buf;
}

char *stagestr(OLD_STAGE *stage)
{
static char buf[1024], timstr1[1024], timstr2[1024];

    sprintf(buf, "sta=%s, chan=%s, time=%s, endtime=%s, stage=%d",
        stage->sta,
        stage->chan,
        utilDttostr(stage->time, 0, timstr1),
        utilDttostr(stage->endtime, 0, timstr2),
        stage->stageid
    );

    return buf;
}

char *schanlocstr(OLD_SCHANLOC *schanloc)
{
static char buf[1024];

    sprintf(buf, "sta=%s, chan=%s", schanloc->sta, schanloc->chan);

    return buf;
}

char *instrumentstr(OLD_INSTRUMENT *instrument)
{
static char buf[1024];

    sprintf(buf, "inid=%d insname='%s' instype='%s'", instrument->inid, instrument->insname, instrument->instype);
    return buf;
}

/* Revision History
 *
 * $Log: strings.c,v $
 * Revision 1.2  2015/03/06 21:44:15  dechavez
 * initial production release
 *
 */
