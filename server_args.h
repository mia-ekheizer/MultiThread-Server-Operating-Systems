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

#endif /* HW3_SERVER_ARGS_H */