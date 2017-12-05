#pragma ident "$Id: stock.h,v 1.1 2010/05/11 18:25:04 akimov Exp $"
#ifndef _stock_
#define _stock_


#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#include <string.h>

 
//typedef Tbl     Bitvector;
#define BITS_PER_INT   	(8*sizeof(int))


#define HOOK_MAGIC 814615491

typedef struct Hook {
    int magic ; 
    void (*free_hook)(void *) ;
    void *p ;
} Hook ; 

typedef struct Xlat {
        char *name ;
        int num ;
} Xlat ;

#define PL_(x) ( )
extern Hook *new_hook( void (*hookfree)(void *));
extern void free_hook(Hook **hookp);


#endif



/* Revision History
 *
 * $Log: stock.h,v $
 * Revision 1.1  2010/05/11 18:25:04  akimov
 * initial production release
 *
 */
