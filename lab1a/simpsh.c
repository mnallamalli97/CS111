#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv){
    static struct option long_options[] =
    {
        {"rdonly", 1, NULL, 'r'},
        {"wronly", 1, NULL, 'w'},
        {"command", 1, NULL, 'c'},
        {"verbose", 0, NULL, 'v'},
        //handles case where none of args are inputted:
        {0,0,0,0}
    };
    
    int opt;
    pid_t child_pid;
    int verbose = 0;
    int newfile;
    int file_arr[3];
    int command_arg = 0;
    int fd_ind = 0;
    int *files = (int*) malloc(500 * sizeof(int*));
    int first;



    
    while((opt = getopt_long(argc, argv, "", long_options, 0 )) != -1){
        
        switch(opt){
            case 'r':
                if (verbose)
                {
                    printf("--rdonly %s \n", argv[optind-1]);
                }
                // newfile = open(optarg, O_RDONLY);
                newfile = open(argv[optind-1], O_RDONLY);
                files[fd_ind] = newfile;

                if (newfile == -1)
                {
                    fprintf(stderr, "Cannot open and read the file.\n");
                    exit(1);
                }
                fd_ind++;
                break;
            case 'w':
                if (verbose)
                {
                    printf("--wronly %s \n", argv[optind-1]);
                }
                // newfile = open(optarg, O_WRONLY);
                newfile = open(argv[optind-1], O_WRONLY);
                files[fd_ind] = newfile;

                if (newfile == -1)
                {
                    fprintf(stderr, "Cannot open and write the file.\n");
                    exit(1);
                }
                fd_ind++;
                break;
            case 'c':

                command_arg = 1;
                //int temp_first = optind + 2;                   
                first = optind;

                  file_arr[0] = atoi(argv[first-1]);    //input
                  file_arr[1] = atoi(argv[first]);    //output
                  file_arr[2] = atoi(argv[first+1]);    //error
                  //the actual file is at argv[first+3]
                  char *opt = argv[first + 2];

                  int cur;
                  cur = first;
                  int fin;
                  int exec;
                  char **command =  (char**) malloc(sizeof(char**) * 500);

                  if (command == NULL || files == NULL)
                  {
                    fprintf(stderr, "Error in allocation of memory.\n" );
                    exit(1);
                  }

                  command[0] = opt;


                while(cur < argc)
                {
                    if (argv[cur][0] == '-' && argv[cur][1] == '-')
                    {
                        //and the next char is also an '-'
                            // optind = temp_first;
                            fin = cur - 1;
                            break;
                        
                    }
                    cur++;
                    //logic helped by TA Alex during OH. I was not sure how to determine the final arg if multiple were given. 
                    (cur == argc) ? (fin = argc - 1) : (fin = cur -1) ; 
                }

                //if either the input, output, or error is not valid, or the value is greater than the index, report error.
                  if (file_arr[0] >= fd_ind || file_arr[1] >= fd_ind || file_arr[2] >= fd_ind || file_arr[0] < 0 || file_arr[1] < 0 || file_arr[2] < 0)
                  {
                      fprintf(stderr, "Invalid file. \n" );
                      exit(1);
                  }

                  int ct_args = 1;
                  while(ct_args <= cur - optind- 3){
                    command[ct_args] = argv[first + 2 + ct_args];
                    //either increment # of args, or if at last arg, then set command to nullptr for execvp
                    (fin == argc) ? (command[ct_args] = NULL) : ct_args++ ;
                  }
                  

                  if(verbose){
                      printf("--command %d %d %d", file_arr[0], file_arr[1], file_arr[2]);
                      int r = 0;
                      while(r < ct_args){
                        printf(" %s", command[r]);
                        r++;
                      }
                      printf("\n");

                  }



                  // if (temp_first > argc)
                  // {
                  //     fprintf(stderr, "Need operands.\n" );
                  //     exit(1);
                  // }

                 



          //        for (int i = 0; i < argc; i++) {
          //            printf("%s %d \n", argv[i], i);
          //        }
          //        printf("---------");
          //        
          //        printf("argv: %s \n", argv[temp_first]);
          //        printf("command: %s \n", command[0]);

                  child_pid = fork();

                  if (child_pid >= 0){
                     if(child_pid == 0){
                      //child process

                        int i;
                        //redirection
                        for (i = 0; i < 3; i++)
                        {
                            dup2(files[file_arr[i]],i);
                        }

                        int j = 0;
                        //closing the original files, now only have new dup
                        for (; j < fd_ind; j++)
                        {
                            close(files[j]);
                        }
                        
                        exec = execvp(command[0], command);
                        if (exec == -1)
                        {
                            fprintf(stderr, "Error in execvp. \n" );
                            exit(1);
                        }
                      }
                  } else{
                    //waitpid(child_pid, status, 0);
                    fprintf(stderr, "Fork has failed in making child process.\n");
                    exit(1);
                  }


                  // if (child_pid < 0)
                  // {
                  //     fprintf(stderr, "Fork has failed in making child process.\n");
                  // }

                 // else{
                 //    fprintf(stderr, "In the parent process...\n");
                 //    exit(1);
                 //  }
                  //free(command);


                  /* *** lines of code (209-216) suggested and helped by TA Alex on 1/17 OH. see readme for more details *** */
                  int optd = optind;
                  while(optd < argc){
                    if(argv[optind][0] == '-' && argv[optind][1] == '-'){
                        break;
                    }
                    optind++;
                    optd++;
                  }

                    break;

            case 'v':
                verbose = 1;
                break;
            default:
                fprintf(stderr, "Not recognized argument.");
                exit(1);
                break;
        }
    }
    
    free(files);
    exit(0);
}
