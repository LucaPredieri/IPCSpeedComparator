#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 200000

// Fabio = Pig

#define CHECK(X) (                                                 \
    {                                                              \
        int __val = (X);                                           \
        (__val == -1 ? (                                           \
                           {                                       \
                               fprintf(stderr, "ERROR ("__FILE__   \
                                               ":%d) -- %s\n",     \
                                       __LINE__, strerror(errno)); \
                               exit(EXIT_FAILURE);                 \
                               -1;                                 \
                           })                                      \
                     : __val);                                     \
    })


void dostuff(int sock, int bufSize);

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void fillBuffer(int buf[], int bufSize)
{
    for (int i = 0; i < bufSize; i++)
    {
        buf[i] = rand() % 9;
    }
}

int main(int argc, char *argv[])
{   
        int sockfd, n;
        struct hostent *server;
        int bufSize = atoi(argv[3]);
        int buffer[SIZE];

        int newsockfd, pid;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;

        int option = 1;
        int portno = atoi(argv[1]);

    // Producer Process (Child).

    if(fork() == 0){

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) error("ERROR opening socket");

        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        if (bind(sockfd, (struct sockaddr *)&serv_addr,
                 sizeof(serv_addr)) < 0)
            error("ERROR on binding");

        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        dostuff(newsockfd, atoi(argv[3]));
        close(newsockfd);
        close(sockfd);
        //sleep(5);
        return 0; /* we never get here */
    }

    // Consumer Process (Parent).

    else{

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");
        server = gethostbyname(argv[2]);
        if (server == NULL)
        {
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }
        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,
              (char *)&serv_addr.sin_addr.s_addr,
              server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR connecting");
            
            
        bzero(buffer, SIZE);
        
        double time;
        struct timespec finish;

        for (int i = 0; i < bufSize; i++)
        {
            CHECK(read(sockfd, &buffer[(i) % SIZE], sizeof(int)));
        }

        clock_gettime(CLOCK_REALTIME, &finish);
        time = 1000000000 * (finish.tv_sec) + (finish.tv_nsec);

        int fd_fifo = open("my_timecons", O_WRONLY);
        write(fd_fifo, &time, sizeof(double));
        //sleep(5);
        
        //close(fd_fifo);

        return 0;
    }
}


/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/

void dostuff(int sock, int bufSize)
{
    // printf("dajeeee");
    double time;
    struct timespec start;
    //get the current time (before semaphore)

    int buffer[SIZE];
    bzero(buffer, SIZE);
    
    if (bufSize<SIZE)
    {
        fillBuffer(buffer, bufSize);
    }
    else
    {
        fillBuffer(buffer, SIZE);
    }

    clock_gettime(CLOCK_REALTIME, &start);
    time = 1000000000 * (start.tv_sec) + (start.tv_nsec);

    for (int i = 0; i < bufSize; i++)
    {
        CHECK(write(sock, &buffer[(i) % SIZE], sizeof(int)));
    }

    int fd_fifo = open("my_timeprod", O_WRONLY);
    write(fd_fifo, &time, sizeof(double));
    
    //close(fd_fifo);
}