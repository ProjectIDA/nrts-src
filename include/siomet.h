#pragma ident "$Id$"
/*
 * SIO met sensor support 
 */

#ifndef siomet_h_defined
#define siomet_h_defined

#include "platform.h"
#include "ttyio.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#define SIOMET_BUFLEN     80
#define SIOMET_DEFAULT_TO  5

/* Structure templates */

typedef struct {
    TTYIO *tty;
    LOGIO *lp;
    char buf[SIOMET_BUFLEN];
    BOOL debug;
} SIOMET;

/* Function return codes */

#define SIOMET_OK          0
#define SIOMET_TIMED_OUT  -1
#define SIOMET_CRC_ERROR  -2
#define SIOMET_BAD_STRING -3

/* Function prototypes */
 
/* siomet.c */
SIOMET *siometOpen(char *port, int speed, LOGIO *lp, BOOL debug);
BOOL siometRead(SIOMET *handle, REAL64 *Ta, REAL64 *Ua, REAL64 *Pa);
void siometClose(SIOMET *handle);
 
/* version.c */
char *siometVersionString(void);
VERSION *siometVersion(void);
 
 #ifdef __cplusplus
}
#endif
 
#endif /* siomet_h_included */
 
/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log$
 */
