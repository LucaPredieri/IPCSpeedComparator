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

	struct timespec start, finish;
	double time;
	int buffer[SIZE];
	int bufSize = atoi(argv[2]);

	// Producer Process (Child).

	if(fork() == 0){


		if (bufSize < SIZE){
	    	fillBuffer(buffer, bufSize);
	    }
		else{
			fillBuffer(buffer, SIZE);
		}
		
		int fd = open(argv[1], O_WRONLY);
		clock_gettime(CLOCK_REALTIME, &start);

		for (int i = 0; i < bufSize - 1; i++){
	    	write(fd, &buffer[(i) % SIZE], sizeof(int));
		}
		time = 1000000000 * (start.tv_sec) + (start.tv_nsec);
		int fd_fifo = open("my_timeprod", O_WRONLY);
		write(fd_fifo, &time, sizeof(double));

		// printf("\n\ntime= %lf \n", time);
		
		close(fd);
		//close(fd_fifo);
		//sleep(10);
		return 0;
	}

	// Consumer Process (Parent).

	else{

		int fd = open(argv[1], O_RDONLY);

		for (int i = 0; i < bufSize - 1; i++){
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
}