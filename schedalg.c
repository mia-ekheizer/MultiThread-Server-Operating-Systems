#include "schedalg.h"
#include "queue.h"
#include "segel.h"

void blockSchedAlg(request *req, serverArgs *servArgs) {
    pthread_mutex_lock(&servArgs->currMutex);
    while(waiting_requests->size >= servArgs->queue_size) {
        pthread_cond_wait(&servArgs->cond_var_master, &servArgs->currMutex);
    }
    enqueue(servArgs->waiting_requests, req);
    pthread_cond_signal(&servArgs->cond_var_workers);
    pthread_mutex_unlock(&servArgs->currMutex);
    requestError(req->connfd, req->filename, "404", "Not found", "OS-HW3 Server could not find this file"); // part 3 error
    close(req->connfd);
    free(req);

}
void dropTailSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_lock(&servArgs->currMutex);
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        requestError(req->connfd, req->filename, "404", "Not found", "OS-HW3 Server could not find this file"); // part 3 error
        close(req->connfd);
        free(req);
    }
    else {
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(&servArgs->cond_var_workers);
    }
    pthread_mutex_unlock(&servArgs->currMutex);
}

void dropHeadSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_lock(&servArgs->currMutex);
    if(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        if(isEmpty(servArgs->waiting_requests)) {
            requestError(req->connfd, req->filename, "404", "Not found", "OS-HW3 Server could not find this file"); // part 3 error
            close(req->connfd);
            free(req);
            pthread_mutex_unlock(&servArgs->currMutex);
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
        pthread_mutex_unlock(&servArgs->currMutex);
    }

}
void blockFlushSchedAlg(request *req, serverArgs *servArgs){

}
void dropRandomSchedAlg(request *req, serverArgs *servArgs){

}