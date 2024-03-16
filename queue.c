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
void deleteByIndex(requestQueue *q, int index) {
    if (index > q->size - 1) {
        return;
    }
    int curr_index = 0;
    request* curr_request = q->head;
    request* to_delete;
    while (curr_index != index) {
        curr_request = curr_request->next;
        curr_index++;
    }
    // if we want to delete the head of the queue
    if (q->head == curr_request) { 
        to_delete = dequeue(q);
    }
    // if we want to delete the tail of the queue
    else if (q->tail == curr_request) { 
        request* new_tail = curr_request->prev;
        new_tail->next = NULL;
        q->tail = new_tail;
        to_delete = curr_request;
        q->size--;
    }
    // if we want to delete any other element in the queue
    else { 
        request* curr_prev = curr_request->prev;
        request* curr_next = curr_request->next;
        curr_prev->next = curr_request->next;
        curr_next->prev = curr_request->prev;
        to_delete = curr_request;
        q->size--;
    } 
    Close(to_delete->connfd);
    free(to_delete);
}

// for the threads' function usage
void deleteByConnfd(requestQueue *q, int connfd) {
    request* curr_request = q->head;
    int curr_connfd = curr_request->connfd;
    request* to_delete;
    while (curr_request != NULL && curr_connfd != connfd) {
        curr_request = curr_request->next;
        curr_connfd = curr_request->connfd;
    }
    // if no request found with connfd
    if (curr_request == NULL) {
        return;
    }
    // if we want to delete the head of the queue
    else if (q->head == curr_request) { 
        to_delete = dequeue(q);
    }
    // if we want to delete the tail of the queue
    else if (q->tail == curr_request) { 
        request* new_tail = curr_request->prev;
        new_tail->next = NULL;
        q->tail = new_tail;
        to_delete = curr_request;
        q->size--;
    }
    // if we want to delete any other element in the queue
    else { 
        request* curr_prev = curr_request->prev;
        request* curr_next = curr_request->next;
        curr_prev->next = curr_request->next;
        curr_next->prev = curr_request->prev;
        to_delete = curr_request;
        q->size--;
    } 
    Close(to_delete->connfd);
    free(to_delete);
}