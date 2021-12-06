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

    int sockfd, portno, n;
    int flag = 1;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int bufSize = atoi(argv[3]);
    int buffer[SIZE];
    if (argc < 4)
    {
        fprintf(stderr, "usage %s hostname port bufferSize\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
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
