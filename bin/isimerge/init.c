#pragma ident "$Id: init.c,v 1.1 2010/09/10 22:56:31 dechavez Exp $"
/*======================================================================
 *
 *  Initialize a fresh SITE_PAR
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isimerge.h"

#define MY_MOD_ID ISI_MERGE_MOD_INIT

#define INITIAL_RETRY_INTERVAL 10
#define MAX_RETRY_INTERVAL 80

void InitializeSitePar(SITE_PAR *site)
{

/* raw, semaphore, and mutex get initialized once they are instanced in the list */

    site->name[0] = 0;
    site->server[0] = 0;
    site->beg = ISI_UNDEFINED_SEQNO;
    site->end = ISI_UNDEFINED_SEQNO;
    site->last = ISI_UNDEFINED_SEQNO;
    isiInitDefaultPar(&site->isi_param);
    site->first = TRUE;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.1  2010/09/10 22:56:31  dechavez
 * initial release
 *
 */
