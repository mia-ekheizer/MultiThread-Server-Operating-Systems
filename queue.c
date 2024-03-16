#include "queue.h"

void initRequestQueue(requestQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

request* initRequest(int connfd) {
    request* new_request = (request*)malloc(sizeof(request));
    new_request->connfd = connfd;
    new_request->prev = NULL;
    new_request->next = NULL;
    gettimeofday(&(new_request->arrival_time), NULL);
    return new_request;
}

void enqueue(requestQueue *q, request *req) {
    if(q->size == 0) {
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

request* dequeue(requestQueue *q) {
    request *returnRequest = q->head;
    if(q->size == 1) {
        q->head = NULL;
        q->tail = NULL;
    }
    else {
        q->head = q->head->next;
        q->head->prev = NULL;
    }
    q->size--;
    return returnRequest;
}

//for drop_random usage:
void delByIndex(requestQueue *q, int index); //TODO: