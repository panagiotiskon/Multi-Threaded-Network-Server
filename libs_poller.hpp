
#include <stdio.h>
#include <sys/wait.h>	/* sockets */
#include <sys/types.h>	/* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h>		/* ge th os tb ya dd r */
#include <unistd.h>		/* fork */
#include <stdlib.h>		/* exit */
#include <ctype.h>		/* toupper */
#include <csignal>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include <cstring> // for strcpy
#include <fcntl.h>

#define THR_LIMIT 100
using namespace std;

queue <int* > buffer;
vector<pair<string, string>> votes;
vector<string> names;
vector<string> political_parties;

int signal_flag = 0;
int poll_log_file = -1;
int poll_stat_file = -1;

const char* poll_stat_name;

pthread_mutex_t  buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  names_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  parties_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  votes_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t  poll_lock = PTHREAD_MUTEX_INITIALIZER;


pthread_cond_t buffer_nonempty = PTHREAD_COND_INITIALIZER; //empty buffer in worker
pthread_cond_t buffer_nonfull = PTHREAD_COND_INITIALIZER;   //full buffer in master 

void perror_exit(char *message);
void* get_vote(void* arg);
void signal_handler(int signum);
