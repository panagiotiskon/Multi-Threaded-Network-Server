// PANAGIOTIS KONTOEIDIS
// 1115201900266

#include "libs_pollSwayer.hpp"

int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, 0);

    if (argc != 4)
    {
        cout << " Wrong number of arguments" << endl;
        exit(1);
    }

    host = argv[1];
    port = atoi(argv[2]);
    filename = argv[3];
    int status;
    int num_of_lines=get_number_of_lines();   // get number of lines 
    pthread_t *threads = new pthread_t[num_of_lines];

    for(int i=0; i<num_of_lines; i++){
        int* u = new int;
        *u = i+1;
        if(pthread_create(&threads[i], NULL, send_vote, (void*)u)<0){
            perror_exit(" thread create ");
        }
    }

    for(int i=0; i<num_of_lines; i++){
        if(pthread_join(threads[i], (void **)&status)<0)
            perror_exit(" thread join");
    }

    pthread_exit(NULL);
    return 1;
}

int get_number_of_lines()
{
    char buf[256];
    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL)
        perror_exit(" fopen ");

    int lines_num = 0;
    while (fgets(buf, sizeof(buf), file))
    {
        lines_num++;
    }
    fclose(file);
    return lines_num;
}


void* send_vote(void* arg){
    int line = *((int*)arg);

    FILE *file = fopen(filename.c_str(), "r");

    char buf[256] = "";

    string name;
    string vote; 
    int curr =1;

    while(fgets(buf, sizeof(buf), file)){
        if(curr == line)
            break;
        curr++;
    }

    fclose(file);

    int flag=0;

    for(int i=0; i<sizeof(buf); i++)
    {
        if (flag == 0 && buf[i] != ' ')
        {
            name.push_back(buf[i]);
        }
        else if (flag == 0 && buf[i] == ' ')
        {
            name.push_back(buf[i]);
            flag = 1;
        }
        else if (flag == 1 && buf[i] != ' ')
        {
            name.push_back(buf[i]);
        }
        else if (flag == 1 && buf[i] == ' ')
        {
            name.push_back('\n');
            //name.push_back('\0');
            flag = 2;
        }
        else if (flag == 2 && buf[i] != '\n')
        {
            vote.push_back(buf[i]);
        }
        else if (flag == 2 && buf[i] == '\n')
        {
            vote.push_back('\n');
            //vote.push_back('\0');
            break;
        }
    }

    int sock;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit(" socket ");
    /* Find server address */
    if ((rem = gethostbyname(host)) == NULL)
    {
        herror(" gethostbyname ");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem -> h_addr, rem -> h_length);
    server.sin_port = htons(port); /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server)) < 0)
        perror_exit(" connect ");

    printf(" Connecting to % s port % d \n ", host, port);

    char buf2[1];
    char buf3[1];

    while (read(sock, buf2, 1) > 0)
    {
        if (buf2[0] != '\n')
        {
            putchar(buf2[0]);
        }
        else
        {
            putchar('\n');
            break;
        }
    }

    write(sock, name.c_str(), name.size());
    string temp;
    while (read(sock, buf2, 1) > 0)
    {
        if (buf2[0] != '\n')
        {
            putchar(buf2[0]);
            temp.push_back(buf2[0]);
        }
        else
        {
            putchar(buf2[0]);
            temp.push_back(buf2[0]);
            break;
        }
    }

    if(strcmp(temp.c_str(),"ALREADY VOTED\n")==0){
        close(sock);
        pthread_exit(NULL);
    }
    if(write(sock, vote.c_str(), vote.size())<0)
        perror("write ");


    while (read(sock, buf3, 1) > 0)
    {
        if (buf3[0] != '\n')
        {
            putchar(buf3[0]);
        }
        else
        {
            putchar('\n');
            break;
        }
    }
    
    close(sock);
    pthread_exit(NULL);
    return nullptr;
}


void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}