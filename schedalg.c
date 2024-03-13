#include "schedalg.h"
#include "queue.h"
#include "segel.h"
#include "server_args.h"

void blockSchedAlg(request *req, serverArgs *servArgs) {
    mutex_lock(&(servArgs->currMutex));
    while(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        pthread_cond_wait(&(servArgs->cond_var_master), &(servArgs->currMutex));
    }
    enqueue(servArgs->waiting_requests, req);
    gettimeofday(req->dispatch_time, NULL);
    pthread_cond_signal(&servArgs->cond_var_workers);
    mutex_unlock(&servArgs->currMutex);
    close(req->connfd);
    free(req);
}
void dropTailSchedAlg(request *req, serverArgs *servArgs){
    mutex_lock(&(servArgs->currMutex));
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        close(req->connfd);
        free(req);
    }
    else {
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(&servArgs->cond_var_workers);
    }
    mutex_unlock(&servArgs->currMutex);
}

void dropHeadSchedAlg(request *req, serverArgs *servArgs){
    mutex_lock(&(servArgs->currMutex));
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        // TODO: check what should be done in this case.
        if(isEmpty(servArgs->waiting_requests)) {
            close(req->connfd);
            free(req);
            mutex_unlock(&servArgs->currMutex);
            exit(1);
        }
        else {
            struct request *temp = dequeue(servArgs->waiting_requests);
            close(temp->connfd);
            free(temp);
        }
    }
    else {
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(&servArgs->cond_var_workers);
        mutex_unlock(&servArgs->currMutex);
    }
}
void blockFlushSchedAlg(request *req, serverArgs *servArgs){

}
void dropRandomSchedAlg(request *req, serverArgs *servArgs){

}