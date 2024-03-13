#ifndef HW3_SERVER_ARGS_H
#define HW3_SERVER_ARGS_H

#include <pthread.h>
#include "queue.h"

typedef struct serverArgs {
    mutex_t currMutex;
    cond_t cond_var_workers;
    cond_t cond_var_master;
    requestQueue* waiting_requests;
    requestQueue* handled_requests;
    int queue_size;
} serverArgs;

#endif /* HW3_SERVER_ARGS_H */