#include "queue.h"

void enqueue(queue *q, request *req) {
    if(q.isEmpty()) {
        q->head = req;
        q->tail = req;
    }
    else {
        request *prevTail = q->tail;
        q->tail = req;
        req->prev = prevTail;
        prevTail->next = req;
    }
    q->size++;
}
request* dequeue(queue *q) {
    request *returnRequest = q->head;
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
bool isEmpty(queue q) {
    return q.size == 0;
}

//for drop_random usage:
void delByIndex(queue *q, int index); //TODO:
int getQueueSize(queue q) {
    return q.size;
}