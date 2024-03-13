#include "queue.h"
#include <sys/time.h>

void initRequestQueue(requestQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

request* initRequest(int connfd) {
    request* new_request = (request*)malloc(sizeof(request));
    new_request.connfd = connfd;
    new_request->prev = NULL;
    new_request->next = NULL;
    gettimeofday(new_request->arrival_time, NULL);
    new_request->dispatch_time = NULL;
    return new_request;
}

void enqueue(requestQueue *q, request *req) {
    if (req->arrival_time)
    if(isEmpty(q)) {
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
        q->head = returnRequest->next;
        returnRequest->prev = NULL;
    }
    q->size--;
    return returnRequest;
}

int isEmpty(requestQueue *q) {
    return q->size == 0;
}

//for drop_random usage:
void delByIndex(requestQueue *q, int index); //TODO: