#ifndef HW3_QUEUE_H
#define HW3_QUEUE_H

#include <string.h>
#include <sys/time.h>

typedef struct request {
    int connfd;
    struct request *prev;
    struct request *next;
    // for statistics
    struct timeval arrival_time;
    struct timeval dispatch_time;
} request;

typedef struct requestQueue {
    request *head;
    request *tail;
    int size;
} requestQueue;

void initRequestQueue(requestQueue* q);
request* initRequest(int connfd);
void enqueue(requestQueue *q, request *req);
request* dequeue(requestQueue *q);
int isEmpty(requestQueue *q);
struct timeval calcDispatchInterval(request* req);

//for drop_random usage:
void delByIndex(requestQueue *q, int index);

#endif /* HW3_QUEUE_H */