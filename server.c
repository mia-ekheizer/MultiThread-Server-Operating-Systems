#include "segel.h"
#include "request.h"
#include <pthread.h>
#include "queue.h"
#include "schedalg.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// setting up the condition variables, the mutex lock and the state variables to execute threadFunction and the master thread in order.
cond_t cond_var_master;
cond_t cond_var_workers;
mutex_t mutex;
int waiting_requests_size = 0; // state variable for the worker condition variable
int handled_requests_size = 0; // state variable for the master condition variable

enum Schedalg {BLOCK, DT, DH, BF, RANDOM};

// HW3: Parse the new arguments too
void getargs(int *portnum, int *threads, int* queue_size, enum Schedalg *schedalg, int argc, char *argv[])
{
    if (argc != 5) { //TODO: changed from <2 to != 5
	    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	    exit(1);
    }
    *portnum = atoi(argv[1]);
    if(*portnum < 1024 || *portnum > 65535) {//TODO: is this necessary?
        //TODO: fix silent exit??
        exit(1);
    }
    *threads = atoi(argv[2]);
    if (!threads || *threads <= 0) {
        exit(1);
    }
    *queue_size = atoi(argv[3]);
    if (!queue_size || *queue_size <= 0) {
        exit(1);
    }
    *schedalg = argv[4];
    if(strcmp(*schedalg, "block") == 0) {
        *schedalg = BLOCK;
    }
    else if(strcmp(*schedalg, "dt") == 0) {
        *schedalg = DT;
    }
    else if(strcmp(*schedalg, "dh") == 0) {
        *schedalg = DH;
    }
    else if(strcmp(*schedalg, "bf") == 0) {
        *schedalg = BF;
    }
    else if(strcmp(*schedalg, "random") == 0) {
        *schedalg = RANDOM;
    }
    else {
        //TODO: fix silent exit??
        exit(1);
    }
}

// the function that all worker threads are working on.
void* threadFunction(void* args)
{
    struct threadArgs* curr_args = (struct threadArgs*)args;
    // after a worker thread finishes handling a request, it waits again for another request.
    while (1) {
        mutex_lock(&mutex);
        // free threads are waiting for a new request.
        while (waiting_requests_size == 0)
        {
            pthread_cond_wait(&cond_var_workers, &mutex);
        }
        // once there is a free thread and a waiting request, the thread starts handling the request.
        struct request* curr_request = dequeue(curr_args->waiting_requests);
        waiting_requests_size--;
        enqueue(curr_args->handled_requests, curr_request);
        handled_requests_size++;
        mutex_unlock(&mutex);
        requestHandle(curr_request->connfd);
        mutex_lock(&mutex);
        struct request* finished_request = dequeue(curr_args->handled_requests, curr_request);
        free(finished_request);
        handled_requests_size--;
        // the thread signals the master that a request has been handled.
        pthread_cond_signal(&cond_var_master);
        mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    // setting up all the structs and variables.
    int listenfd, connfd, clientlen, portnum, threads, queue_size;
    enum Schedalg schedalg;
    struct sockaddr_in clientaddr;
    struct requestQueue waiting_requests, handled_requests;
    
    pthread_cont_t cond_var_workers; // need this to ensure that the variable is initialized.
    pthread_cont_t cond_var_master;
    pthread_cond_init(&cond_var_workers, NULL);
    pthread_cond_init(&cond_var_master, NULL);

    // initializing the request queues.
    initRequestQueue(waiting_requests);
    initRequestQueue(handled_requests);

    //initializing serverArgs.
    serverArgs servArgs;
    servArgs.currMutex = &mutex;
    servArgs.cond_var_workers = &cond_var_workers;
    servArgs.cond_var_master = &cond_var_master;
    servArgs.waiting_requests = &waiting_requests;
    servArgs.handled_requests = &handled_requests;
    servArgs.queue_size = queue_size;

    getargs(&portnum, &threads, &queue_size, &schedalg, argc, argv);
    // initializing the worker threads.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * threads);
    for (int i = 0; i < threads; i++)
    {
        struct threadArgs* args = (threadArgs*)malloc(sizeof(threadArgs));
        args->waiting_requests = waiting_requests;
        args->handled_requests = handled_requests;
        pthread_create(&worker_threads[i], NULL, threadFunction, (void)* args);
    }

    // setting up server.
    listenfd = Open_listenfd(portnum);
    while (1) {
        struct request* curr_request = (request*)malloc(sizeof(request));
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        // adding the current request to the waiting_requests queue and starting again.
        curr_request.connfd = connfd;
        curr_request.prev = NULL;
        curr_request.next = NULL;

        if(schedalg == BLOCK) {
            blockSchedAlg(curr_request, &servArgs);
        }
        else if(schedalg == DT) {
            dropTailSchedAlg(curr_request, &servArgs);
        }
        else if(schedalg == DH) {
            dropHeadSchedAlg(curr_request, &servArgs);
        }
        else if(schedalg == BF) {
            blockFlushSchedAlg(curr_request, &servArgs);
        }
        else if(schedalg == RANDOM) {
            dropRandomSchedAlg(curr_request, &servArgs);
        }
        
        /*
        mutex_lock(&mutex);
        // if the server has reached the maximal amount of requests, it waits.
        while (handled_requests_size + waiting_requests_size > queue_size) { //TODO: part2
            pthread_cond_wait(&cond_var_master, &mutex);
        }
        // once there is enough space to handle another request, the server continues.
        enqueue(waiting_requests, curr_request);
        waiting_requests_size++;
        // the master thread signals to the other threads that there are waiting requests.
        pthread_cond_signal(&cond_var_workers);
        mutex_unlock(&mutex);
	    Close(connfd);
        */
    }
    pthread_cond_destroy(cond_var_master);
    pthread_cond_destroy(cond_var_workers);
    free(worker_threads);
}
