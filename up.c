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

void bar(int percent_done);

void fillBuffer(int buf[], int bufSize){
	for (int i = 0; i < bufSize; i++)
	{
		buf[i] = rand() % 10;
	}
}
int main(int argc, char *argv[]){

	double time;
	int buffer[SIZE];
	int bufSize = atoi(argv[3]);
	struct timespec start, finish;

	// Producer Process.

	if(fork() == 0){

		if (bufSize < SIZE) fillBuffer(buffer, bufSize);
		else fillBuffer(buffer, SIZE);
		
		int w_fd = atoi(argv[1]);

		clock_gettime(CLOCK_REALTIME, &start);

		for (int i = 0; i < bufSize - 1; i++)
		{

			write(w_fd, &buffer[(i) % SIZE], sizeof(int));
		}

		time = 1000000000 * (start.tv_sec) + (start.tv_nsec);

		int fd_fifo = open("/tmp/my_time_p", O_WRONLY);
		write(fd_fifo, &time, sizeof(double));

		//sleep(10);
		return 0;
	}

	// Consumer Process.

	else{

		int r_fd = atoi(argv[2]);
		for (int i = 0; i < bufSize - 1; i++)
		{
			if(i % bufSize/100 == 0){
				bar(i);
				usleep(20000);
			}
			
			read(r_fd, &buffer[(i) % SIZE], sizeof(int));
		}
		printf("\n");
		clock_gettime(CLOCK_REALTIME, &finish);

		time = 1000000000 * (finish.tv_sec) + (finish.tv_nsec);
		int fd_fifo = open("/tmp/my_time_c", O_WRONLY);
		write(fd_fifo, &time, sizeof(double));

		return 0;
	}
}


void bar(int percent_done){

	const int PROG_BAR_LENGHT = 30;

	int num_chars = percent_done * PROG_BAR_LENGHT / 100 ;

	printf("\r[");
	for(int i = 0 ; i < num_chars ; i++){
		printf("#");
	}
	for(int i = 0 ; i < PROG_BAR_LENGHT - num_chars; i++){
		printf(" ");
	}
	printf("] %d%% DONE", percent_done);
	fflush(stdout);
}