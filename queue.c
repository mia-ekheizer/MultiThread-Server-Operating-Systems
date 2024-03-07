#include "queue.h"

void initRequestQueue(struct requestQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void enqueue(struct requestQueue *q, struct request *req) {
    if(isEmpty(q)) {
        q->head = req;
        q->tail = req;
    }
    else {
        struct request *prevTail = q->tail;
        q->tail = req;
        req->prev = prevTail;
        prevTail->next = req;
    }
    q->size++;
}

struct request* dequeue(struct requestQueue *q) {
    struct request *returnRequest = q->head;
    if(q->size == 1) {
        q->head = NULL;
        q->tail = NULL;
    }
    else {
        q->head = returnRequest->next;
        returnRequest->prev = NULL;
    }
    q->size--;
    return returnRequest;
}

int isEmpty(struct requestQueue *q) {
    return q->size == 0;
}

//for drop_random usage:
void delByIndex(struct requestQueue *q, int index); //TODO: