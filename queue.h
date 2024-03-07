#ifndef HW3_QUEUE_H
#define HW3_QUEUE_H

#include <string.h>

typedef struct request {
    int connfd;
    struct request *prev;
    struct request *next;
};

typedef struct requestQueue {
    struct request *head;
    struct request *tail;
    int size;
};

void initRequestQueue(struct requestQueue* q);
void enqueue(struct requestQueue *q, struct request *req);
struct request* dequeue(struct requestQueue *q);
int isEmpty(struct requestQueue *q);

//for drop_random usage:
void delByIndex(struct requestQueue *q, int index);

#endif /* HW3_QUEUE_H */