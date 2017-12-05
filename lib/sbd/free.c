#pragma ident "$Id: free.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 * 
 * Free SBD resources
 *
 *====================================================================*/
#include "sbd.h"

SBD *sbdFree(SBD *sbd)
{
    if (sbd == (SBD *) NULL) return (SBD *) NULL;
    if (sbd->attr.at_dbgpath != NULL) free(sbd->attr.at_dbgpath);
    free(sbd);

    return (SBD *) NULL;
}

/* Revision History
 *
 * $Log: free.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
