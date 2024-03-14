#ifndef HW3_SERVER_ARGS_H
#define HW3_SERVER_ARGS_H

#include <pthread.h>
#include "queue.h"

typedef struct serverArgs {
    pthread_mutex_t currMutex;
    pthread_cond_t cond_var_workers;
    pthread_cond_t cond_var_master;
    requestQueue* waiting_requests;
    requestQueue* handled_requests;
    int queue_size;
} serverArgs;

void initServerArgs(serverArgs* server_args, pthread_mutex_t mutex, pthread_cond_t cond_var_workers, pthread_cond_t cond_var_master, requestQueue* waiting_requests, requestQueue* handled_requests, int queue_size);

#endif /* HW3_SERVER_ARGS_H */