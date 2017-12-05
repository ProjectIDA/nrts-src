#pragma ident "$Id: print.c,v 1.2 2015/03/06 21:44:14 dechavez Exp $"
/*======================================================================
 *
 *  Print Datascope formatted IDA database records
 *
 *====================================================================*/
#include "convertdb.h"

void PrintOldStage(FILE *fp, OLD_STAGE *stage)
{
    fprintf(fp, "%-6s",     stage->sta);
    fprintf(fp, " %-8s",    stage->chan);
    fprintf(fp, " %17.5f", stage->time);
    fprintf(fp, " %17.5f", stage->endtime);
    fprintf(fp, " %8d",    stage->stageid);
    fprintf(fp, " %-16s",   stage->ssident);
    fprintf(fp, " %10.5g", stage->gnom);
    fprintf(fp, " %-16s",   stage->iunits);
    fprintf(fp, " %-16s",   stage->ounits);
    fprintf(fp, " %10.6f", stage->gcalib);
    fprintf(fp, " %-20s",   stage->gtype);
    fprintf(fp, " %8d",    stage->izero);
    fprintf(fp, " %8d",    stage->decifac);
    fprintf(fp, " %11.7f", stage->samprate);
    fprintf(fp, " %11.7f", stage->leadfac);
    fprintf(fp, " %-64s",   stage->dir);
    fprintf(fp, " %-32s",   stage->dfile);
    fprintf(fp, " %17.5f", stage->lddate);
    fprintf(fp, "\n");
}

void PrintOldSeedloc(FILE *fp, OLD_SEEDLOC *seedloc)
{
    fprintf(fp, "%-6s",     seedloc->sta);
    fprintf(fp, " %-8s",    seedloc->chan);
    fprintf(fp, " %17.5f", seedloc->time);
    fprintf(fp, " %17.5f", seedloc->endtime);
    fprintf(fp, " %-8s",    seedloc->newchan);
    fprintf(fp, " %-2s",    seedloc->loccode);
    fprintf(fp, " %17.5f", seedloc->lddate);
    fprintf(fp, "\n");
}

void PrintOldSitechan(FILE *fp, OLD_SITECHAN *sitechan)
{
    fprintf(fp, "%-6s",     sitechan->sta);
    fprintf(fp, " %-8s",    sitechan->chan);
    fprintf(fp, " %8d",    sitechan->ondate);
    fprintf(fp, " %8d",    sitechan->chanid);
    fprintf(fp, " %8d",    sitechan->offdate);
    fprintf(fp, " %-4s",    sitechan->ctype);
    fprintf(fp, " %9.4f",  sitechan->edepth);
    fprintf(fp, " %6.1f",  sitechan->hang);
    fprintf(fp, " %6.1f",  sitechan->vang);
    fprintf(fp, " %-50s",   sitechan->descrip);
    fprintf(fp, " %17.5f", sitechan->lddate);
    fprintf(fp, "\n");
}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.2  2015/03/06 21:44:14  dechavez
 * initial production release
 *
 */
