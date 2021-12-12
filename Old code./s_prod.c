/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
   gcc server2.c -lsocket
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 200000

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

void dostuff(int, int); /* function prototype */
void error(char *msg)
{
    perror(msg);
    exit(1);
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

    int sockfd, newsockfd, portno, pid;

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    int option = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
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

    dostuff(newsockfd, atoi(argv[2]));
    close(newsockfd);
    close(sockfd);
    //sleep(5);
    return 0; /* we never get here */
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
