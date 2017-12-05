#pragma ident "$Id: main.c,v 1.1 2016/02/19 17:57:20 dechavez Exp $"
/*======================================================================
 *
 *  semaphore portability test code
 *
 *====================================================================*/
#include "util.h"

static LOGIO *lp = NULL;

static THREAD_FUNC TestThread(void *argptr)
{
char ident[MAXPATHLEN+1];
static int count = 0;
SEMAPHORE *psem;

    ++count;
    sprintf(ident, "thread%d", count);
    psem = (SEMAPHORE *) argptr;

    while (1) {
        fprintf(stderr, "     %s: sleep(%d)\n", ident, count);
        sleep(count);
        fprintf(stderr, "     %s: SEM_POST()\n", ident);
        SEM_POST(psem);

        pause();
    }
}

int main(int argc, char **argv)
{
THREAD tid;
SEMAPHORE sem1, sem2;

    SEM_INIT(&sem1, 0, 1);
    fprintf(stderr, "main: launching first thread\n");

    if (!THREAD_CREATE(&tid, TestThread, (void *) &sem1)) {
        perror("THREAD_CREATE");
        exit(1);
    }

    SEM_INIT(&sem2, 0, 1);
    fprintf(stderr, "main: launching second thread\n");

    if (!THREAD_CREATE(&tid, TestThread, (void *) &sem2)) {
        perror("THREAD_CREATE");
        exit(1);
    }

    fprintf(stderr, "waiting for sem1\n");
    SEM_WAIT(&sem1);
    fprintf(stderr, "main: sem1 posted\n");

    fprintf(stderr, "waiting for sem2\n");
    SEM_WAIT(&sem2);
    fprintf(stderr, "main: sem2 posted\n");

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2016/02/19 17:57:20  dechavez
 * committed to CVS
 *
 */
