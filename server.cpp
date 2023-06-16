#include "libs_server.hpp"

int main(int argc, char *argv[])
{

	int port, serversock, clientsock;
	int buff_size, num_threads;
	int status;

	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr *)&server;
	struct sockaddr *clientptr = (struct sockaddr *)&client;
	struct hostent *rem;

	struct sigaction sa;

	sa.sa_sigaction = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &sa, NULL);

	if (argc < 2)
	{
		printf(" Please give port number \n ");
		exit(1);
	}

	port = atoi(argv[1]);
	num_threads = atoi(argv[2]);
	buff_size = atoi(argv[3]);
	const char *poll_log_name = argv[4];
	poll_stat_name = argv[5];

	if (num_threads > THR_LIMIT)
	{
		cout << "Please give less threads\n";
		exit(1);
	}

	poll_log_file = open(poll_log_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // create poll log file

	if (poll_log_file == -1)
	{
		perror_exit(" failed to open file");
	}

	if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* Create socket */
		perror_exit(" failed to create socket ");

	server.sin_family = AF_INET; /* Internet domain */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port); /* The given port */

	if (bind(serversock, serverptr, sizeof(server)) < 0) /* Bind socket to address */
		perror_exit(" failed to bind ");

	if (listen(serversock, 128) < 0) /* Listen for connections */
		perror_exit(" failed to listen ");

	printf(" Listening for connections to port % d \n ", port);

	pthread_mutex_init(&buffer_lock, 0);
	pthread_mutex_init(&names_lock, 0);
	pthread_mutex_init(&parties_lock, 0);
	pthread_mutex_init(&votes_lock, 0);
	pthread_mutex_init(&poll_lock, 0);
	pthread_cond_init(&buffer_nonempty, 0);
	pthread_cond_init(&buffer_nonfull, 0);

	pthread_t *threads = new pthread_t[num_threads];

	for (int i = 0; i < num_threads; i++) // create threads
	{
		if (pthread_create(&threads[i], NULL, get_vote, NULL) < 0)
			perror_exit(" failed to create thread ");
	}

	while (!signal_flag)
	{
		if ((clientsock = accept(serversock, clientptr, &clientlen)) < 0)
			perror_exit("failed to accept ");

		int *client_ptr = new int[sizeof(int)];
		*client_ptr = clientsock;

		pthread_mutex_lock(&buffer_lock);

		while (buffer.size() == buff_size)
		{ // wait for worker to empty buffer
			pthread_cond_wait(&buffer_nonfull, &buffer_lock);
		}
		buffer.push(client_ptr); // then push new connection to buffer
		pthread_cond_signal(&buffer_nonempty);
		pthread_mutex_unlock(&buffer_lock);
	}

	for (int i = 0; i < num_threads; i++) // wait for threads to join
	{
		if (pthread_join(threads[i], (void **)&status) < 0)
			perror_exit(" failed to join");
	}

	pthread_exit(NULL);

	close(clientsock);
	close(serversock);

	delete[] threads;
	return 1;
}

void *get_vote(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&buffer_lock);

		while (buffer.size() == 0) 		// wait if buffer is empty
		{
			pthread_cond_wait(&buffer_nonempty, &buffer_lock);
		}

		int *client_sock = buffer.front();
		buffer.pop();

		pthread_cond_signal(&buffer_nonfull);
		pthread_mutex_unlock(&buffer_lock);

		char buf[1];

		int *newsock = nullptr;
		newsock = client_sock;
		write(*newsock, "SEND NAME PLEASE\n", 18);

		vector<char> temp_name;
		vector<char> temp_party;

		while (read(*newsock, buf, 1) > 0)
		{					 /* Receive 1 char */
			putchar(buf[0]); /* Print received char */

			if (buf[0] != '\n' && buf[0] != '\r')
				temp_name.push_back(buf[0]);

			if (buf[0] == '\n')
				break;
		}

		string temp_name2(temp_name.begin(), temp_name.end());
		pthread_mutex_lock(&names_lock); // lock names vector

		if (!names.empty())
		{
			if (count(names.begin(), names.end(), temp_name2))
			{
				write(*newsock, "ALREADY VOTED\n", 14);
				printf("Closing connection .\n ");
				close(*newsock); /* Close socket */
				pthread_mutex_unlock(&names_lock);
				continue;
			}
			else
			{
				names.push_back(temp_name2);
			}
		}
		else
		{
			names.push_back(temp_name2);
		}
		pthread_mutex_unlock(&names_lock);
		write(*newsock, "SEND VOTE PLEASE\n", 18);
		while (read(*newsock, buf, 1) > 0)
		{
			putchar(buf[0]);
			if (buf[0] != '\n' && buf[0] != '\r')
				temp_party.push_back(buf[0]);

			if (buf[0] == '\n')
				break;
		}
		string temp_vote(temp_party.begin(), temp_party.end());
		pthread_mutex_lock(&parties_lock);
		if (!political_parties.empty())
		{
			if (count(political_parties.begin(), political_parties.end(), temp_vote) == 0)
			{
				political_parties.push_back(temp_vote);
			}
		}
		else
		{
			political_parties.push_back(temp_vote); // add to political parties vector
		}
		pthread_mutex_unlock(&parties_lock);

		pthread_mutex_lock(&votes_lock);
		votes.push_back(make_pair(temp_name2, temp_vote)); // add to votes
		pthread_mutex_unlock(&votes_lock);

		pthread_mutex_lock(&poll_lock);
		char s[temp_name2.size() + temp_vote.size() + 1];
		strcpy(s, temp_name2.c_str());
		strcat(s, " ");
		strcat(s, temp_vote.c_str());
		strcat(s, "\n");
		write(poll_log_file, s, strlen(s));
		pthread_mutex_unlock(&poll_lock);

		string s2 = "VOTE for Party " + temp_vote + " RECORDED\n";
		write(*newsock, s2.c_str(), s2.size());
		temp_name.clear();
		temp_name2.clear();
		temp_vote.clear();

		printf("Closing connection .\n ");
		close(*newsock); /* Close socket */
	}

	pthread_exit(NULL);
	return NULL;
}

void perror_exit(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

void signal_handler(int, siginfo_t *, void *)
{
	signal_flag = 1;

	poll_stat_file = open(poll_stat_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // create poll log file

	if (!votes.empty()) // if there are votes
	{
		if (political_parties.empty())
		{
			string s = "TOTAL 0";
			write(poll_stat_file, s.c_str(), s.size());
			exit(1);
		}
		else
		{
			int total = 0;
			sort(political_parties.begin(), political_parties.end());
			for (int i = 0; i < political_parties.size(); i++) // for every political party find its number of occurances in the votes vector
			{
				int count = 0;
				for (int k = 0; k < votes.size(); k++)
				{
					if (political_parties[i] == votes[k].second)
						count++;
				}
				total+=count;
				string s = political_parties[i] + " " + to_string(count) + "\n"; // write the number of votes for every party in poll-stat
				write(poll_stat_file, s.c_str(), s.size());
			}

			string s2 = "TOTAL "+to_string(total);
			write(poll_stat_file, s2.c_str(), s2.size());
		}
	}
}
