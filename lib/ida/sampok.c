#pragma ident "$Id: sampok.c,v 1.4 2012/02/14 20:15:23 dechavez Exp $"
/*======================================================================
 *
 *  Make sure number of samples is reasonable.
 *
 *====================================================================*/
#include "ida.h"

int ida_sampok(IDA_DHDR *dhead)
{
INT16 min_nsamp, max_nsamp;

    switch (dhead->form) {
      case S_UNCOMP:  
      case L_UNCOMP:
      case FP32_UNCOMP:
        min_nsamp = max_nsamp = dhead->nbytes / dhead->wrdsiz;
        break;
      case IGPP_COMP:
      case FP32_COMP:
        min_nsamp = (dhead->nbytes - 3) / 3;
        max_nsamp =  dhead->nbytes - 2;
        break;
      default:
        return 0;
    }

    return (dhead->nsamp >= min_nsamp && dhead->nsamp <= max_nsamp);

}

/* Revision History
 *
 * $Log: sampok.c,v $
 * Revision 1.4  2012/02/14 20:15:23  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2006/02/08 23:10:33  dechavez
 * ansi-style declaration
 *
 * Revision 1.2  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
