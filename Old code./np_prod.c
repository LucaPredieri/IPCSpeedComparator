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

void fillBuffer(int buf[], int bufSize)
{
  for (int i = 0; i < bufSize; i++)
  {
    buf[i] = rand() % 9;
  }
}

int main(int argc, char *argv[])
{
  int bufSize = atoi(argv[2]);
  int buffer[SIZE];

  if (bufSize < SIZE)
  {
    fillBuffer(buffer, bufSize);
  }
  else
  {
    fillBuffer(buffer, SIZE);
  }

  double time;
  struct timespec start, finish;
  int fd = open(argv[1], O_WRONLY);

  clock_gettime(CLOCK_REALTIME, &start);
  for (int i = 0; i < bufSize - 1; i++)
  {
    write(fd, &buffer[(i) % SIZE], sizeof(int));
  }

  time = 1000000000 * (start.tv_sec) + (start.tv_nsec);
  int fd_fifo = open("my_timeprod", O_WRONLY);
  write(fd_fifo, &time, sizeof(double));

  //printf("\n\ntime= %lf \n", time);

  close(fd);
  //close(fd_fifo);

  //sleep(10);
  return 0;
}
