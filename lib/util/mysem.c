#pragma ident "$Id: mysem.c,v 1.2 2016/08/01 21:44:33 dechavez Exp $"
/*======================================================================
 *
 * anonymous semaphore operations for those who lack them (eg, Darwin).
 *
 *====================================================================*/
#include "platform.h"
#include "mysem.h"

int mysemInit(MYSEM *psem, int init)
{
int i, flags;

    if (psem == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strncmp(psem->nomonkeys, MYSEM_SIGNATURE, strlen(MYSEM_SIGNATURE)) == 0) {
        errno = EALREADY;
        return -1;
    }

    pthread_mutex_init(&psem->mutex, NULL);
    psem->value = init;
    pipe(psem->pipe);

    for (i = 0; i < 2; i++) {
        flags = fcntl(psem->pipe[i], F_GETFD, 0);
        flags |= FD_CLOEXEC;
        fcntl(psem->pipe[i], F_SETFD, flags);
    }

    strcpy(psem->nomonkeys, MYSEM_SIGNATURE);

    return 0;
}

int mysemPost(MYSEM *psem)
{
int retval;
const char buf = 1;

    if (psem == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strncmp(psem->nomonkeys, MYSEM_SIGNATURE, strlen(MYSEM_SIGNATURE)) != 0) {
        errno = ENXIO;
        return -1;
    }

    pthread_mutex_lock(&psem->mutex);
        if (psem->value++ < 0) {
            retval = (write(psem->pipe[1], &buf, 1) == 1) ? 0 : -1;
        } else {
            retval = 0;
        }
    pthread_mutex_unlock(&psem->mutex);
    
    return retval;
}

int mysemWait(MYSEM *psem)
{
char buf;
int retval, value;

    if (psem == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strncmp(psem->nomonkeys, MYSEM_SIGNATURE, strlen(MYSEM_SIGNATURE)) != 0) {
        errno = ENXIO;
        return -1;
    }

    pthread_mutex_lock(&psem->mutex);
        value = --psem->value;
    pthread_mutex_unlock(&psem->mutex);

    if (value < 0) {
        retval = (read(psem->pipe[0], &buf, 1) == 1) ? 0 : -1;
    } else {
        retval = 0;
    }

    return retval;
}

int mysemTryWait(MYSEM *psem)
{
int retval;
char buf;

    if (psem == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strncmp(psem->nomonkeys, MYSEM_SIGNATURE, strlen(MYSEM_SIGNATURE)) != 0) {
        errno = ENXIO;
        return -1;
    }

    pthread_mutex_lock(&psem->mutex);
        if (psem->value > 0) {
            --psem->value;
            retval = 0;
        } else {
            errno = EAGAIN;
            retval = -1;
        }
    pthread_mutex_unlock(&psem->mutex);

    return retval;
}

int mysemDestroy(MYSEM *psem)
{
int i;

    if (psem == NULL) {
        errno = EINVAL;
        return -1;
    }

    for (i = 0; i < 2; i++) if (close(psem->pipe[i]) != 0) return -1;
    return 0;
}

/* Revision History
 *
 * $Log: mysem.c,v $
 * Revision 1.2  2016/08/01 21:44:33  dechavez
 * indented code inside mutex locked region, for clarity and consistency
 *
 * Revision 1.1  2015/11/24 23:06:14  dechavez
 * created
 *
 */
