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
    int curr_index = 0;
    request* curr_request = q->head;
    if (index > q->size) {
        return;
    }
    while (curr_request != NULL) {
        if (curr_index == index) {
            if (q->head == curr_request) { // if we want to delete the head of the queue
                request* to_delete = dequeue(q);
                Close(to_delete->connfd);
                free(to_delete);
                return;
            }
            else if (q->tail == curr_request) { // if we want to delete the tail of the queue
                curr_request->prev->next = NULL;
                q->tail = curr_request->prev;
                Close(curr_request->connfd);
                free(curr_request);
                return;
            }
            else { // if we want to delete any other element in the queue
                curr_request->prev->next = curr_request->next;
                curr_request->next->prev = curr_request->prev;
                Close(curr_request->connfd);
                free(curr_request);
                return;
            } 
        }
        curr_request = curr_request->next;
        curr_index++;
    }
}