#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 200000

int main(int argc, char *argv[])
{

  // printf("waiting \n");
  double time;
  int bufSize = atoi(argv[2]);
  int buffer[SIZE];

  struct timespec finish;
  int fd = open(argv[1], O_RDONLY);

  for (int i = 0; i < bufSize - 1; i++)
  {
    read(fd, &buffer[(i) % SIZE], sizeof(int));
  }

  clock_gettime(CLOCK_REALTIME, &finish);
  time = 1000000000 * (finish.tv_sec) + (finish.tv_nsec);

  int fd_fifo = open("my_timecons", O_WRONLY);
  write(fd_fifo, &time, sizeof(double));
  //printf("\n\ntime= %lf \n", time);

  //close(fd);
  //close(fd_fifo);
  return 0;
}
