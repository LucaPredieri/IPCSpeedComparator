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
		buf[i] = rand() % 10;
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
	
	int w_fd = atoi(argv[1]);

	
	double time;
	struct timespec start, finish;

	clock_gettime(CLOCK_REALTIME, &start);

	for (int i = 0; i < bufSize - 1; i++)
	{
		write(w_fd, &buffer[(i) % SIZE], sizeof(int));
	}


	time = 1000000000 * (start.tv_sec) + (start.tv_nsec);

	int fd_fifo = open("my_timeprod", O_WRONLY);
	write(fd_fifo, &time, sizeof(double));

	//sleep(10);
	return 0;
}
