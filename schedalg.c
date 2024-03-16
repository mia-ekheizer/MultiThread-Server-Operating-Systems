#include "schedalg.h"
#include "queue.h"
#include "segel.h"
#include "server_args.h"

// picks the scheduling algorithm according to the user's input in case of overloading.
void pickSchedAlg(char* sched_alg, request* curr_request, serverArgs *servArgs) {
    pthread_mutex_lock(servArgs->mutex);
    if (servArgs->waiting_requests->size + servArgs->handled_requests->size < servArgs->queue_size) {
        enqueue(servArgs->waiting_requests, curr_request);
        pthread_cond_signal(servArgs->cond_var_workers);
        pthread_mutex_unlock(servArgs->mutex);
    }
    else if(strcmp(sched_alg, "block") == 0) {
        blockSchedAlg(curr_request, servArgs);
    }
    else if(strcmp(sched_alg, "dt") == 0) {
        dropTailSchedAlg(curr_request, servArgs);
    }
    else if(strcmp(sched_alg, "dh") == 0) {
        dropHeadSchedAlg(curr_request, servArgs);
    }
    else if(strcmp(sched_alg, "bf") == 0) {
        blockFlushSchedAlg(curr_request, servArgs);
    }
    else if(strcmp(sched_alg, "random") == 0) {
        dropRandomSchedAlg(curr_request, servArgs);
    }
}

// implementation of the block scheduling algorithm.
void blockSchedAlg(request *req, serverArgs *servArgs) {
    while(servArgs->waiting_requests->size + servArgs->handled_requests->size >= servArgs->queue_size) {
        pthread_cond_wait(servArgs->cond_var_master, servArgs->mutex);
    }
    enqueue(servArgs->waiting_requests, req);
    pthread_cond_signal(servArgs->cond_var_workers);
    pthread_mutex_unlock(servArgs->mutex);
}

// implementation of the drop tail scheduling algorithm.
void dropTailSchedAlg(request *req, serverArgs *servArgs){
    Close(req->connfd);
    free(req);
    pthread_mutex_unlock(servArgs->mutex);
}

// implementation of the drop head scheduling algorithm.
void dropHeadSchedAlg(request *req, serverArgs *servArgs){
    if (servArgs->waiting_requests->size == 0) {
        Close(req->connfd);
        free(req);
        pthread_mutex_unlock(servArgs->mutex);
        return;
    }
    else {
        request *head_request = dequeue(servArgs->waiting_requests);
        Close(head_request->connfd);
        free(head_request);
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(servArgs->cond_var_workers);
        pthread_mutex_unlock(servArgs->mutex);
    }
}

// implementation of the block flush scheduling algorithm.
void blockFlushSchedAlg(request *req, serverArgs *servArgs){
    pthread_mutex_unlock(servArgs->mutex);
}

// implementation of the drop random scheduling algorithm.
void dropRandomSchedAlg(request *req, serverArgs *servArgs){
    if (servArgs->waiting_requests->size == 0) {
        Close(req->connfd);
        free(req);
        pthread_mutex_unlock(servArgs->mutex);
        return;
    }
    else {
        int dropped_counter = 0;
        int num_of_requests_to_drop = servArgs->waiting_requests->size / 2;
        if (servArgs->waiting_requests->size % 2 != 0) {
            num_of_requests_to_drop++;
        }
        while (dropped_counter < num_of_requests_to_drop) {
            deleteByIndex(servArgs->waiting_requests, rand() % servArgs->waiting_requests->size);
            dropped_counter++;
        }
        enqueue(servArgs->waiting_requests, req);
        pthread_cond_signal(servArgs->cond_var_workers);
        pthread_mutex_unlock(servArgs->mutex);
    }
}