#ifndef HW3_SCHEDULING_ALGORITHM_H
#define HW3_SCHEDULING_ALGORITHM_H

#include "queue.h"
#include "server_args.h"

// all possible scheduling algorithms.
typedef enum SchedAlg {BLOCK, DT, DH, BF, RANDOM} SchedAlg;

void pickSchedAlg(SchedAlg policy, request* curr_request, serverArgs *servArgs);
void blockSchedAlg(request *req, serverArgs *servArgs);
void dropTailSchedAlg(request *req, serverArgs *servArgs);
void dropHeadSchedAlg(request *req, serverArgs *servArgs);
void blockFlushSchedAlg(request *req, serverArgs *servArgs);
void dropRandomSchedAlg(request *req, serverArgs *servArgs);
#endif /* HW3_SCHEDULING_ALGORITHM_H */