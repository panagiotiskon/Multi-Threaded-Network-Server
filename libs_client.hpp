//PANAGIOTIS KONTOEIDIS
//1115201900266

#include <stdio.h>
#include <string.h>
#include <netinet/in.h> /* i n t e r n e t s o c k e t s */
#include "fcntl.h"
#include "sys/wait.h"
#include <unistd.h>
#include <iostream>
#include <vector>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>

#define MAX_LINE_SIZE 256

using namespace std;

string filename;
const char* host;
int port;


pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;


void* send_vote(void *);
int get_number_of_lines(void);
void perror_exit ( char * message );
