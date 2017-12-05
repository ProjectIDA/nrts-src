#pragma ident "$Id: get.c,v 1.1 2013/03/07 21:01:11 dechavez Exp $"
/*======================================================================
 *
 *  Retrive parameters from the handle
 *
 *====================================================================*/
#include "sbd.h"

time_t sbdGetStartTime(SBD *sbd)
{
time_t retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return 0;
    }

    retval = sbd->connect;
    return retval;
}

int sbdGetDebug(SBD *sbd)
{
int retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return 0;
    }

    retval = sbd->debug;
    return retval;
}

UINT32 sbdGetTimeoutInterval(SBD *sbd)
{
UINT32 retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return 0xFFFFFFFF;
    }

    retval = sbd->attr.at_timeo;
    return retval;
}

SBD_ATTR *sbdGetAttr(SBD *sbd, SBD_ATTR *attr)
{
    if (sbd == NULL || attr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    *attr = sbd->attr;
    return attr;
}

UINT32 sbdGetRetryInterval(SBD *sbd)
{
UINT32 retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return 0xFFFFFFFF;
    }

    retval = sbd->attr.at_wait;
    return retval;
}

UINT32 sbdGetMaxError(SBD *sbd)
{
UINT32 retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return 0xFFFFFFFF;
    }

    retval = sbd->attr.at_maxerr;
    return retval;
}

BOOL sbdGetRetry(SBD *sbd)
{
BOOL retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    retval = sbd->attr.at_retry;
    return retval;
}

BOOL sbdGetDisabled(SBD *sbd)
{
BOOL retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    retval = sbd->disabled;
    return retval;
}

LOGIO *sbdGetLogio(SBD *sbd)
{
LOGIO *retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return NULL;
    }

    retval = sbd->lp;
    return retval;
}

int sbdGetSendError(SBD *sbd)
{
int retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return -1;
    }

    retval = sbd->error.send;
    return retval;
}

int sbdGetRecvError(SBD *sbd)
{
int retval;

    if (sbd == NULL) {
        errno = EINVAL;
        return -1;
    }

    retval = sbd->error.send;
    return retval;
}

/* Revision History
 *
 * $Log: get.c,v $
 * Revision 1.1  2013/03/07 21:01:11  dechavez
 * initial release
 *
 */
