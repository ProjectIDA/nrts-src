#pragma ident "$Id: ReleaseNotes.c,v 1.14 2017/04/13 16:11:55 dechavez Exp $"

char *VersionIdentString = "Release 1.4.6";

/* Release notes

1.4.6  04/13/2017
       link with libqdp 3.14.4 to fix "too many servers" error after testing a disabled data port

1.4.5  01/26/2017
       link with libq330 3.7.6 to address re-introducing the double free bug in 3.7.5! 

1.4.4  01/24/2017
       link with libq330 3.7.5 to address bugs introduced in libq330 3.7.4

1.4.3  12/22/2016
       link with libq330 3.7.4 to address double free bug with bad config files

1.4.2  09/23/2016
       link with libqdp 3.13.6

1.4.1  09/07/2016
       link with libqdp 3.13.5

1.4.0  08/19/2016
       link with libqdp 3.13.3
       made -v (verbose) the default and added -q (quiet) option to suppress

1.3.2  08/15/2016
       link with libqdp 3.13.2, use qdpShutdown() instead of q330Deregister()

1.3.1  08/04/2016
       link with libqdp 3.13.1 and libq330 3.7.1

1.3.0  07/20/2016
       replaced maxtry=integer option with watchdog=secs
       link with libqdp 3.13.0 to address handshake failure modes not previously encountered
       and dealt with in a half-assed manner in release 1.2.1

1.2.1  06/23/2016
       added QDP_ERR_REJECTED and QDP_ERR_NODPMEM failure mode

1.2.0  06/15/2016
       Added support for debug options 4 and 5 (raw packet logging).
       Link with libqdp-3.12.0 to fix incorrect auth code handling bug

1.1.0  05/10/2016
       Print enabled channel list when verifying a data port

1.0.0  02/11/2016
       Initial release
*/

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2016 Regents of the University of California            |
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
