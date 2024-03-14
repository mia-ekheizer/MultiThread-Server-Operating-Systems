#include "schedalg.h"
#include "queue.h"
#include "segel.h"
#include "server_args.h"

void pickSchedAlg(Schedalg schedalg, request* curr_request, serverArgs *servArgs) {
    if(schedalg == BLOCK) {
        blockSchedAlg(curr_request, servArgs);
    }
    else if(schedalg == DT) {
        dropTailSchedAlg(curr_request, servArgs);
    }
    else if(schedalg == DH) {
        dropHeadSchedAlg(curr_request, servArgs);
    }
    else if(schedalg == BF) {
        blockFlushSchedAlg(curr_request, servArgs);
    }
    else if(schedalg == RANDOM) {
        dropRandomSchedAlg(curr_request, servArgs);
    }
}

void blockSchedAlg(request *req, serverArgs *servArgs) {
    pthread_mutex_lock(&(servArgs->currMutex));
    while(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        pthread_cond_wait(&(servArgs->cond_var_master), &(servArgs->currMutex));
    }
    enqueue(servArgs->waiting_requests, req);
    pthread_cond_signal(&(servArgs->cond_var_workers));
    pthread_mutex_unlock(&(servArgs->currMutex));
    Close(req->connfd);
    free(req);
}
void dropTailSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_lock(&(servArgs->currMutex));
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        Close(req->connfd);
        free(req);
    }
    else {
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(&servArgs->cond_var_workers);
    }
    pthread_mutex_unlock(&servArgs->currMutex);
}

void dropHeadSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_lock(&(servArgs->currMutex));
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        // TODO: check what should be done in this case.
        if(isEmpty(servArgs->waiting_requests)) {
            Close(req->connfd);
            free(req);
            pthread_mutex_unlock(&servArgs->currMutex);
            exit(1);
        }
        else {
            struct request *temp = dequeue(servArgs->waiting_requests);
            Close(temp->connfd);
            free(temp);
        }
    }
    else {
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(&servArgs->cond_var_workers);
        pthread_mutex_unlock(&servArgs->currMutex);
    }
}
void blockFlushSchedAlg(request *req, serverArgs *servArgs){

}
void dropRandomSchedAlg(request *req, serverArgs *servArgs){

}