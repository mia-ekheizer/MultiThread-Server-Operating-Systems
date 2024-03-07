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
cond_t cond_var;
mutex_t mutex;
int waiting_requests_size = 0;
// HW3: Parse the new arguments too
void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

void waitUntilRequestsArrive(struct* queue waiting_requests)
{
    mutex_lock(&mutex);
    while (waiting_requests_size == 0)
    {
        cond_wait(&cond_var, &mutex);
    }
    requestHandle(waiting_requests.head.fd);
    mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    getargs(&port, argc, argv);

    // 
    // HW3: Create some threads...
    //
    struct queue waiting_requests;
    struct queue curr_handled_requests;
    
    int num_threads = atoi(argv[2]);
    pthread_t worker_threads[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&worker_threads[i], NULL, waitUntilRequestsArrive, &waiting_requests);
    }

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	//
    waiting_requests.enqueue(connfd);

	Close(connfd);
    }
}
