
#include <stdio.h>
#include <sys/wait.h>	
#include <sys/types.h>	
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>		
#include <unistd.h>		
#include <stdlib.h>		
#include <ctype.h>		
#include <csignal>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>
#include <cstring>
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
bool compare(pair<string, int>& a, pair<string, int>& b);
