
/* ****************************************************************************
 * simpsh.c
 * 28 January 2019
 * Operating Systems
 * */

#define _GNU_SOURCE

#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

//****************** *** File flags: *** ****************** 
int append_f = 0;
int cloexec_f = 0;
int creat_f = 0;
int directory_f = 0;
int dsync_f = 0;
int excl_f = 0;
int nofollow_f = 0;
int nonblock_f = 0;
int rsync_f = 0;
int sync_f = 0;
int trunc_f = 0;

//****************** *** Misc options: : *** ******************  
int verbose = 0;
int command_arg = 0;
int wait_f = 0;
int fail_f = 0;

// for the shell's wait
int shell_wstatus;

//interrupt handling: 
int catch_sig = 0;
int ignore_sig = 0;
int default_sig = 0;

// int* ptr = NULL;
// int caught = *ptr;

/* ****************************************************************************
 * enumerations and structures for the program
 * */

/* these maximums were chosen based on the maximum command line
 * length that the shell will tolerate
 * */
/* maximum number of arguments for any subcommand
 * */
#define argument_list_length 50

/* maximum for the number of files that are in use
 * */
#define file_list_length 50

/* maximum for the number of commands that are in use
 * */
#define command_list_length 50

enum file_mode{
  _read, _write, _readwrite, _piperead, _pipewrite
};
typedef enum file_mode file_mode;

struct my_file
{
  int fd;
  file_mode mode;
  int valid;
  char* name;
};
typedef struct my_file my_file;

enum command_wait {
  _not_waiting = 0, _done_waiting
};
typedef enum command_wait command_wait;

struct my_command
{
  char* name;
  char* argument_list[argument_list_length];
  int signal_catch;
  int signal_ignore;
  int signal_default;
  pid_t child_pid;
  int wstatus;
  int options;
  command_wait wait_f;
};
typedef struct my_command my_command;

/* this signal handler does some i/o to stderr
 * and then exits with the signal number
 * */
void signal_handler(int sig)
{
    if (sig == catch_sig)
    {
	fprintf(stderr, "%d caught\n", catch_sig);
	exit(sig);
    }
}

/* our favorite default signal handler 
 * exits with code 139 and produces no
 * output to either stdout nor stderr...
 * */
void default_signal_handler(int sig)
{
    /* there is a use for it...
     * */
    sig = sig + 1;

    exit(139);
}

/* update the signal handler
 * */
void update_signal_handler(int sig, void (*new_handler) (int))
{
    struct sigaction sig_action;

    sig_action.sa_handler = new_handler;

    if (0 != sigaction(sig, &sig_action, NULL))
    {
	// we have a problem registering the handler
	if (EFAULT == errno)
	{
	    fprintf(stderr, "The specified handler is not valid.\n");
	}
	else			// EINVAL == errno
	{
	    if (SIGKILL == sig)
	    {
		fprintf(stderr,
			"The handler for SIGKILL may not be overridden.\n");
	    }
	    else if (SIGSTOP == sig)
	    {
		fprintf(stderr,
			"The handler for SIGSTOP may not be overridden.\n");
	    }
	    else		// invalid signal number
	    {
		fprintf(stderr, "The signal number %d is not valid.\n", sig);
	    }
	}
    }
}

/* ****************************************************************************
 * the main function...
 * */
int main(int argc, char **argv)
{

    // the long opts set up for parsing the command line
    static struct option long_options[] = {
	{"rdonly", 1, NULL, 'r'},
	{"wronly", 1, NULL, 'w'},
	{"command", 1, NULL, 'c'},
	{"rdwr", 1, NULL, 't'},
	{"verbose", 0, NULL, 'v'},

	{"append", 0, NULL, 'a'},
	{"cloexec", 0, NULL, 'b'},
	{"creat", 0, NULL, 'd'},
	{"directory", 0, NULL, 'e'},
	{"dsync", 0, NULL, 'f'},
	{"excl", 0, NULL, 'g'},
	{"nofollow", 0, NULL, 'h'},
	{"nonblock", 0, NULL, 'i'},
	{"rsync", 0, NULL, 'j'},
	{"sync", 0, NULL, 'k'},
	{"trunc", 0, NULL, 'l'},

	{"abort", 0, NULL, 'm'},
	{"pipe", 0, NULL, 'n'},
	{"pause", 0, NULL, 'o'},
	{"catch", 1, NULL, 'p'},
	{"close", 1, NULL, 'q'},
	{"wait", 0, NULL, 's'},
	{"ignore", 1, NULL, 'u'},
	{"default", 1, NULL, 'z'},

	{0, 0, 0, 0}		// handles invalid args
    };

    /* used by getopt_long to store the return code for the
     * command line flag currently being processed
     * */
    int opt;

    /* used in the command flag switch branches
     * */
    char **command;
    int file_arr[3];

    /* used by the file/pipe creation flag switch branches
     * */
    int newfile; // result from open calls
    int newpipe; // result from open calls
    int pipe_file_descriptors[2]; // read/write descriptors

    /* store string N from command line as an integer
     * */
    int mandatory_n = 0;

    /* handy loop counters i and j are
     * */
    int i;
    int j;

    /* index file/command currently being constructed
     * */
    int fd_ind = 0;
    int cmd_ind = 0;

    /* allocate and initialize the bookkeeping for the files and commands
     * */
    my_file *files = (my_file *) malloc
	    (file_list_length * sizeof(my_file));
    my_command *command_list = (my_command *) malloc
	    (command_list_length * sizeof(my_command));

    if (command_list == NULL || files == NULL)
    {
	fprintf(stderr, "Error in allocation of memory.\n");
	exit(1);
    }
    memset(files, 0, file_list_length * sizeof(my_file));
    memset(command_list, 0, command_list_length * sizeof(my_command));



/* ****************************************************************************
 * the option processing loop
 * */
    while ((opt = getopt_long(argc, argv, "", long_options, 0)) != -1)
    {
	/* the wait flag was previously specified...
	 * */
	if (wait_f == 1)
        {
		/* was there a previous command, if so
		 * just stop and wait
		 * */
		if(0 <= cmd_ind - 1)
		{
			waitpid(command_list[cmd_ind - 1].child_pid,
				&command_list[cmd_ind - 1].wstatus, 0);

			/* should we test to see if it was signaled
			 * or exited...status will have that information
			 * */
			fprintf(stdout, "exit %d %s ",
				command_list[cmd_ind - 1].wstatus,
				command_list[cmd_ind - 1].name);

			i = 0;

			while (command_list[cmd_ind - 1].argument_list[i] 
				!= NULL)
			{
			    fprintf(stdout, "%s ",
				    command_list[cmd_ind - 1].argument_list[i]);
			    i++;
			}
			fprintf(stdout, "\n");

			/* reset the wait flag because it wasn't for the shell
			 * */
			wait_f = 0;

			/* the shell has finished waiting for this process
			 * */
			command_list[cmd_ind - 1].wait_f = _done_waiting;
		}
		/* should the shell leave the flag set so that it waits
		 * after processing or reset it because there are more 
		 * flags to read?
		else
		{
		}
		 * */
		wait_f = 0;
	}

	switch (opt)
	{
/* ****************************************************************************
 * --rdonly <filename>
 * a read only input stream has been specified
 * */
	case 'r':

	    if (verbose)
	    {
		printf("--rdonly %s \n", argv[optind - 1]);
		fflush(stdout);
	    }
	    newfile = open(argv[optind - 1],
			   O_RDONLY | append_f * O_APPEND |
			   cloexec_f * O_CLOEXEC | creat_f * O_CREAT |
			   directory_f * O_DIRECTORY | dsync_f * O_DSYNC |
			   excl_f * O_EXCL | nofollow_f *
			   O_NOFOLLOW | nonblock_f * O_NONBLOCK | rsync_f *
			   O_RSYNC | sync_f * O_SYNC | trunc_f * O_TRUNC,
			   O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	    files[fd_ind].fd = newfile;
	    files[fd_ind].mode = _read;
	    files[fd_ind].valid = 1;
	    files[fd_ind].name = argv[optind - 1];

	    if (newfile == -1)
	    {
		fprintf(stderr, "Cannot open and read the file.\n");
		files[fd_ind].valid = 0;
		fail_f = 1;
	    }

	    append_f = 0;
	    cloexec_f = 0;
	    creat_f = 0;
	    directory_f = 0;
	    dsync_f = 0;
	    excl_f = 0;
	    nofollow_f = 0;
	    nonblock_f = 0;
	    rsync_f = 0;
	    sync_f = 0;
	    trunc_f = 0;

	    fd_ind++;
	    break;
/* ****************************************************************************
 * --wronly <filename>
 * a write only output stream has been specified
 * */
	case 'w':
	    if (verbose)
	    {
		printf("--wronly %s \n", argv[optind - 1]);
		fflush(stdout);
	    }
	    newfile = open(argv[optind - 1],
			   O_WRONLY | append_f * O_APPEND |
			   cloexec_f * O_CLOEXEC | creat_f * O_CREAT |
			   directory_f * O_DIRECTORY | dsync_f * O_DSYNC |
			   excl_f * O_EXCL | nofollow_f *
			   O_NOFOLLOW | nonblock_f * O_NONBLOCK | rsync_f *
			   O_RSYNC | sync_f * O_SYNC | trunc_f * O_TRUNC,
			   O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	    files[fd_ind].fd = newfile;
	    files[fd_ind].mode = _write;
	    files[fd_ind].valid = 1;
	    files[fd_ind].name = argv[optind - 1];

	    if (newfile == -1)
	    {
		fprintf(stderr, "Cannot open and write the file.\n");
		files[fd_ind].valid = 0;
		fail_f = 1;
	    }

	    append_f = 0;
	    cloexec_f = 0;
	    creat_f = 0;
	    directory_f = 0;
	    dsync_f = 0;
	    excl_f = 0;
	    nofollow_f = 0;
	    nonblock_f = 0;
	    rsync_f = 0;
	    sync_f = 0;
	    trunc_f = 0;

	    fd_ind++;
	    break;

/* ****************************************************************************
 * --rdwr <filename>
 * a read and write input/output has been specified
 * */
	case 't':

	    if (verbose)
	    {
		printf("--rdwr %s \n", argv[optind - 1]);
		fflush(stdout);
	    }

// the library implementation is bad as indicated by the 
// error no return and the creation flag
// discovered running case 20
//	    newfile = open(argv[optind - 1],
//			   O_RDWR | append_f * O_APPEND |
//			   cloexec_f * O_CLOEXEC | creat_f * O_CREAT |
//			   directory_f * O_DIRECTORY | dsync_f * O_DSYNC |
//			   excl_f * O_EXCL | nofollow_f *
//			   O_NOFOLLOW | nonblock_f * O_NONBLOCK | rsync_f *
//			   O_RSYNC | sync_f * O_SYNC | trunc_f * O_TRUNC,
//			   O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	    newfile = open(argv[optind - 1],
			   O_RDWR | append_f * O_APPEND
			   | cloexec_f * O_CLOEXEC | creat_f * O_CREAT
			   | directory_f * O_DIRECTORY | dsync_f * O_DSYNC
			   | nofollow_f * O_NOFOLLOW | nonblock_f * O_NONBLOCK 
			   | rsync_f * O_RSYNC | sync_f * O_SYNC 
			   | trunc_f * O_TRUNC,
			   O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	    files[fd_ind].fd = newfile;
	    files[fd_ind].mode = _readwrite;
	    files[fd_ind].valid = 1;
	    files[fd_ind].name = argv[optind - 1];

	    if (newfile == -1)
	    {
		fprintf(stderr, 
			"Cannot read and write the file named: %s ", 
			files[fd_ind].name);
		fprintf(stderr, "errno = %d.\n",
			errno);
		files[fd_ind].valid = 0;
		fail_f = 1;
	    }

	    append_f = 0;
	    cloexec_f = 0;
	    creat_f = 0;
	    directory_f = 0;
	    dsync_f = 0;
	    excl_f = 0;
	    nofollow_f = 0;
	    nonblock_f = 0;
	    rsync_f = 0;
	    sync_f = 0;
	    trunc_f = 0;

	    fd_ind++;
	    break;
/* ****************************************************************************
 * --append
 * */
	case 'a':

	    if (verbose)
	    {
		printf("--append\n");
		fflush(stdout);
	    }
	    append_f = 1;
	    break;
/* ****************************************************************************
 * --cloexec
 * */
	case 'b':

	    if (verbose)
	    {
		printf("--cloexec\n");
		fflush(stdout);
	    }
	    cloexec_f = 1;
	    break;
/* ****************************************************************************
 * --creat
 * */
	case 'd':

	    if (verbose)
	    {
		printf("--creat\n");
		fflush(stdout);
	    }
	    creat_f = 1;
	    break;
/* ****************************************************************************
 * --directory
 * */
	case 'e':

	    if (verbose)
	    {
		printf("--directory\n");
		fflush(stdout);
	    }
	    directory_f = 1;
	    break;
/* ****************************************************************************
 * --dsync
 * */
	case 'f':

	    if (verbose)
	    {
		printf("--dsync\n");
		fflush(stdout);
	    }
	    dsync_f = 1;
	    break;
/* ****************************************************************************
 * --execl
 * */
	case 'g':

	    if (verbose)
	    {
		printf("--execl\n");
		fflush(stdout);
	    }
	    excl_f = 1;
	    break;
/* ****************************************************************************
 * --nofollow
 * */
	case 'h':

	    if (verbose)
	    {
		printf("--nofollow\n");
		fflush(stdout);
	    }
	    nofollow_f = 1;
	    break;
/* ****************************************************************************
 * --nonblock
 * */
	case 'i':

	    if (verbose)
	    {
		printf("--nonblock\n");
		fflush(stdout);
	    }
	    nonblock_f = 1;
	    break;
/* ****************************************************************************
 * --rsync
 * */
	case 'j':

	    if (verbose)
	    {
		printf("--rsync\n");
		fflush(stdout);
	    }
	    rsync_f = 1;
	    break;
/* ****************************************************************************
 * --sync
 * */
	case 'k':

	    if (verbose)
	    {
		printf("--sync\n");
		fflush(stdout);
	    }
	    sync_f = 1;
	    break;
/* ****************************************************************************
 * --trunc
 * */
	case 'l':

	    if (verbose)
	    {
		printf("--trunc\n");
		fflush(stdout);
	    }
	    trunc_f = 1;
	    break;
/* ****************************************************************************
 * --command N0 N1 N2 <cmd> [arg1 [arg2 [...]]]
 * */
	case 'c':		//command
	    /* collect stdin, stdout and stderr
	     * */
	    file_arr[0] = atoi(argv[optind - 1]);	//input
	    file_arr[1] = atoi(argv[optind]);	//output
	    file_arr[2] = atoi(argv[optind + 1]);	//error

	    /* collect the command from the command line 
	     * arguments
	     * */
	    command_list[cmd_ind].name = argv[optind + 2];

	    /* verify that it isn't an invalid command
	     * e.g. one that begins with "--"
	     *
	     * if it is report to standard error (the shell's)
	     * and then break from the switch statement
	     * before allocating any memory
	     * */
	    if (command_list[cmd_ind].name[0] == '-'
		&& command_list[cmd_ind].name[1] == '-')
	    {
		fprintf(stderr, "Invalid command format. \n");
		fail_f = 1;
		break;
	    }

	    /* make space for the command's arguments,
	     * if any
	     * */
	    command = (char **)malloc
		    (sizeof(char *) * argument_list_length);

	    if (command == NULL)
	    {
		fprintf(stderr, "Error in allocation of memory.\n");
		exit(1);
	    }
	    memset(command, 0, sizeof(char*) * argument_list_length);

	    /* make space for the executable name
	     * and store a copy for the subcommand if possible
	     * */
	    command[0] = (char*)malloc
	    (sizeof(char) * (strlen(command_list[cmd_ind].name) + 1));

	    if (command[0] == NULL)
	    {
		fprintf(stderr, "Error in allocation of memory.\n");
		exit(1);
	    }
	    memset(command[0], 0, 
		sizeof(char) * (strlen(command_list[cmd_ind].name) + 1));
	    strcpy(command[0], command_list[cmd_ind].name);

	    /* for each of the subcommand's parameters, allocate a
	     * space and store a copy for the subcommand
	     * */
	    i = optind + 3;

	    while ( argv[i] != NULL 
		&& ( ! (argv[i][0] == '-' && argv[i][1] == '-')) )
	    {
	    	/* make space for the executable name
		 * and store a copy for the subcommand if possible
		 * */
		command[i - (optind + 3) + 1] = (char*)malloc
			(sizeof(char) * (strlen(argv[i]) + 1));

		if (command[i - (optind + 3) + 1] == NULL)
		{
			fprintf(stderr,
				"Error in allocation of memory.\n");
			exit(1);
		}
		memset(command[i - (optind + 3) + 1], 0, 
			sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(command[i - (optind + 3) + 1], argv[i]);

		/* store a pointer to the shell's argv[i]
		 * for the shell's command list
		 * */
		command_list[cmd_ind].argument_list[i - (optind + 3)] =
			argv[i];
		i++;
	    }
	    command[i - (optind + 3) + 1] = NULL;
	    command_list[cmd_ind].argument_list[i - (optind + 3)] = NULL;

	    /* if either the input, output, or error is not valid, 
	     * or the value is greater than the index, report error.
	     * */
	    i = 0;

	    while(i < 3)
	    {
	    	if (file_arr[i] >= fd_ind || file_arr[i] < 0)
		{
			fprintf(stderr, "Invalid file no.\n");
			fail_f = 1;
		}
		i++;
	    }

	    /* now that the subcommand line processing is finished
	     * report the composition of the command flag if
	     * the verbose flag is set
	     * */
	    if (verbose)
	    {
		printf("--command %d %d %d", 
				file_arr[0], 
				file_arr[1],
				file_arr[2]);
		i = 0;

		while (command[i] != NULL)
		{
		    printf(" %s", command[i]);
		    i++;
		}
		printf("\n");
	    }

	    /* create a new process and attempt to execute the
	     * subcommand
	     * */
	    command_list[cmd_ind].child_pid = fork();

	    /* fork returns a positive number to the parent
	     * process on success...
	     *
	     * free the command array to avoid memory leaks
	     * */
	    if (command_list[cmd_ind].child_pid > 0)
	    {
		/*
		 * not clear about why the parent would
		 * test the file descriptors at this point
		 * */
		i = 0;

		while(i < 3)
		{
			if (files[file_arr[i]].valid == 0)
			{
			    fprintf(stderr, "File is not valid: %s. \n",
				    files[file_arr[i]].name);
			    fail_f = 1;
			}
			i++;
		}

		/* free the memory for the command array
		 * */
		i = 0;

		while(command[i] != NULL)
		{
			free(command[i]);
			i++;
		}
		free(command);
	    }
	    /* fork returns exactly zero to the child process
	     * which now has a copy of the parent's memory 
	     * space on success...
	     * */
	    else if (command_list[cmd_ind].child_pid == 0)
	    {
		    /* if we must declare it here...
		     * let us put it at the top of the 
		     * section.
		     * */
		    int exec;

		    /* setup the i/o redirection
		     * */
		    for (i = 0; i < 3; i++)
		    {
			dup2(files[file_arr[i]].fd, i);
		    }

		    //closing the original files, now only have new dup
		    for (i = 0; i < fd_ind; i++)
		    {
			close(files[i].fd);
		    }

		    // create the new process
		    exec = execvp(command[0], command);

		    // die if the new process fails
		    if (exec == -1)
		    {
			exit(1);
		    }
	    }
	    /* fork returns a negative number if there was an 
	     * error
	     * */
	    else
	    {
		fprintf(stderr, "Fork failed making child process.\n");
		fail_f = 1;
	    }
	    /* on to the next process....
	     * */
	    cmd_ind++;

	    /* move past the current options, so that 
	     * getopt doesn't process them
	     * */
	    while(NULL != argv[optind]
	    && ! (argv[optind][0] == '-' && argv[optind][1] == '-') )
	    {
		optind++;
	    }
	    break;

/* ****************************************************************************
 * --verbose
 * */
	case 'v':

	    if (verbose)
	    {
		printf("--verbose\n");
		fflush(stdout);
	    }
	    verbose = 1;
	    break;

/* ****************************************************************************
 * --abort
 * */
	case 'm':

	    if (verbose)
	    {
		printf("--abort \n");
		fflush(stdout);
	    }
	    raise(SIGSEGV);

	    break;

/* ****************************************************************************
 * --pipe
 * */
	case 'n':

	    if (verbose)
	    {
		printf("--pipe\n");
		fflush(stdout);
	    }

	    //0 is read; 1 is write
	    newpipe = pipe(pipe_file_descriptors);

	    if (newpipe < 0)
	    {
		fprintf(stderr, "Cannot create a pipe for read and write.\n");
		fail_f = 1;
	    }

	    // insert read and increment ptr
	    files[fd_ind].fd = pipe_file_descriptors[0];
	    files[fd_ind].mode = _piperead;
	    files[fd_ind].valid = 1;
	    fd_ind++;

	    // insert write and increment ptr
	    files[fd_ind].fd = pipe_file_descriptors[1];
	    files[fd_ind].mode = _pipewrite;
	    files[fd_ind].valid = 1;
	    fd_ind++;
	    break;

/* ****************************************************************************
 * --pause
 * */
	case 'o':

	    if (verbose)
	    {
		printf("--pause\n");
		fflush(stdout);
	    }
	    // pause execution until further notice
	    pause();

	    break;

/* ****************************************************************************
 * --catch N
 * */
	case 'p':

	    // read the mandatory argument as an integer...
	    catch_sig = atoi(argv[optind - 1]);

	    if (verbose)
	    {
		printf("--catch %d\n", catch_sig);
		fflush(stdout);
	    }
	    // record the signal handler that was reset for this command
	    command_list[cmd_ind].signal_catch = catch_sig;

	    // update the handler
	    update_signal_handler(catch_sig, signal_handler);
	    break;

/* ****************************************************************************
 * --close N
 * */
	case 'q':

	    // read the mandatory argument as an integer...
	    mandatory_n = atoi(argv[optind - 1]);

	    if (verbose)
	    {
		printf("--close %d\n", mandatory_n);
		fflush(stdout);
	    }
	    // close the file and update it's valid flag
	    close(files[mandatory_n].fd);
	    files[mandatory_n].valid = 0;
	    break;

/* ****************************************************************************
 * --wait
 * */
	case 's':

	    if (verbose)
	    {
		printf("--wait\n");
		fflush(stdout);
	    }
	    wait_f = 1;
	    break;

/* ****************************************************************************
 * register the ignore handler for the mandatory parameter N
 * --ignore N
 * */
	case 'u':
	    // read the mandatory argument as an integer...
	    ignore_sig = atoi(argv[optind - 1]);

	    if (verbose)
	    {
		printf("--ignore %d\n", ignore_sig);
		fflush(stdout);
	    }
	    // record the signal handler that was reset for this command
	    command_list[cmd_ind].signal_ignore = ignore_sig;

	    // update the handler
	    update_signal_handler(ignore_sig, SIG_IGN);
	    break;

/* ****************************************************************************
 * restore our default signal handling for the mandatory parameter N
 * --default N
 * */
	case 'z':
	    // read the mandatory argument as an integer...
	    default_sig = atoi(argv[optind - 1]);

	    if (verbose)
	    {
		printf("--default %d\n", default_sig);
		fflush(stdout);
	    }
	    // record the signal handler that was reset for the command
	    command_list[cmd_ind].signal_default = default_sig;

	    // update the handler
	    update_signal_handler(default_sig, default_signal_handler);
	    break;

/* ****************************************************************************
 * handle the unknown/invalid arguments
 * */
	default:
	    // inform the user of the problem and terminate
	    fprintf(stderr, "Not recognized argument.");
	    exit(1);
	    break;
	}
    }

/* ****************************************************************************
 * check on the child processes if any
 * */
    for (i = 0; i < cmd_ind; ++i)
    {

/* ****************************************************************************
 * output for the catch flag
 * */
	if (command_list[i].signal_catch != 0)
	{
	    waitpid(command_list[i].child_pid, &command_list[i].wstatus, WNOHANG);

	    if (WIFSIGNALED(command_list[i].wstatus))
	    {
		if (command_list[i].signal_catch ==
		    WTERMSIG(command_list[i].wstatus))
		{
		    fprintf(stderr, "%d caught\n",
			    command_list[i].signal_catch);
		    exit(command_list[i].signal_catch);
		}
	    }
	}

/* ****************************************************************************
 * was there a stray wait flag...
 * */
	if(wait_f == 1 && command_list[i].wait_f != _done_waiting)
	{
		waitpid(command_list[i].child_pid,
			&command_list[i].wstatus, 0);

			/* should we test to see if it was signaled
			 * or exited...status will have that information
			 * */
		fprintf(stdout, "exit %d %s ",
				command_list[i].wstatus,
				command_list[i].name);

		j = 0;

		while (command_list[i].argument_list[j] 
			!= NULL)
		{
		    fprintf(stdout, "%s ",
			    command_list[i].argument_list[j]);
		    j++;
		}
		fprintf(stdout, "\n");
	}
    }

/* ****************************************************************************
 * housekeeping
 * */
    free(files);
    free(command_list);

/* ****************************************************************************
 * was there a failure/signal that the creating process should learn about, 
 * if so how big is it
 * */
    if (fail_f > 0)
    {
	exit(1);
    }

    exit(0);
}

