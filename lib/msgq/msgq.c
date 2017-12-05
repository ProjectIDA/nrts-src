#pragma ident "$Id: msgq.c,v 1.17 2017/10/07 21:38:10 dechavez Exp $"
/*======================================================================
 *
 *  Simple minded message queue to handle buffering of data between
 *  threads.  This implementation uses the mutex and semaphore macros.
 *
 *  A message is pulled off a queue with msgqGet(), and put on a queue
 *  with msgqPut().  Use msgqInit() to initialize a queue and fill it
 *  it with pointers to pre-sized buffers.
 *
 *  The simple minded part is that there is no notion of message type.
 *  All messages are UINT8 pointers, and it is assumed that the
 *  application knows what to do with them, and won't clobber memory.
 *
 *====================================================================*/
#include "msgq.h"

#define TESTLEN  ((size_t) 4)
#define TESTCHAR 'e'

/* Initialize a queue with pre-sized buffers */

BOOL msgqInit(MSGQ *queue, UINT32 nelem, UINT32 maxelem, UINT32 maxlen)
{
UINT32 i;
MSGQ_MSG *crnt, *newm;

    MUTEX_INIT(&queue->mp);
    SEM_INIT(&queue->sp, nelem, maxelem);
    queue->lp = (LOGIO *) NULL;
    queue->head.next = (MSGQ_MSG *) NULL;
    queue->head.data = (UINT8 *) NULL;
    queue->nfree = nelem;
    queue->lowat = nelem;
    queue->hiwat = 0;

    crnt = &queue->head;
    for (i = 0; i < nelem; i++) {

    /* create the message object */

        newm = (MSGQ_MSG *) malloc(sizeof(MSGQ_MSG));
        if (newm == (MSGQ_MSG *) NULL) return FALSE;

    /* initialize the message object */

        newm->maxlen = maxlen;
        newm->data   = (UINT8 *) malloc(maxlen+TESTLEN);
        newm->len    = 0;
        if (newm->data == (UINT8 *) NULL) return FALSE;
        memset((void *) newm->data, (int) TESTCHAR, (size_t) maxlen+TESTLEN);
        newm->next = (MSGQ_MSG *) NULL;

    /* add it to the end of the list */

        crnt->next = newm;
        crnt       = crnt->next;
    }

    return TRUE;
}

/* Enable logging */

VOID msgqSetLog(MSGQ *queue, LOGIO *lp)
{
    MUTEX_LOCK(&queue->mp);
        queue->lp = lp;
    MUTEX_UNLOCK(&queue->mp);
}

/* Pull the next message off the queue */

MSGQ_MSG *msgqGet(MSGQ *queue, UINT16 flag)
{
MSGQ_MSG *head, *msg;
static char *fid = "msgqGet";

    if (queue == (MSGQ *) NULL) {
        logioMsg(queue->lp, LOG_ERR, "%s: ABORT - NULL queue pointer received!", fid);
        abort();
    }

    if (flag == MSGQ_WAIT) {
        SEM_WAIT(&queue->sp);
    } else if (SEM_TRYWAIT(&queue->sp) != 0) {
        return (MSGQ_MSG *) NULL;
    }

    MUTEX_LOCK(&queue->mp);
        head = &queue->head;
        if (head->next == (MSGQ_MSG *) NULL) {
            logioMsg(queue->lp, LOG_ERR, "%s: nfree=%d lowat=%d hiwat=%d", fid, queue->nfree, queue->lowat, queue->hiwat);
            logioMsg(queue->lp, LOG_ERR, "%s: ABORT - QUEUE MANAGMENT IS HOSED!", fid);
            abort();
        }
        msg = head->next;
        head->next = msg->next;
        if (--queue->nfree < queue->lowat) queue->lowat = queue->nfree;
    MUTEX_UNLOCK(&queue->mp);

    return msg;
}

VOID msgqPut(MSGQ *queue, MSGQ_MSG *newm)
{
MSGQ_MSG *crnt;

    MUTEX_LOCK(&queue->mp);
        crnt = &queue->head;
        while (crnt->next != (MSGQ_MSG *) NULL) crnt = crnt->next;
        crnt->next = newm;
        newm->next = (MSGQ_MSG *) NULL;
        if (++queue->nfree > queue->hiwat) queue->hiwat = queue->nfree;
        SEM_POST(&queue->sp);
    MUTEX_UNLOCK(&queue->mp);
}

UINT32 msgqNfree(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->nfree;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

UINT32 msgqLowat(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->lowat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

UINT32 msgqHiwat(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->hiwat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

/* Flush a queue */

VOID msgqFlush(MSGQ *full, MSGQ *empty)
{
MSGQ_MSG *msg;

    while ((msg = msgqGet(full, MSGQ_NOWAIT)) != (MSGQ_MSG *) NULL) {
        msgqPut(empty, msg);
    }
}

/* Check for message overruns */

BOOL msgqCheck(MSGQ_MSG *msg)
{
static UINT8 tbuf[TESTLEN] = {TESTCHAR, TESTCHAR, TESTCHAR, TESTCHAR};
static CHAR *fid = "msgqCheck";

    if (msg == (MSGQ_MSG *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (msg->maxlen <= 0) {
        errno = EINVAL;
        return FALSE;
    }

    if (msg->data == (UINT8 *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (memcmp((void *) (msg->data + msg->maxlen), (void *) tbuf, (size_t) TESTLEN) != 0) {
        errno = EMSGSIZE;
        return FALSE;
    }

    if (msg->len > msg->maxlen) {
        errno = EMSGSIZE;
        return FALSE;
    }

    return TRUE;
}

BOOL msgqInitBuf(MSGQ_BUF *buf, UINT32 nelem, UINT32 size)
{
    if (!msgqInit(&buf->heap, nelem, nelem, size)) return FALSE;
    if (!msgqInit(&buf->full,     0, nelem, size)) return FALSE;

    return TRUE;
}

/* Destroy items */

VOID msgqDestroyMessage(MSGQ_MSG *msg)
{
    if (msg == NULL) return;
    free(msg->data);
    free(msg);
}

VOID msgqDestroyQueue(MSGQ *queue)
{
MSGQ_MSG *msg;

    if (queue == NULL) return;

    while ((msg = msgqGet(queue, MSGQ_NOWAIT)) != (MSGQ_MSG *) NULL) msgqDestroyMessage(msg);
}

VOID msgqDestroyBuf(MSGQ_BUF *buf)
{
    if (buf == NULL) return;

    msgqDestroyQueue(&buf->heap);
    msgqDestroyQueue(&buf->full);
}

#ifdef DEBUG_TEST

/* Expected output from this program

   % test 10 -slow
   ProducerThread: generating 10 messages
   ConsumerThread: ready
   ConsumerThread: message number 1 of 10
   ConsumerThread: message number 2 of 10
   ConsumerThread: message number 3 of 10
   ConsumerThread: message number 4 of 10
   ConsumerThread: message number 5 of 10
   ConsumerThread: message number 6 of 10
   ConsumerThread: message number 7 of 10
   ConsumerThread: message number 8 of 10
   ConsumerThread: message number 9 of 10
   Producer thread is finished... send self-destruct message to consumer thread
   ConsumerThread: message number 10 of 10
   ConsumerThread: shutdown message received
   exit(0)

 */

#include "platform.h"

#define TIME_TO_DIE -1

MSGQ heap, consumerQ;
BOOL GoSlowly = FALSE;

typedef struct {
    char string[80];
    int value;
} PAYLOAD;

typedef struct {
    int count;
    SEMAPHORE sem;
} CONTROL;

static void BuildPayload(PAYLOAD *payload, int message, int count)
{
    if (((payload->value) = message) == TIME_TO_DIE) return;
    sprintf(payload->string, "message number %d of %d", message, count);
}

THREAD_FUNC ProducerThread(void *arg)
{
int i, DontCare=0;
CONTROL *control;
PAYLOAD *payload;
MSGQ_MSG *msg;

    /* we are expecting to be given a CONTROL structure from the main */

    control = (CONTROL *) arg;
    printf("ProducerThread: generating %d messages\n", control->count);

    /* block until initializtion is complete */

    SEM_WAIT(&control->sem);

    /* Send the requested number of messages */

    for (i = 0; i < control->count; i++) {
        msg = msgqGet(&heap, MSGQ_WAIT);
            BuildPayload((PAYLOAD *) msg->data, i, control->count);
            if (GoSlowly) sleep(1);
        msgqPut(&consumerQ, msg);
    }

    /* Send the shutdown message */

    printf("Producer thread is finished... send self-destruct message to consumer thread\n"); fflush(stdout);
    if (GoSlowly) sleep(1);
    msg = msgqGet(&heap, MSGQ_WAIT);
        BuildPayload((PAYLOAD *) msg->data, TIME_TO_DIE, DontCare);
    msgqPut(&consumerQ, msg);

    /* We are done here, wait until death */

    pause(); /* we could have also used THREAD_EXIT((void *) 0) */

}

static BOOL ProcessPayload(PAYLOAD *payload)
{
BOOL retval = FALSE;

    if (payload->value == TIME_TO_DIE) {
        printf("ConsumerThread: ProcessPayload: shutdown message received\n");
        retval = TRUE;
    } else {
        printf("ConsumerThread: ProcessPayload: %s\n", payload->string);
    }

    return retval;
}

THREAD_FUNC ConsumerThread(void *arg)
{
SEMAPHORE *sem;
PAYLOAD *payload;
MSGQ_MSG *msg;
BOOL finished = FALSE;

    /* all we expect from main is the semaphore to use to signify we are ready */

    sem = (SEMAPHORE *) arg;
    printf("ConsumerThread: ready\n"); fflush(stdout);
    if (GoSlowly) sleep(1);

    /*  Let the producer thread it is OK to begin */

    SEM_POST(sem);

    /* process messages until we get the one that says to quit */

    while (!finished) {
        msg = msgqGet(&consumerQ, MSGQ_WAIT);
            finished = ProcessPayload((PAYLOAD *) msg->data);
        msgqPut(&heap, msg);
    }

    printf("exit(0)\n");
    exit(0);
}

int main(int argc, char **argv)
{
THREAD tid;
#define MAXMSGLEN 1024
int i, unused = 0;
CONTROL control;

    control.count = -1;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-slow") == 0) {
            GoSlowly = TRUE;
        } else if (control.count == -1) {
            if ((control.count = atoi(argv[i])) <= 0) {
                fprintf(stderr, "illegal count '%s'\n", argv[i]);
                exit(1);
            }
        } else {
            fprintf(stderr, "usage: %s count [-slow]\n", argv[0]);
            exit(2);
        }
    }

    /* The producer thread gets an empty message to start */

    if (!msgqInit(&heap, 1, unused, MAXMSGLEN)) perror("msgqInit");

    /* The consumer gets just the framework to receive messages, but no content at first */

    if (!msgqInit(&consumerQ, 0, unused, MAXMSGLEN)) perror("msgqInit");

    /* Fire up the consumer thread.  It is going to need to know how many messages to send as
     * well as the synchornization semaphore, all conveniently contained in CONTROL
     */
    if (!THREAD_CREATE(&tid, ProducerThread, (void *) &control    )) perror("THREAD_CREATE");
    if (GoSlowly) sleep(1);

    /* The consumer only needs the semaphore */

    if (!THREAD_CREATE(&tid, ConsumerThread, (void *) &control.sem)) perror("THREAD_CREATE");

    /* All work is taking place between the two threads, so we just wait for them to do their thing */

    while (1) pause(); /* Alternatively we could have not used ConsumerThread and just put its code here */
}

#endif /* DEBUG_TEST */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: msgq.c,v $
 * Revision 1.17  2017/10/07 21:38:10  dechavez
 * made the demo even more clear
 *
 * Revision 1.16  2017/10/07 21:22:19  dechavez
 * added commentary to demo program
 *
 * Revision 1.15  2017/10/07 21:00:13  dechavez
 * added example program via -DDEBUG_TEST
 *
 * Revision 1.14  2016/07/20 18:56:06  dechavez
 * added msgqDestroyMessage(), msgqDestroyQueue(), msgqDestroyBuf()
 *
 * Revision 1.13  2013/01/23 23:58:44  dechavez
 * include stats when aborting() on corrupt queue
 *
 * Revision 1.12  2006/05/17 23:23:24  dechavez
 * added copyright notice
 *
 * Revision 1.11  2006/05/17 18:24:47  dechavez
 * replaced previous msgq.c which has been deprecated off to oldmsgq library
 *
 * Revision 1.2  2006/05/04 20:36:16  dechavez
 * added msgqInitBuf()
 *
 * Revision 1.1  2003/06/10 00:12:08  dechavez
 * imported from ESSW
 *
 */
