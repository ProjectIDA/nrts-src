#pragma ident "$Id: wah.c,v 1.2 2015/12/04 22:58:46 dechavez Exp $"
/*======================================================================
 *
 *  Write an ascii SAC header
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"

int sacio_wah(fp, hdr)
FILE *fp;
struct sac_header *hdr;
{

/*  Force the header to contain those "mysterious LLNL set values"  */

    if (hdr->internal1 == -12345.0) hdr->internal1 = 2.0;
    if (hdr->internal4 == -12345)   hdr->internal4 = 6;
    if (hdr->internal5 == -12345)   hdr->internal5 = 0;
    if (hdr->internal6 == -12345)   hdr->internal6 = 0;
    if (hdr->unused27  == -12345)   hdr->unused27  = 0;

/*  Write the header  */

    clearerr(fp);

    fprintf(fp, SACWFCS, hdr->delta, hdr->depmin, hdr->depmax, hdr->scale, hdr->odelta);
    fprintf(fp, SACWFCS, hdr->b, hdr->e, hdr->o, hdr->a, hdr->internal1);
    fprintf(fp, SACWFCS, hdr->t0, hdr->t1, hdr->t2, hdr->t3, hdr->t4);
    fprintf(fp, SACWFCS, hdr->t5, hdr->t6, hdr->t7, hdr->t8, hdr->t9);
    fprintf(fp, SACWFCS, hdr->f, hdr->resp0, hdr->resp1, hdr->resp2, hdr->resp3);
    fprintf(fp, SACWFCS, hdr->resp4, hdr->resp5, hdr->resp6, hdr->resp7, hdr->resp8);
    fprintf(fp, SACWFCS, hdr->resp9, hdr->stla, hdr->stlo, hdr->stel, hdr->stdp);
    fprintf(fp, SACWFCS, hdr->evla, hdr->evlo, hdr->evel, hdr->evdp, hdr->unused1);
    fprintf(fp, SACWFCS, hdr->user0, hdr->user1, hdr->user2, hdr->user3, hdr->user4);
    fprintf(fp, SACWFCS, hdr->user5, hdr->user6, hdr->user7, hdr->user8, hdr->user9);
    fprintf(fp, SACWFCS, hdr->dist, hdr->az, hdr->baz, hdr->gcarc, hdr->internal2);
    fprintf(fp, SACWFCS, hdr->internal3, hdr->depmen, hdr->cmpaz, hdr->cmpinc, hdr->unused2);
    fprintf(fp, SACWFCS, hdr->unused3, hdr->unused4, hdr->unused5, hdr->unused6, hdr->unused7);
    fprintf(fp, SACWFCS, hdr->unused8, hdr->unused9, hdr->unused10, hdr->unused11, hdr->unused12);
    fprintf(fp, SACWICS, hdr->nzyear, hdr->nzjday, hdr->nzhour, hdr->nzmin, hdr->nzsec);
    fprintf(fp, SACWICS, hdr->nzmsec, hdr->internal4, hdr->internal5, hdr->internal6, hdr->npts);
    fprintf(fp, SACWICS, hdr->internal7, hdr->internal8, hdr->unused13, hdr->unused14, hdr->unused15);
    fprintf(fp, SACWICS, hdr->iftype, hdr->idep, hdr->iztype, hdr->unused16, hdr->iinst);
    fprintf(fp, SACWICS, hdr->istreg, hdr->ievreg, hdr->ievtyp, hdr->iqual, hdr->isynth);
    fprintf(fp, SACWICS, hdr->unused17, hdr->unused18, hdr->unused19, hdr->unused20, hdr->unused21);
    fprintf(fp, SACWICS, hdr->unused22, hdr->unused23, hdr->unused24, hdr->unused25, hdr->unused26);
    fprintf(fp, SACWICS, hdr->leven, hdr->lpspol, hdr->lovrok, hdr->lcalda, hdr->unused27);
    fprintf(fp, SACWCCS2, hdr->kstnm, hdr->kevnm);
    fprintf(fp, SACWCCS1, hdr->khole, hdr->ko, hdr->ka);
    fprintf(fp, SACWCCS1, hdr->kt0, hdr->kt1, hdr->kt2);
    fprintf(fp, SACWCCS1, hdr->kt3, hdr->kt4, hdr->kt5);
    fprintf(fp, SACWCCS1, hdr->kt6, hdr->kt7, hdr->kt8);
    fprintf(fp, SACWCCS1, hdr->kt9, hdr->kf, hdr->kuser0);
    fprintf(fp, SACWCCS1, hdr->kuser1, hdr->kuser2, hdr->kcmpnm);
    fprintf(fp, SACWCCS1, hdr->knetwk, hdr->kdatrd, hdr->kinst);

    return (ferror(fp)) ? -1 : 0;
}

/* Revision History
 *
 * $Log: wah.c,v $
 * Revision 1.2  2015/12/04 22:58:46  dechavez
 * simplified code by only checking fp for errors once, at the end
 *
 * Revision 1.1.1.1  2000/02/08 20:20:37  dec
 * import existing IDA/NRTS sources
 *
 */
