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

// Colors for konsole prints.
const char *red = "\033[0;31m";
const char *bhred = "\e[1;91m";
const char *green = "\033[1;32m";
const char *yellow = "\033[1;33m";
const char *cyan = "\033[0;36m";
const char *magenta = "\033[0;35m";
const char *bhgreen = "\e[1;92m";
const char *bhyellow = "\e[1;93m";
const char *bhblue = "\e[1;94m";
const char *bhmagenta = "\e[1;95m";
const char *bhcyan = "\e[1;96m";
const char *bhwhite = "\e[1;97m";
const char *reset = "\033[0m";

typedef enum {
    false,
    true
}bool;

bool mode = true;
bool cycle = true;

#define MODENUMBER 4

// Declaring Functions
int spawn(const char *pogram, char **arg_list);
void create_fifo(const char *name);
int mode_function(int arr[MODENUMBER]);
void replace(int arr[MODENUMBER], int x);


// Function: CHECK(X)
// This function writes on the shell whenever a sistem call returns any kind of error.
// The function will print the name of the file and the line at which it found the error.
// It will end the check exiting with code 1.

#define CHECK(X) (                                             \
    {                                                          \
      int __val = (X);                                         \
      (__val == -1 ? (                                         \
                         {                                     \
                           fprintf(stderr, "ERROR ("__FILE__   \
                                           ":%d) -- %s\n",     \
                                   __LINE__, strerror(errno)); \
                           exit(EXIT_FAILURE);                 \
                           -1;                                 \
                         })                                    \
                   : __val);                                   \
    })

int circular_buffer_size = 0;
char circular_buffer_size_s[80];
pid_t pid_cb;

int main(int argc, char const *argv[]){

	if (argc != 1){
    	fprintf(stderr, "Usage:%s <filename>\n", argv[0]);
    	exit(-1);
	}

	char buffer_size_s[80];
	char YesNo;

	double time_p, time_c;

  	create_fifo("/tmp/my_time_p");
  	create_fifo("/tmp/my_time_c");

	int fd_producer, fd_consumer;
	int buffer_size;
	int input_mode;

	int modes_array[MODENUMBER] = {1,2,3,4};

	//unnamed pipe
  	int fd_up[2];

  	//named pipe
  	char *named_pipe = "/tmp/named_pipe";

  	printf("\nPID master [%d]\n", getpid());
  	fflush(stdout);

  	while(1){

  		switch(mode){

  			case true:

  				printf("\n\n%s Insert buffer size [MB] smaller than 100MB or insert [0] to quit:%s \n", bhblue, reset);
  				scanf("%d", &buffer_size);
  				cycle = true;
  				if (buffer_size > 100 || buffer_size < 0 || !buffer_size){
        			printf("%sWrong value! try again:%s\n", bhred, reset);
       				fflush(stdout);
     			}
     			if(buffer_size==0) return 0;

     			buffer_size = buffer_size*250000;

     			sprintf(buffer_size_s, "%d", buffer_size);

  			break;

  			case false:
  				
  				while(getchar() != '\n');
  				printf("\n Would you like to try the same buffer size with another mode? [Y/n]  ,  to quit spacebar: ");
  				YesNo = getchar();

  				if(YesNo == 'y' || YesNo == 'Y'){
  					printf(" OK Let's go");
  					cycle = true;
  				}
  				if(YesNo == 'n' || YesNo == 'N'){
  				 	printf(" OKeeee");
  				 	for(int i=0;i<MODENUMBER; i++) modes_array[i] = i+1;
  				 	mode = true;
  				 	cycle = false;
  				}
  				if(YesNo == 32) return 0;
  			
  			break;
  		}

  		while(cycle){

			printf("\nWhich data sharing mode do you want to use? Press: \n");
			input_mode = mode_function(modes_array);

			if (input_mode != 1 && input_mode != 2 && input_mode != 3 && input_mode != 4){
				printf("%sWrong key! try again:%s\n", bhred, reset);
				fflush(stdout);
			}
			else{
				mode =  false;
			}

  			switch(input_mode){

  				// Unnamed Pipes
  				case 1:

  					printf("\n%s[1]: Un-named pipes%s\n", bhyellow, reset);
      				printf("%s-----------------------%s\n", bhyellow, reset);
      				printf("Sending data...\n"); fflush(stdout);

      				char fd_1[80];
      				char fd_2[80];


					if (pipe(fd_up) == -1){
						perror("Error creating unnamed fifo\n");
						exit(1);
					}

					// Assigning the two variables for reading and writing 
					// the unnamed pipe.

					sprintf(fd_1, "%d", fd_up[1]);
					sprintf(fd_2, "%d", fd_up[0]);

					char *arg_list_up[] = {"./up", fd_1, fd_2, buffer_size_s, NULL};

					// Spawning of the other processes.

					pid_t pid_up = spawn("./up", arg_list_up);

					replace(modes_array,1);

					cycle = false;

					printf("PID UP [%d]\n", pid_up); fflush(stdout);

  				break;

  				// Named Pipes
  				case 2:

  					printf("\n%s[2]: Named pipes:%s\n", bhgreen, reset);
 					printf("\n%s-----------------------%s\n", bhgreen, reset);
					printf("Sending data...\n"); fflush(stdout);

					// Creating pipes.

					create_fifo(named_pipe);

					// Creating argument list.
					char *arg_list_np[] = {"./np", named_pipe, buffer_size_s, NULL};

 					// Spawning of the other processes.

					pid_t pid_np = spawn("./np", arg_list_np);

					replace(modes_array,2);

					cycle = false;

					printf("PID Named Pipe [%d]\n", pid_np); fflush(stdout);

  				break;

  				// Sockets
  				case 3:

  					printf("\n%s[3]: Sockets:%s\n", bhmagenta, reset);
					printf("\n%s-----------------------%s\n", bhmagenta, reset);
					printf("Sending data...\n"); fflush(stdout);

					// Initialization of the array of data needed for using the execvp() function
					// contained in the spawn() function.

					char *arg_list_socket[] = {"./socket", "5096", "127.0.0.1", buffer_size_s, (char *)NULL};

					// Spawning processes.

					pid_t pid_socket = spawn("./socket", arg_list_socket);

					replace(modes_array,3);

					cycle = false;

    				printf("PID socket [%d]\n", pid_socket); fflush(stdout);


  				break;
  				// Circular Buffer
  				case 4:

					printf("\nHow big should the circular-buffer memory be [KB]? (0.1-10): \n");
					do{
						scanf("%d", &circular_buffer_size);
						if (circular_buffer_size <= 0 || circular_buffer_size > 10){
							printf("%sWrong mem! try again:%s\n", bhred, reset);
							fflush(stdout);
						}
      				} while (circular_buffer_size <= 0 || circular_buffer_size > 10);
      
      
					circular_buffer_size=circular_buffer_size*250;

					sprintf(circular_buffer_size_s, "%d", circular_buffer_size);

					printf("\n%s[4]: Shared memory with circular buffer:%s\n", bhcyan, reset);
					printf("\n%s-----------------------%s\n", bhcyan, reset);
					printf("Sending data...\n");
					fflush(stdout);
					char *arg_list_1[] = {"./cb", buffer_size_s, circular_buffer_size_s, (char *)NULL};
					pid_cb = spawn("./cb", arg_list_1);

					replace(modes_array,4);

					cycle = false;

					printf("PID Circular-buffer [%d]\n", pid_cb); fflush(stdout);

  				break;

			}
			
	
			CHECK(fd_producer = open("/tmp/my_time_p", O_RDONLY));
 			CHECK(fd_consumer = open("/tmp/my_time_c", O_RDONLY));

			sleep(1);
		
			int status;
			CHECK(wait(&status));
	
			if (WIFEXITED(status))
			{
			  CHECK(WEXITSTATUS(status));
			}

			read(fd_producer, &time_p, sizeof(double));
			read(fd_consumer, &time_c, sizeof(double));

			double timediff = time_c - time_p;

			time_p = time_p / 1000000000;
			time_c = time_c / 1000000000;
			printf("TIME 1 IS: %lf\n", time_p); fflush(stdout);
			printf("TIME 2 IS: %lf\n", time_c); fflush(stdout);

			timediff = timediff / 1000000000;
			printf("TIME DIFFERENCE IS: %lf\n", timediff); fflush(stdout);

			switch (input_mode)
		    {
		    case 1:
		      close(fd_up[0]);
		      close(fd_up[1]);
		      printf("%s-----------------------%s\n", bhyellow, reset);
		      fflush(stdout);
		      break;
		    case 2:
		      unlink(named_pipe);
		      printf("%s-----------------------%s\n", bhgreen, reset);
		      fflush(stdout);
		      break;

		    case 3:
		      printf("%s-----------------------%s\n", bhmagenta, reset);
		      fflush(stdout);
		      break;

		    case 4:
		      printf("%s-----------------------%s\n", bhcyan, reset);
		      fflush(stdout);
		      break;
		    }
		}
  	}

	unlink("/tmp/my_time_p");
	unlink("/tmp/my_time_c");
	return 0;
}

void replace(int arr[MODENUMBER],int x){

	for(int i = 0 ; i < MODENUMBER ; i++){
		if(arr[i]==x){
			arr[i]=0;
		}
	}
}


int mode_function(int arr[MODENUMBER]){

	int input;
	for(int i=0 ; i<MODENUMBER ; i++){
		switch(arr[i]){
			case 1:
				printf("\n [1]: Un-named pipes\n");
			break;
			case 2: 
				printf("\n [2]: Named pipes\n");
			break;
			case 3:
				printf("\n [3]: Sockets\n");
			break;
			case 4:
				printf("\n [4]: Shared Memory with Circular Buffer\n");
			break;

			case 0:

				printf("\n ---------------- \n");

			break;
		}
	}

	scanf("%d", &input);

	return input;
}

void create_fifo(const char *name){

  // automatically checks for errors.
  if (mkfifo(name, 0666) == -1){

    if (errno != EEXIST){

      perror("Error creating named fifo\n");
      exit(1);
    }
  }
}


// Function: spawn(__,__)
// This function forks the program and, on the child process, calls the function execvp()
// with the parameters passed as arguments of spawn().
int spawn(const char *program, char **arg_list){

  pid_t child_pid = fork();
  CHECK(child_pid);

  if (child_pid != 0) {return child_pid;}

  else{
    execvp(program, arg_list);
    // Check for errors of execvp.
    perror("exec failed");
    return 1;
  }
}

