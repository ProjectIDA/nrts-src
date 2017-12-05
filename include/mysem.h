#pragma ident "$Id: mysem.h,v 1.1 2015/11/24 23:03:15 dechavez Exp $"
/*==============================================================================
 *
 * Portable anonymous semaphores
 *
 *============================================================================*/
#ifndef mysem_h_included
#define mysem_h_included
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MYSEM_SIGNATURE "This object has been correctly initialized"

typedef struct {
    int pipe[2];
    int value;
    pthread_mutex_t mutex;
    char nomonkeys[MAXPATHLEN+1];
} MYSEM;

/* Function prototypes */

int mysemInit(MYSEM *psem, int init);
int mysemPost(MYSEM *psem);
int mysemWait(MYSEM *psem);
int mysemTryWait(MYSEM *psem);
int mysemDestroy(MYSEM *psem);

#ifdef __cplusplus
}
#endif

#endif /* mysem_h_included */

/* Revision History
 *
 * $Log: mysem.h,v $
 * Revision 1.1  2015/11/24 23:03:15  dechavez
 * created
 *
 */
