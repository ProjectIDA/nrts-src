#pragma ident "$Id: version.c,v 1.19 2016/01/28 00:56:08 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "qdplus.h"

static VERSION version = {1, 4, 3};

/* qdplus library release notes

1.4.3   01/27/2016
        meta.c: add C2_EPD supprot to qdplusInitializeLCQMetaData()

1.4.2   12/05/2015
        decode.c, print.c, state.c: casts, format fixes and the like, all to calm OS X compiles

1.4.1   08/06/2012
        state.c: fixed incorrect argument sanity check in PrintState()

1.4.0   04/07/2011
        qdplus.h: replaced QDPLUS_DT_USER_AUX with QDPLUS_DT_USER_PAROSCI, defined QDPLUS_USERPKT
        decode.c: replaced qdplusDecodeUserAux with qdplusDecodeUser()+DecodeUserParosci()

1.3.1   10/02/2009
        meta.c: fixed bug causing qdplusReadMetaData() to fail if meta dir
           contained extraneous files

1.3.0   07/02/2009
        io.c: added qdplusReadgz()

1.2.0   02/23/2009
        print.c: added qdplusPrintPkt()

1.1.1   12/16/2008
        state.c: fixed empty qhlp state file bug

1.1.0   12/15/2008
        handle.c: set handle->state in qdplusCreateHandle()
        state.c: use clean/dirty flag in state file

1.0.0   05/17/2007
        Initial production release
  
 */

char *qdplusVersionString()
{
static char string[] = "qdplus library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qdplus library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qdplusVersion()
{
    return &version;
}
