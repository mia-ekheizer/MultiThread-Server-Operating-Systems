#include "schedalg.h"
#include "queue.h"
#include "segel.h"
#include "server_args.h"

// picks the scheduling algorithm according to the user's input if the waiting requests queue is full.
void pickSchedAlg(SchedAlg sched_alg, request* curr_request, serverArgs *servArgs) {
    pthread_mutex_lock(&(servArgs->currMutex));
    if (servArgs->waiting_requests->size + servArgs->handled_requests->size <= servArgs->queue_size) {
        enqueue(servArgs->waiting_requests, curr_request);
        pthread_cond_signal(&servArgs->cond_var_workers);
        pthread_mutex_unlock(&servArgs->currMutex);
    }
    else if(sched_alg == BLOCK) {
        blockSchedAlg(curr_request, servArgs);
    }
    else if(sched_alg == DT) {
        dropTailSchedAlg(curr_request, servArgs);
    }
    else if(sched_alg == DH) {
        dropHeadSchedAlg(curr_request, servArgs);
    }
    else if(sched_alg == BF) {
        blockFlushSchedAlg(curr_request, servArgs);
    }
    else if(sched_alg == RANDOM) {
        dropRandomSchedAlg(curr_request, servArgs);
    }
}

// implementation of the block scheduling algorithm.
void blockSchedAlg(request *req, serverArgs *servArgs) {
    while(servArgs->waiting_requests->size + servArgs->handled_requests->size > servArgs->queue_size) {
        pthread_cond_wait(&(servArgs->cond_var_master), &(servArgs->currMutex));
    }
    enqueue(servArgs->waiting_requests, req);
    pthread_cond_signal(&(servArgs->cond_var_workers));
    pthread_mutex_unlock(&(servArgs->currMutex));
    Close(req->connfd);
    free(req);
}

// implementation of the drop tail scheduling algorithm.
void dropTailSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_unlock(&(servArgs->currMutex));
    Close(req->connfd);
    free(req);
}

// implementation of the drop head scheduling algorithm.
void dropHeadSchedAlg(request *req, serverArgs *servArgs){
    struct request *temp = dequeue(servArgs->waiting_requests);
    Close(temp->connfd);
    free(temp);
    enqueue(servArgs->waiting_requests, req);
    pthread_cond_signal(&servArgs->cond_var_workers);
    pthread_mutex_unlock(&servArgs->currMutex);
}

// implementation of the block flush scheduling algorithm.
void blockFlushSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_unlock(&servArgs->currMutex);
}

// implementation of the drop random scheduling algorithm.
void dropRandomSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_unlock(&servArgs->currMutex);
}