#include "server_args.h"

void initServerArgs(serverArgs* server_args, pthread_mutex_t* mutex, pthread_cond_t* cond_var_workers, pthread_cond_t* cond_var_master, requestQueue* waiting_requests, requestQueue* handled_requests, int queue_size) {
    server_args->currMutex = *mutex;
    server_args->cond_var_workers = *cond_var_workers;
    server_args->cond_var_master = *cond_var_master;
    server_args->waiting_requests = waiting_requests;
    server_args->handled_requests = handled_requests;
    server_args->queue_size = queue_size;
}