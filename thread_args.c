#include "thread_args.h"

void initThreadArgs(threadArgs* thread_args, requestQueue* waiting_requests, requestQueue* handled_requests, int id) {
    thread_args->waiting_requests = waiting_requests;
    thread_args->handled_requests = handled_requests;
    thread_args->id = id;
    thread_args->stat_req = 0;
    thread_args->dynm_req = 0;
    thread_args->total_req = 0;
}