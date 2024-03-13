#ifndef HW3_THREAD_ARGS_H
#define HW3_THREAD_ARGS_H

#include "queue.h"

typedef struct threadArgs {
    requestQueue* waiting_requests;
    requestQueue* handled_requests;
} threadArgs;

#endif /* HW3_THREAD_ARGS_H */