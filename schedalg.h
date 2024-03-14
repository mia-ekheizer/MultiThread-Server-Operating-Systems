#ifndef HW3_SCHEDALG_H
#define HW3_SCHEDALG_H

#include "queue.h"
#include "server_args.h"

typedef enum SchedAlg {BLOCK, DT, DH, BF, RANDOM} SchedAlg;

void pickSchedAlgAndExecute(SchedAlg schedalg, request* curr_request, serverArgs *servArgs);
void blockSchedAlg(request *req, serverArgs *servArgs);
void dropTailSchedAlg(request *req, serverArgs *servArgs);
void dropHeadSchedAlg(request *req, serverArgs *servArgs);
void blockFlushSchedAlg(request *req, serverArgs *servArgs);
void dropRandomSchedAlg(request *req, serverArgs *servArgs);
#endif