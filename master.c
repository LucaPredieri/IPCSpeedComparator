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

// Function: spawn(__,__)
// This function forks the program and, on the child process, calls the function execvp()
// with the parameters passed as arguments of spawn().
int spawn(const char *program, char **arg_list)
{

  pid_t child_pid = fork();
  CHECK(child_pid);
  if (child_pid != 0)
    return child_pid;

  else
  {
    execvp(program, arg_list);
    // Check for errors of execvp.
    perror("exec failed");
    return 1;
  }
}

void create_fifo(const char *name)
{
  // automatically checks for errors.
  if (mkfifo(name, 0666) == -1)
  {
    if (errno != EEXIST)
    {
      perror("Error creating named fifo\n");
      exit(1);
    }
  }
}

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

int main(int argc, char *argv[])
{
  // Check for correct number of arguments.
  if (argc != 1)
  {
    fprintf(stderr, "usage:%s <filename>\n", argv[0]);
    exit(-1);
  }
  //for everyone
  char bufSize_s[80];
  int mode[4] = {1, 2, 3, 4};

  int input_mode = 0;
  pid_t pid_consumer, pid_producer, pid_cb;
  double timeprod, timecons;
  create_fifo("my_timeprod");
  create_fifo("my_timecons");
  int fd_fifoprod, fd_fifocons;
  int bufSize;

  //unnamed pipe
  int fd_up[2];

  //named pipe
  char *named_pipe = "/tmp/named_pipe";

  printf("\nPID master [%d]\n", getpid());
  fflush(stdout);

  while (1)
  {
    printf("\n\n%sInsert buffer size [MB] smaller than 100MB or insert [0] to quit:%s \n", bhblue, reset);

    do
    {
      scanf("%d", &bufSize);
      if (bufSize == 0)
      {
        unlink("my_timeprod");
        unlink("my_timecons");
        printf("program stopped by the user\n");
        fflush(stdout);
        return 0;
      }
      if (bufSize > 100 || bufSize < 0 || !bufSize)
      {
        printf("%sWrong value! try again:%s\n", bhred, reset);
        fflush(stdout);
      }
    } while (bufSize > 100 || bufSize < 0 || !bufSize);
    
    bufSize=bufSize*250000;
    
    sprintf(bufSize_s, "%d", bufSize);

    printf("\nWhich data sharing mode do you want to use? Press: \n");
    printf("%s[1]: Un-named pipes%s\n%s[2]: Named pipes%s\n%s[3]: Sockets%s\n%s[4]: Shared memory with circular buffer%s\n", bhyellow, reset, bhgreen, reset, bhmagenta, reset, bhcyan, reset);

    do
    {
      scanf("%d", &input_mode);
      if (input_mode != 2 && input_mode != 1 && input_mode != 3 && input_mode != 4)
      {
        printf("%sWrong key! try again:%s\n", bhred, reset);
        fflush(stdout);
      }
    } while (input_mode != 2 && input_mode != 1 && input_mode != 3 && input_mode != 4);

    // UNNAMED PIPE MODE.

    if (input_mode == 1){

      printf("\n%s[1]: Un-named pipes%s\n", bhyellow, reset);
      printf("%s-----------------------%s\n", bhyellow, reset);
      printf("Sending data...\n"); fflush(stdout);

      char fd_1[80];
      char fd_2[80];


      if (pipe(fd_up) == -1)
      {
        perror("Error creating unnamed fifo\n");
        exit(1);
      }

      // Assigning the two variables for reading and writing 
      // the unnamed pipe.

      sprintf(fd_1, "%d", fd_up[1]);
      sprintf(fd_2, "%d", fd_up[0]);

      // char *arg_list_1[] = {"./up_prod", fd_1, bufSize_s, NULL};
      // char *arg_list_2[] = {"./up_cons", fd_2, bufSize_s, NULL};
      char *arg_list_up[] = {"./up", fd_1, fd_2, bufSize_s, NULL};

      // Spawning of the other processes.

      // pid_producer = spawn("./up_prod", arg_list_1);
      // pid_consumer = spawn("./up_cons", arg_list_2);
      pid_t pid_up = spawn("./up", arg_list_up);

      printf("PID UP [%d]\n", pid_up); fflush(stdout);
    }

    // NAMED PIPE MODE.

    if (input_mode == 2){

      printf("\n%s[2]: Named pipes:%s\n", bhgreen, reset);
      printf("\n%s-----------------------%s\n", bhgreen, reset);
      printf("Sending data...\n"); fflush(stdout);

      // Creating pipes.

      create_fifo(named_pipe);

      // Creating argument list.

      // char *arg_list_1[] = {"./np_prod", named_pipe, bufSize_s, NULL};
      // char *arg_list_2[] = {"./np_cons", named_pipe, bufSize_s, NULL};
      char *arg_list_np[] = {"./np", named_pipe, bufSize_s, NULL};

      // Spawning of the other processes.

      // pid_producer = spawn("./np_prod", arg_list_1);
      // pid_consumer = spawn("./np_cons", arg_list_2);
      pid_t pid_np = spawn("./np", arg_list_np);

      printf("PID Named Pipe [%d]\n", pid_np); fflush(stdout);
    }

    // SOCKET MODE.

    if (input_mode == 3){

      printf("\n%s[3]: Sockets:%s\n", bhmagenta, reset);
      printf("\n%s-----------------------%s\n", bhmagenta, reset);
      printf("Sending data...\n"); fflush(stdout);

      // Initialization of the array of data needed for using the execvp() function
      // contained in the spawn() function.

      // char *arg_list_1[] = {"./s_prod", "5096", bufSize_s, (char *)NULL};
      // char *arg_list_2[] = {"./s_cons", "127.0.0.1", "5096", bufSize_s, (char *)NULL};
      char *arg_list_socket[] = {"./socket", "5096", "127.0.0.1", bufSize_s, (char *)NULL};

      // Spawning processes.

      // pid_producer = spawn("./s_prod", arg_list_1);
      // pid_consumer = spawn("./s_cons", arg_list_2);
      pid_t pid_socket = spawn("./socket", arg_list_socket);

      // printf("PID producer [%d]\n", pid_producer);
      // printf("PID consumer [%d]\n", pid_consumer); 
      printf("PID socket [%d]\n", pid_socket); fflush(stdout);
    }

    if (input_mode == 4)
    {
      int circular_buffer_size = 0;
      char circular_buffer_size_s[80];
      printf("\nHow big should the circular-buffer memory be [KB]? (0.1-10): \n");
      do
      {
        scanf("%d", &circular_buffer_size);
        if (circular_buffer_size <= 0 || circular_buffer_size > 10)
        {
          printf("%sWrong mem! try again:%s\n", bhred, reset);
          fflush(stdout);
        }
      } while (circular_buffer_size <= 0 || circular_buffer_size > 10);
      
      
      bufSize=bufSize*250;

      sprintf(circular_buffer_size_s, "%d", circular_buffer_size);

      printf("\n%s[4]: Shared memory with circular buffer:%s\n", bhcyan, reset);
      printf("\n%s-----------------------%s\n", bhcyan, reset);
      printf("Sending data...\n");
      fflush(stdout);
      char *arg_list_1[] = {"./cb", bufSize_s, circular_buffer_size_s, (char *)NULL};
      pid_cb = spawn("./cb", arg_list_1);
      printf("PID Circular-buffer [%d]\n", pid_cb);
      fflush(stdout);
    }

    CHECK(fd_fifoprod = open("my_timeprod", O_RDONLY));
    CHECK(fd_fifocons = open("my_timecons", O_RDONLY));

    sleep(1);

    int status;
    CHECK(wait(&status));

    if (WIFEXITED(status))
    {
      CHECK(WEXITSTATUS(status));
    }

    read(fd_fifoprod, &timeprod, sizeof(double));
    read(fd_fifocons, &timecons, sizeof(double));

    double timediff = timecons - timeprod;

    timeprod = timeprod / 1000000000;
    timecons = timecons / 1000000000;
    printf("TIME 1 IS: %lf\n", timeprod);
    printf("TIME 2 IS: %lf\n", timecons); fflush(stdout);

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
    sleep(1);
  }

  unlink("my_timeprod");
  unlink("my_timecons");
  return 0;
}
