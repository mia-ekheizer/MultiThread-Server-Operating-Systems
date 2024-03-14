#include "segel.h"
#include "request.h"
#include <pthread.h>
#include "queue.h"
#include "schedalg.h"
#include "thread_args.h"
#include "server_args.h"
#include <stdlib.h>

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
pthread_cond_t cond_var_master;
pthread_cond_t cond_var_workers;
pthread_mutex_t mutex;

SchedAlg setSchedAlg(const char* user_input) {
    if(strcmp(user_input, "block") == 0) {
        return BLOCK;
    }
    else if(strcmp(user_input, "dt") == 0) {
        return DT;
    }
    else if(strcmp(user_input, "dh") == 0) {
        return DH;
    }
    else if(strcmp(user_input, "bf") == 0) {
        return BF;
    }
    else if(strcmp(user_input, "random") == 0) {
        return RANDOM;
    }
    else {
        //TODO: fix silent exit??
        exit(1);
    }
}

// HW3: Parse the new arguments too
void getargs(int *portnum, int *num_threads, int* queue_size, SchedAlg *schedalg, int argc, char *argv[])
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
    *num_threads = atoi(argv[2]);
    if (!num_threads || *num_threads <= 0) {
        exit(1);
    }
    *queue_size = atoi(argv[3]);
    if (!queue_size || *queue_size <= 0) {
        exit(1);
    }
    *schedalg = setSchedAlg(argv[4]);
}

// the function that all worker num_threads are working on.
void* threadFunction(void* args)
{
    threadArgs* curr_args = (threadArgs*)args;
    // after a worker thread finishes handling a request, it waits again for another request.
    while (1) {
        pthread_mutex_lock(&mutex);
        curr_args->id = pthread_self();
        // free num_threads are waiting for a new request.
        while (curr_args->waiting_requests->size == 0) // state variable for cond_var_workers.
        {
            pthread_cond_wait(&cond_var_workers, &mutex);
        }
        // once there is a free thread and a waiting request, the thread starts handling the request.
        request* curr_request = dequeue(curr_args->waiting_requests);
        enqueue(curr_args->handled_requests, curr_request);
        gettimeofday(&(curr_request->dispatch_time), NULL);
        pthread_mutex_unlock(&mutex);
        curr_args->total_req++;
        requestHandle(curr_request, curr_args);
        pthread_mutex_lock(&mutex);
        request* finished_request = dequeue(curr_args->handled_requests);
        free(finished_request);
        // the thread signals the master that a request has been handled.
        pthread_cond_signal(&cond_var_master);
        pthread_mutex_unlock(&mutex);
    }
}

void initWorkerThreads(pthread_t* worker_threads, int num_threads, requestQueue* waiting_requests, requestQueue* handled_requests) {
    for (int curr_thread_id = 0; curr_thread_id < num_threads; curr_thread_id++)
    {
        threadArgs* args = (threadArgs*)malloc(sizeof(threadArgs));
        initThreadArgs(args, waiting_requests, handled_requests, curr_thread_id);
        pthread_create(&worker_threads[curr_thread_id], NULL, threadFunction, (void*) args);
    }
}

void destroyServer(pthread_t* worker_threads, int num_threads, requestQueue* waiting_requests, requestQueue* handled_requests) {
    pthread_cond_destroy(&cond_var_master);
    pthread_cond_destroy(&cond_var_workers);
    for (int curr_thread_id = 0; curr_thread_id < num_threads; curr_thread_id++)
    {
        pthread_join(worker_threads[curr_thread_id], NULL);
        // how do we free args for each thread?
    }
    free(worker_threads);
    free(waiting_requests);
    free(handled_requests);
}

int main(int argc, char *argv[])
{
    // setting up all the structs and variables.
    int listenfd, connfd, clientlen, portnum, num_threads, queue_size;
    SchedAlg schedalg;
    struct sockaddr_in clientaddr;
    requestQueue* waiting_requests = (requestQueue*)malloc(sizeof(requestQueue));
    requestQueue* handled_requests = (requestQueue*)malloc(sizeof(requestQueue));
    serverArgs servArgs;
    
    //initializing structs and variables.
    pthread_cond_init(&cond_var_workers, NULL);
    pthread_cond_init(&cond_var_master, NULL);
    initRequestQueue(waiting_requests);
    initRequestQueue(handled_requests);
    initServerArgs(&servArgs, mutex, cond_var_workers, cond_var_master, waiting_requests, handled_requests, queue_size);
    getargs(&portnum, &num_threads, &queue_size, &schedalg, argc, argv);
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    initWorkerThreads(worker_threads, num_threads, waiting_requests, handled_requests);

    // server main operation.
    listenfd = Open_listenfd(portnum);
    while (1) {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        request* curr_request = initRequest(connfd);
        pickSchedAlgAndExecute(schedalg, curr_request, &servArgs);
    }
    destroyServer(worker_threads, num_threads, waiting_requests, handled_requests);
}
