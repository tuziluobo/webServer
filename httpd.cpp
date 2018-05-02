#include <iostream>
#include "httpd.h"
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include <pthread.h>  
#include "CalcFramer.hpp"
#include "CalcParser.hpp"
#include "sstream"
#include "string"
#define MAXPENDING 5    /* Maximum outstanding connection requests */
using namespace std;

void DieWithError(const char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket, string doc_root);   /* TCP client handling function */
void *ThreadMain(void *arg);  
  
struct ThreadArgs {  
    int client_sock;  
};  
string root;
void start_httpd(unsigned short port, string doc_root)
{
	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;
   
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
 
    unsigned short echoServPort;     /* Server port */

    pthread_t threadID;
    struct ThreadArgs *threadArgs;

    root = doc_root;
    echoServPort = port;  /* First arg:  local port */

    //    DieWithError("listen() failed");

    servSock = CreateTCPServerSocket(echoServPort);
    for (;;) /* Run forever */
    {
 
        clntSock = AcceptTCPConnection(servSock);
       
        if((threadArgs = (struct ThreadArgs*) malloc(sizeof (struct ThreadArgs)))==NULL)   
            DieWithError("malloc() failed!"); 

        threadArgs->client_sock = clntSock;

        if(pthread_create(&threadID,NULL,ThreadMain,(void *) threadArgs)!=0)
            DieWithError("pthread_create() failed");
        printf("with thread %lu \n", (unsigned long int)threadID);  
    }
    close(servSock);  
}

void *ThreadMain(void* arg) {  
    pthread_detach(pthread_self());  
    int client_sock = ((struct ThreadArgs*) arg)->client_sock;  
    free(arg);  
    struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
               sizeof(timeout)) < 0)
        {
            DieWithError("setsockopt SO_RCVTIMEO");
        }

        if (setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
               sizeof(timeout)) < 0)
        {
            DieWithError("setsockopt SO_SNDTIMEO");
        }
    HandleTCPClient(client_sock,root);  
    return NULL;
}  
void DieWithError(const char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

