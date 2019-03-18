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

void segFault_handler(int num){

  fprintf(stderr, "Segment Fault Handled with signal number %d\n", num);
  exit(4);
}


int main(int argc, char **argv){

    static struct option long_options[] =
    {
      	{"input", 1, NULL, 'i'},
      	{"output", 1, NULL, 'o'},
      	{"segfault", 0, NULL, 's'},
      	{"catch", 0, NULL, 'c'},
      	{"dump-core", 0, NULL, 'd'},
      	//handles case where none of args are inputted:
      	{0,0,0,0} 
    };


    int in = 0;
    int out = 1;
    char *input = NULL;
    char *output = NULL;
    int segfault = 0;

    char input_args[3];
    int pointer_ia = 0;

    int opt;



   
    while((opt = getopt_long(argc, argv, "i:o:sc", long_options, 0 )) != -1){
      switch(opt){
      case 'i':
	input = optarg;
	break;
      case 'o':
	       output = optarg;
	break;
      case 's':
      	segfault = 1;
      	input_args[pointer_ia] = 's';
      	pointer_ia++;
	break;
      case 'c':
    	input_args[pointer_ia] = 'c';
    	pointer_ia++;
	break;
      case 'd':
    	input_args[pointer_ia] = 'd';
    	pointer_ia++;
	break;
      default:
    	fprintf(stderr, "Not recognized argument. Should be in the form ./lab0 --input --output [--segfault] [--catch] [--dump-core] \n");
    	exit(1);
	   break;
      }
        
        
    }



    if (input != NULL)
      {
        in = open(input, O_RDONLY);

        if (in >= 0)
	  {
            close(0);
            dup(in);
            close(in);
	  }else{
	  fprintf(stderr, "Unable to open input file: %s\n", input );
	  exit(2);
        }
      }

    if (output != NULL)
      {
        out = creat(output, 0666);
        if (out >= 0)
	  {
            close(1);
            dup(out);
            close(out);
	  }else{
	  fprintf(stderr, "Unable to open output file: %s\n", output );
	  exit(3);
        }
      }

    /*
        dump_core catch segfault = catch
        catch segfault = catch

        dump_core segfault catch = segfault
        catch dump_core segfault = dump_core segfault = segfault
        segfault dump_core = segfault
        segfault catch = segfault
    */

    if (input_args[0] == 'd' && input_args[1] == 'c' && input_args[2] == 's')
      {
        signal(SIGSEGV,segFault_handler);
      }else if (input_args[0] == 'c' && input_args[1] == 's'){
      signal(SIGSEGV,segFault_handler);
    }

char *ptr = NULL;

    if (segfault != 0)
      {
          *ptr = 0;
           exit(139);
      }


    //at this point, all the checks for valid execution have passed. Now we can copy each character, one at a time. 
    ssize_t num_bytes_processed = sizeof(char);
    char* buffer = (char*) malloc(num_bytes_processed);

    while(read(0, buffer, num_bytes_processed) > 0){
      if (write(1, buffer, num_bytes_processed) < 0){
	fprintf(stderr, "Error, can't write to the output file...: %s\n", strerror(errno) );
	exit(2);
      }
    }

    exit(0);
    
}
