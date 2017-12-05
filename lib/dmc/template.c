#pragma ident "$Id: template.c,v 1.2 2015/11/04 23:02:37 dechavez Exp $"
/*======================================================================
 *
 *  Manage MSRecord templates, kept in a per-stream linked list.
 *
 *====================================================================*/
#include "libmseed.h"

void mseedPrintCinfo(MSChannelInfo *cinfo, BOOL newline)
{
    fprintf(stderr, "cinfo 0x%08lx: ",   (unsigned long) cinfo);
    fprintf(stderr, "chn=%3s",          cinfo->chn);
    fprintf(stderr, ", loc='%2s'",      cinfo->loc);
    fprintf(stderr, ", nsint=%9llu",    cinfo->nsint);
    fprintf(stderr, ", srate=%8.4lf",   cinfo->srate);
    fprintf(stderr, ", options=0x%08x", cinfo->options);
	if (newline) fprintf(stderr, "\n");
}

void mseedPrintMsr(MSRecord *msr, BOOL newline)
{
    fprintf(stderr, "msr 0x%08lx: ",      (unsigned long) msr);
    fprintf(stderr, "network=%2s",       msr->network);
    fprintf(stderr, ", station=%4s",     msr->station);
    fprintf(stderr, ", channel=%3s",     msr->channel);
    fprintf(stderr, ", location=%2s",    msr->location);
    fprintf(stderr, ", samprate=%8.4lf", msr->samprate);
    fprintf(stderr, ", dataquality=%c",  msr->dataquality);
	if (newline) fprintf(stderr, "\n");
}

void mseedPrintTemplate(MSTemplate *template)
{
    fprintf(stderr, "MSTemplate 0x%08lx: ", (unsigned long) template);
    mseedPrintCinfo(&template->cinfo, FALSE);
    fprintf(stderr, " ");
    mseedPrintMsr(template->msr, TRUE);
}

void mseedDumpHandle(MSTemplateHandle *handle)
{
int count = 0;
LNKLST_NODE *crnt;

    if (handle == NULL) {
        fprintf(stderr, "mseedListTemplates: NULL input\n");
        return;
    }

    fprintf(stderr, "HANDLE 0x%08lx: ", (unsigned long) handle);
    fprintf(stderr, "nname='%s'", handle->nname);
    fprintf(stderr, ", sname='%s'", handle->sname);
    fprintf(stderr, ", qcode='%c'", handle->qcode);
    fprintf(stderr, ", reclen=%d",  handle->reclen);
    fprintf(stderr, ", drift=%g",  handle->drift);
    fprintf(stderr, ", threshold=(%d, %d)",  handle->clock.locked, handle->clock.suspect);
    fprintf(stderr, "\n");
    fprintf(stderr, "%d templates\n", handle->list->count);

    crnt = listFirstNode(handle->list);
    while (crnt != NULL) {
        mseedPrintTemplate((MSTemplate *) crnt->payload);
        crnt = listNextNode(crnt);
    }
}

static MSTemplate *LocateTemplate(MSTemplateHandle *handle, MSChannelInfo *cinfo)
{
MSTemplate *template;
LNKLST_NODE *crnt;

    crnt = listFirstNode(handle->list);
    while (crnt != NULL) {
        template = (MSTemplate *) crnt->payload;
//DEBUG//fprintf(stderr, "LocateTemplate: COMPARE A:"); mseedPrintCinfo(cinfo, TRUE);
//DEBUG//fprintf(stderr, "LocateTemplate: COMPARE B:"); mseedPrintCinfo(&template->cinfo, FALSE);
//DEBUG//if (memcmp(&template->cinfo, cinfo, sizeof(MSChannelInfo)) == 0) fprintf(stderr, " MATCH\n");
        if (memcmp(&template->cinfo, cinfo, sizeof(MSChannelInfo)) == 0) return template;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static MSTemplate *CreateTemplate(MSTemplateHandle *handle, MSChannelInfo *cinfo)
{
MSTemplate new;
struct blkt_1001_s Blkt1001;
struct blkt_100_s Blkt100;

/* initialize new template */

    new.cinfo = *cinfo;

    if ((new.msr = msr_init(NULL)) == 0) return NULL;

    ms_strncpclean(new.msr->network,  handle->nname,   2); /* same hard-coded lengths as libmseed */
    ms_strncpclean(new.msr->station,  handle->sname,   5); /*  "     "    "     "     "     "     */
    ms_strncpclean(new.msr->location, cinfo->loc,      2); /*  "     "    "     "     "     "     */
    ms_strncpclean(new.msr->channel,  cinfo->chn,      3); /*  "     "    "     "     "     "     */
    util_ucase(new.msr->network);
    util_ucase(new.msr->station);
    util_ucase(new.msr->location);
    util_ucase(new.msr->channel);
    new.msr->samprate    = cinfo->srate;
    new.msr->dataquality = handle->qcode;

/* add blockette 100 if requested */

    if (cinfo->options & LIBMSEED_OPTION_INCLUDE_BLOCKETTE100) {
        memset(&Blkt100, 0, sizeof(struct blkt_100_s));
        Blkt100.samprate = (float) (cinfo->srate * ((double) 1.0 - handle->drift));
        if (msr_addblockette(new.msr, (char *) &Blkt100, sizeof(struct blkt_100_s), 100, 0) == NULL) return NULL;
    }

/* add blockette 1001 if requested */

    if (cinfo->options & LIBMSEED_OPTION_INCLUDE_BLOCKETTE1001) {
        memset(&Blkt1001, 0, sizeof(struct blkt_1001_s));
        if (msr_addblockette(new.msr, (char *) &Blkt1001, sizeof(struct blkt_1001_s), 1001, 0) == NULL) return NULL;
    }

/* add this template to the list and request it back for sanity */

    if (!listAppend(handle->list, (void *) &new, sizeof(MSTemplate))) return NULL;

    return (LocateTemplate(handle, cinfo));
}

MSTemplate *mseedGetTemplate(MSTemplateHandle *handle, MSChannelInfo *cinfo)
{
MSTemplate *template;

//DEBUG//fprintf(stderr, "mseedGetTemplate: WANT:   "); mseedPrintCinfo(cinfo, TRUE);
    if ((template = LocateTemplate(handle, cinfo)) != NULL) {
//DEBUG//fprintf(stderr, "                  MATCH  0x%08x: ", template); mseedPrintTemplate(template);
        return template;
    } else {
//DEBUG//fprintf(stderr, "                  NOT FOUND\n");
        template = CreateTemplate(handle, cinfo);
//DEBUG//fprintf(stderr, "mseedGetTemplate: CREATE 0x%08x: ", template); mseedPrintTemplate(template);
    }
    return template;
}

MSTemplateHandle *mseedInitTemplates(char *nname, char *sname, char qcode, int reclen, double drift, MSClockQualityThreshold *clock)
{
MSTemplateHandle *handle;

    if ((handle = (MSTemplateHandle *) malloc(sizeof(MSTemplateHandle))) == NULL) return NULL;
    if ((handle->msr = msr_init(NULL)) == NULL) return NULL;
    if ((handle->mstg = mst_initgroup(NULL)) == NULL) return NULL;
    if ((handle->list = listCreate()) == NULL) return NULL;
    if ((handle->nname = strdup(nname)) == NULL) return NULL;
    if ((handle->sname = strdup(sname)) == NULL) return NULL;
    handle->qcode = qcode;
    handle->reclen = reclen;
    handle->drift = drift;
    if (clock != NULL) {
        handle->clock.locked = clock->locked;
        handle->clock.suspect = clock->suspect;
    } else {
        handle->clock.locked = LIBMSEED_DEFAULT_CLOCK_LOCKED_PERCENT;
        handle->clock.suspect = LIBMSEED_DEFAULT_CLOCK_SUSPECT_PERCENT;
    }

    return handle;
}

/* Revision History
 *
 * $Log: template.c,v $
 * Revision 1.2  2015/11/04 23:02:37  dechavez
 * cosmetic changes to keep OS X compiler from complaining
 *
 * Revision 1.1  2014/07/10 20:30:42  dechavez
 * moved from orginal mseed directory to make room for my own library with that name
 *
 *
 * --comments from when this was called libmseed--
 *
 * Revision 1.2  2014/04/14 18:52:06  dechavez
 * added blockette 100 support
 *
 * Revision 1.1  2014/03/06 18:50:24  dechavez
 * created (adapted from imseed)
 *
 */
