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
#include <sys/mman.h>
#include <semaphore.h>

#define SIZE 200000

// Function: CHECK(X)
// This function writes on the shell whenever a sistem call returns any kind of error.
// The function will print the name of the file and the line at which it found the error.
// It will end the check exiting with code 1.
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

void fillBuffer(int buf[], int bufSize)
{
	for (int i = 0; i < bufSize; i++)
	{
		buf[i] = rand() % 9;
	}
}

int main(int argc, char *argv[])
{

	int buffer_size = atoi(argv[1]);
	int circular_buffer_size = atoi(argv[2]);
	sem_t *not_empty = sem_open("/sem_1", O_CREAT, S_IRUSR | S_IWUSR, 0);
	sem_t *not_full = sem_open("/sem_2", O_CREAT, S_IRUSR | S_IWUSR, buffer_size - 1);
	sem_t *mutex = sem_open("/sem_3", O_CREAT, S_IRUSR | S_IWUSR, 1);

	int *buffer = mmap(NULL, buffer_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (buffer == MAP_FAILED)
	{
		printf("Mapping Failed\n");
		fflush(stdout);
		return -1;
	}

	// Child Pro-duce.

	if (fork() == 0)
	{
		int in = 0;
		int counter = 0;
		double time;
		struct timespec start;
		int producer_arr[SIZE];

		if (buffer_size < SIZE)
		{
			fillBuffer(producer_arr, buffer_size);
		}
		else
		{
			fillBuffer(producer_arr, SIZE);
		}
		clock_gettime(CLOCK_REALTIME, &start);
		time = 1000000000 * (start.tv_sec) + (start.tv_nsec);
		while (counter < buffer_size)
		{
			sem_wait(not_full);
			sem_wait(mutex);
			buffer[in] = producer_arr[counter++ % SIZE];
			in = (in + 1) % circular_buffer_size;
			sem_post(mutex);
			sem_post(not_empty);
		}
		int fd_fifo = open("/tmp/my_time_p", O_WRONLY);
		write(fd_fifo, &time, sizeof(double));
		sleep(5);
		// return 0;
	}

	// Parent consumer.

	else
	{
		int out = 0;
		int x = 0;
		int counter = 0;
		double time;
		int receiver_arr[SIZE];
		struct timespec finish;
		while (counter < buffer_size)
		{
			sem_wait(not_empty);
			sem_wait(mutex);
			receiver_arr[counter++ % SIZE] = buffer[out];
			fflush(stdout);
			out = (out + 1) % circular_buffer_size;
			sem_post(mutex);
			sem_post(not_full);
		}
		clock_gettime(CLOCK_REALTIME, &finish);
		time = 1000000000 * (finish.tv_sec) + (finish.tv_nsec);
		int fd_fifo = open("/tmp/my_time_c", O_WRONLY);
		write(fd_fifo, &time, sizeof(double));
		sleep(5);
	}

	int err;
	CHECK(err = munmap(buffer, buffer_size));

	return 0;
}
