#pragma ident "$Id: globals.c,v 1.3 2012/02/14 20:22:33 dechavez Exp $"
/*======================================================================
 *
 * Declaration of xfer library globals
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "xfer.h"

int xfer_errno    = 0;
int _xfer_timeout = 60;
UINT32 xfer_nsend = 0;
UINT32 xfer_nrecv = 0;

union xfer_buffer _Xfer_Buffer;
char *Xfer_Buffer = (char *) &_Xfer_Buffer;
int Xfer_Buflen   = sizeof(_Xfer_Buffer);

/* Revision History
 *
 * $Log: globals.c,v $
 * Revision 1.3  2012/02/14 20:22:33  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.2  2006/02/09 20:01:01  dechavez
 * eliminated Xfer_DB
 *
 * Revision 1.1.1.1  2000/02/08 20:20:43  dec
 * import existing IDA/NRTS sources
 *
 */
