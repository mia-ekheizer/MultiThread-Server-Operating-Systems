#ifndef HW3_THREAD_ARGS_H
#define HW3_THREAD_ARGS_H

#include "queue.h"

typedef struct threadArgs {
    struct requestQueue waiting_requests;
    struct requestQueue handled_requests;
};

#endif /* HW3_THREAD_ARGS_H */