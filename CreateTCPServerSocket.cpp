#include <sys/socket.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#include <iostream>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include "sstream"
#include "string"
#define MAXPENDING 5

void DieWithError(const char *errorMessage);

int CreateTCPServerSocket(unsigned short port){
	int sock;
	struct sockaddr_in echoServAddr;

	if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		DieWithError("socket() failed");

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(port);

	if(bind(sock,(struct sockaddr *) &echoServAddr,sizeof(echoServAddr))<0)
		DieWithError("bind() failed");

	if(listen(sock,MAXPENDING)<0)
		DieWithError("listen() failed");

	return sock;
}
