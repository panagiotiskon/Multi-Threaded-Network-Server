
#include <stdio.h>
#include <sys/wait.h>	/* sockets */
#include <sys/types.h>	/* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h>		/* ge th os tb ya dd r */
#include <unistd.h>		/* fork */
#include <stdlib.h>		/* exit */
#include <ctype.h>		/* toupper */
#include <signal.h>		/* signal */
#include <pthread.h>
#include <iostream>

using namespace std;

void perror_exit(char *message);
void* tester(void *);

int main(int argc, char *argv[])
{
	int port, serversock, clientsock;
    
    int buff_size, num_threads;

	int status;

    pthread_t thr;
    
	struct sockaddr_in server, client;
	
	socklen_t clientlen;
	
	struct sockaddr *serverptr = (struct sockaddr *)&server;

	struct sockaddr *clientptr = (struct sockaddr *)&client;
	
	struct hostent *rem;

	if (argc != 2)
	{
		printf(" Please give port number \n ");
		exit(1);
	}


	port = atoi(argv[1]);
    // num_threads = atoi(argv[2]);
    // buff_size = atoi(argv[3]);


	/* Create socket */
	if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit(" failed to create socket ");

	server.sin_family = AF_INET; /* Internet domain */

	server.sin_addr.s_addr = htonl(INADDR_ANY);

	server.sin_port = htons(port); /* The given port */

	/* Bind socket to address */
	if (bind(serversock, serverptr, sizeof(server)) < 0)
		perror_exit(" failed to bind ");
	/* Listen for connections */
	if (listen(serversock, 5) < 0)
		perror_exit(" failed to listen ");

	printf(" Listening for connections to port % d \n ", port);
	while (1)
	{
		/* accept connection */
		if ((clientsock = accept(serversock, clientptr, &clientlen)) < 0)
			perror_exit("accept ");

        pthread_t test_thr;

	    if(pthread_create(&test_thr, NULL, tester, &clientsock) < 0)
            perror_exit(" failed to create thread ");

		if(pthread_join(test_thr, (void **) &status) < 0)
			perror_exit(" failed to join");

		pthread_exit(NULL);

		printf(" Accepted connection \n ");

        
        close(clientsock);
        close(serversock);
	}
	return 1;
}


void* tester(void* sock){
	cout<<" created a thread "<<endl;
	int* newsock = static_cast<int *>(sock);
	char buf[1];
	while (read(*newsock, buf, 1) > 0)
	{					 /* Receive 1 char */
		putchar(buf[0]); /* Print received char */
		/* Capitalize character */
		buf[0] = toupper(buf[0]);
		/* Reply */
		if (write(*newsock, buf, 1) < 0)
			perror_exit(" write ");
	}
	printf(" Closing connection .\n ");
	close(*newsock); /* Close socket */

	pthread_exit(NULL);
    return NULL;
}


void perror_exit(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}
