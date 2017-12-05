#pragma ident "$Id: rwfdisc.c,v 1.4 2012/02/14 21:45:30 dechavez Exp $"
/*======================================================================
 *
 *  Read an entire wfdisc file into an array.  2.8 wfdiscs are detected
 *  and converted to 3.0 format silently and automatically.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "cssio.h"

INT32 rwfdisc(FILE *fp, struct cssio_wfdisc **output)
{
INT32 i, nrec = 0;

struct list {
    struct cssio_wfdisc wfdisc;
    struct list *next;
} head, *crnt, *new, *next;

struct cssio_wfdisc wfdisc, *array;

/*  Read entire file into linked list  */

    head.next = NULL;
    crnt      = &head;

    nrec = 0;
    while (rwfdrec(fp, &wfdisc) == 0) {
        new = (struct list *) malloc(sizeof(struct list));
        if (new == NULL) return -1;
        new->wfdisc = wfdisc;
        new->next   = NULL;
        crnt->next  = new;
        crnt        = crnt->next;
        ++nrec;
    }
    if (ferror(fp)) return -2;

/*  Copy from linked list into array  */

    array = (struct cssio_wfdisc *) malloc(nrec*sizeof(struct cssio_wfdisc));
    if (array == NULL) return -3;

    i = 0; crnt = head.next;
    while (crnt != NULL) {
        next = crnt->next;
        array[i++] = crnt->wfdisc;
        free(crnt);
        crnt = next;
    }
    if (i != nrec) return -4;

/*  Assign array to user provided pointer and return number of elements  */

    *output = array;
    return nrec;
}

/* Revision History
 *
 * $Log: rwfdisc.c,v $
 * Revision 1.4  2012/02/14 21:45:30  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2011/03/17 17:21:09  dechavez
 * changed all macros and constants to use names with CSSIO_ or cssio_ prefixes
 * in order to avoid conflicts with IDA DCC source code
 *
 * Revision 1.2  2005/05/25 22:36:49  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
