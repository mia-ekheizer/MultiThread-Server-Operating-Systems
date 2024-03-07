#include "segel.h"
#include "request.h"
#include <pthread.h>

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

// HW3: Parse the new arguments too
void getargs(int *portnum, int *threads, int* queue_size, char** schedalg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *portnum = atoi(argv[1]);
    *threads = atoi(argv[2]);
    if (!threads || *threads <= 0) {
        exit(1);
    }
    *queue_size = atoi(argv[3]);
    if (!queue_size || *queue_size <= 0) {
        exit(1);
    }
    *schedalg = argv[4];
    if (*schedalg != "block" && *schedalg != "dt" && *schedalg != "dh" && *schedalg != "bf" && *schedalg != "random") {
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
    char* schedalg;
    struct sockaddr_in clientaddr;
    struct requestQueue waiting_requests, handled_requests;
    getargs(&portnum, &threads, &queue_size, &schedalg, argc, argv);
    pthread_cond_init(&cond_var_workers, NULL);
    pthread_cond_init(&cond_var_master, NULL);

    // initializing the request queues.
    initRequestQueue(waiting_requests);
    initRequestQueue(handled_requests);

    // initializing the worker threads.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    for (int i = 0; i < num_threads; i++)
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
        mutex_lock(&mutex);
        // if the server has reached the maximal amount of requests, it waits.
        while (handled_requests_size + waiting_requests_size > queue_size) { 
            pthread_cond_wait(&cond_var_master, &mutex);
        }
        // once there is enough space to handle another request, the server continues.
        enqueue(waiting_requests, curr_request);
        waiting_requests_size++;
        // the master thread signals to the other threads that there are waiting requests.
        pthread_cond_signal(&cond_var_workers);
        mutex_unlock(&mutex);
	    Close(connfd);
    }
    pthread_cond_destroy(cond_var_master);
    pthread_cond_destroy(cond_var_workers);
    free(worker_threads);
}
