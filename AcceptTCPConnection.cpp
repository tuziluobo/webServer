#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */

void DieWithError(const char *errorMessage);

int AcceptTCPConnection(int servSock){
	int clntSock;
	struct  sockaddr_in echoClntAddr;
	unsigned int clntLen;

	clntLen = sizeof(echoClntAddr);

	if((clntSock = accept(servSock,(struct sockaddr *) &echoClntAddr, &clntLen))<0)
		DieWithError("accept() failed");
	return clntSock;
}
