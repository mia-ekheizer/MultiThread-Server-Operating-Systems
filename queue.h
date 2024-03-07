#ifndef HW3_QUEUE_H
#define HW3_QUEUE_H

#include "pthread.h"

typedef struct request {
    int connfd;
    struct request *prev;
    struct request *next;
}

typedef struct queue {
    request *head;
    request *tail;
    int size;
    
}

void enqueue(queue *q, request *req);
request* dequeue(queue *q);
bool isEmpty(queue q);

//for drop_random usage:
void delByIndex(queue *q, int index);
int getQueueSize(queue q);



#endif /* HW3_QUEUE_H */