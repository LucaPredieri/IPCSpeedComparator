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
	double time;
	int r_fd = atoi(argv[1]);
	int bufSize = atoi(argv[2]);
	int buffer[SIZE];
	
	struct timespec finish;

	for (int i = 0; i < bufSize - 1; i++)
	{
		read(r_fd, &buffer[(i) % SIZE], sizeof(int));
	}

	clock_gettime(CLOCK_REALTIME, &finish);
	time = 1000000000 * (finish.tv_sec) + (finish.tv_nsec);
	int fd_fifo = open("my_timecons", O_WRONLY);
	write(fd_fifo, &time, sizeof(double));

	return 0;
}
