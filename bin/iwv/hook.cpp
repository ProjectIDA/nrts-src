#pragma ident "$Id: hook.cpp,v 1.1 2010/05/11 18:25:00 akimov Exp $"
#include "stock.h"

Hook *new_hook (void (*hookfree)(void *) )
	{
    Hook *hook ;
 
	hook=new Hook();
    hook->magic = HOOK_MAGIC ; 
    hook->free_hook = hookfree ;
    hook->p = 0 ;
    return hook ;
	}

void free_hook (Hook **hookp )
	{
    Hook *hook ; 

    if ( hookp != 0 && (hook = (Hook *) *hookp) != 0 ) 
		{
		if ( hook->magic == HOOK_MAGIC ) 
			{
			if ( hook->p != 0 )
			delete hook->p; 
			delete hook; 
			*hookp = 0 ;
			} else 
			{
//		printf("free_hook called with bad pointer.\n" ) ; 
			}
		}
	}


/* Revision History
 *
 * $Log: hook.cpp,v $
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
