#pragma ident "$Id: lmplatform.c,v 1.1 2014/07/10 20:30:42 dechavez Exp $"
/***************************************************************************
 * lmplatform.c:
 * 
 * Platform portability routines.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License (GNU-LGPL) for more details.  The
 * GNU-LGPL and further information can be found here:
 * http://www.gnu.org/
 *
 * Written by Chad Trabant, IRIS Data Management Center
 *
 * modified: 2010.304
 ***************************************************************************/

/* Define _LARGEFILE_SOURCE to get ftello/fseeko on some systems (Linux) */
#define _LARGEFILE_SOURCE 1

#include "lmplatform.h"


/***************************************************************************
 * lmp_ftello:
 *
 * Return the current file position for the specified descriptor using
 * the system's closest match to the POSIX ftello.
 ***************************************************************************/
off_t
lmp_ftello (FILE *stream)
{
#if defined(LMP_WIN32)
  return (off_t) ftell (stream);

#else
  return (off_t) ftello (stream);

#endif
}  /* End of lmp_ftello() */


/***************************************************************************
 * lmp_fseeko:
 *
 * Seek to a specific file position for the specified descriptor using
 * the system's closest match to the POSIX fseeko.
 ***************************************************************************/
int
lmp_fseeko (FILE *stream, off_t offset, int whence)
{
#if defined(LMP_WIN32)
  return (int) fseek (stream, (long int) offset, whence);
  
#else
  return (int) fseeko (stream, offset, whence);
  
#endif
}  /* End of lmp_fseeko() */


/* Revision History
 *
 * $Log: lmplatform.c,v $
 * Revision 1.1  2014/07/10 20:30:42  dechavez
 * moved from orginal mseed directory to make room for my own library with that name
 *
 *
 * --comments from when this was called libmseed--
 *
 * Revision 1.2  2014/03/04 19:06:16  dechavez
 * updated to libmseed 2.12 from IRIS
 *
 */
