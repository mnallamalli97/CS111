//Mehar Nallamalli
//804769644
//mnallamalli97@ucla.edu


/*
 * file: lab2_add.c
 * date: 9 Feb 2019
 * This driver tests the performance of a variable number of 
 * threads over a variable number of iterations.
 * */
#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>


/* sets the ts structure to the current time at nanosecond
 * precision and tests for errors
 * */
void set_timer(struct timespec *ts)
{
	// just initialize the timer ts for the current time
	clock_gettime(CLOCK_MONOTONIC, ts);
}


/*
 * print the usage message
 * */
void print_usage(const char *exe)
{
	printf("Usage: %s [--threads=N] [--iterations=M]\n", exe);
}

/* call the appropriate function for converting a string
 * to a long long integer
 * */
void read_num_parameter(long long *no, const char *arg)
{
	if (arg != NULL)
	{
		*no = atoll(arg);
	}else{
		*no = 0;
	}
	
}

int opt_yield;
char sync_opt;
int spinl = 0;
pthread_mutex_t count_lock; 


/******************************************************************************
 * thread specific data/functionality
 * */

struct thread_data
{
	long long* ptr_count;
	long long* ptr_iterations;

};

/* adds a long long to the value at the address where pointer is
 * making reference
 * */
void add(long long *pointer, long long value)
{
	long long sum = *pointer + value;
    if (opt_yield){
    	sched_yield();
    }   
	*pointer = sum;
}



void addm(long long *pointer, long long value)
{
	pthread_mutex_lock(&count_lock);

	long long sum = *pointer + value;
    if (opt_yield){
    	sched_yield();
    }   
	*pointer = sum;

	pthread_mutex_unlock(&count_lock);
}

void adds(long long *pointer, long long value)
{
	while(__sync_lock_test_and_set(&spinl, 1));

	long long sum = *pointer + value;
    if (opt_yield){
    	sched_yield();
    }   
	*pointer = sum;

	__sync_lock_release(&spinl);
}

void addc(long long *pointer, long long value)
{
	long long t1, t2;
	do{
		t1 = *pointer;
		if (opt_yield){
	    	sched_yield();
	    }
	    t2 = t1 + value;   
	}while(__sync_val_compare_and_swap(pointer, t1, t2) != t1);
}


void* thread_add_count(void* td){

	struct thread_data* my_data = td;
	int i;

	//char* sync_opt = my_data->sync_opt;


	//critical code
	for (i = 0; i < *(my_data->ptr_iterations); ++i)
	{
		if (sync_opt == NULL)
		{
			add(my_data->ptr_count, 1);
		}else if(sync_opt == 'm'){
			addm(my_data->ptr_count, 1);
		}else if(sync_opt == 's'){
			adds(my_data->ptr_count, 1);
		}else if(sync_opt == 'c'){
			addc(my_data->ptr_count, 1);
		}
		
	}


	for (i = 0; i < *(my_data->ptr_iterations); ++i)
	{
		if (sync_opt == NULL)
		{
			add(my_data->ptr_count, -1);
		}else if(sync_opt == 'm'){
			addm(my_data->ptr_count, -1);
		}else if(sync_opt == 's'){
			adds(my_data->ptr_count, -1);
		}else if(sync_opt == 'c'){
			addc(my_data->ptr_count, -1);
		}
		
	}
	return td;

}

/******************************************************************************
 * main
 * */
int main(int argc, char **argv)
{
	/* variables that will be tracking the options both
	 * locally and from the getopt library.
	 * */
	int opt;

	static struct option long_options[] =
	{
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", no_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{0, 0, 0, 0}		// handles invalid args
	};
	extern char *optarg;
	extern int optind;
	extern int opterr;
	extern int optopt;
	int i;

	opt_yield = 0;
	sync_opt = 0;

	/* variables that will be tracking the settings for
	 * the current test run
	 * */
	long long no_threads = 1;
	long long no_iterations = 1;
	pthread_t *thread_id;

	/* variables that will be counting progress through the
	 * testing assignment
	 * */
     long long counter = 0;

	/* variables that will be used for the timer
	 * */
	struct timespec begin;
	struct timespec end;
	struct thread_data copy_thread_data;
	struct thread_data my_data;

	copy_thread_data.ptr_count = &counter;
	copy_thread_data.ptr_iterations = &no_iterations;

	while((opt = getopt_long(argc, argv, "", long_options, 0)) != -1)
	{

		/* what did the user specify for the optional arguments
		 * */
		switch(opt)
		{
		case 't':
			read_num_parameter(&no_threads, optarg);
			break;
		case 'i':
			read_num_parameter(&no_iterations, optarg);
			break;
		case 'y':
			opt_yield = 1;
			//yield_flag_enabled(optarg);
			break;
		case 's':
			sync_opt = optarg[0];
			break;
		default:
			print_usage(optarg);
			exit(1);
		}
	}

	/* validate the inputs
	 * */
	if(no_threads < 1 || no_iterations < 1)
	{
		print_usage(argv[0]);
		exit(1);
	}
	set_timer(&begin);

	/* run the tests on the threads
	 * */

	my_data.ptr_count = &counter;
	my_data.ptr_iterations = no_iterations;
	//my_data.sync_opt = sync_opt;

	thread_id = (pthread_t*) malloc(no_threads * sizeof(pthread_t));
	if (thread_id == NULL)
	{
		fprintf(stderr, "Don't have access to memory.\n" );
		exit(1);
	}

	// if (sync_opt == 'm')
	// {
	// 	if (pthread_mutex_init(&count_lock, NULL) != 0)
	// 	{
	// 		fprintf(stderr, "Can't create mutex\n");
	// 		exit(1);
	// 	}
	// }


	for (i = 0; i < no_threads; ++i)
	{
		int ans = pthread_create(&thread_id[i], NULL, thread_add_count, &copy_thread_data);
		if (ans != 0)
		{
			fprintf(stderr, "Error with creating thread\n");
			exit(1);
		}

	}

	for (i = 0; i < no_threads; ++i)
	{
		int ans = pthread_join(thread_id[i], NULL);
		if (ans != 0)
		{
			fprintf(stderr, "Error with joining the thread\n");
			exit(1);
		}

	}

	set_timer(&end);



	/* end the tests on the threads
	 * */

	/* report
	 * */

	long long ops = no_threads*no_iterations*2;
	long long totruntim = (end.tv_nsec + end.tv_sec*1000000000) - (begin.tv_nsec + begin.tv_sec*1000000000);
	long long timeperop = totruntim/ops;




	switch(opt_yield){
		case 0:
			if (sync_opt == 'm')
			{
				fprintf(stdout, "add-m,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}else if(sync_opt == 's'){
				fprintf(stdout, "add-s,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}else if(sync_opt == 'c'){
				fprintf(stdout, "add-c,%llu,%llu,%llu,%llu,%llu,%llu\nn", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}
			break;

		case 1:
			if (sync_opt == 'm')
			{
				fprintf(stdout, "add-yield-m,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}else if(sync_opt == 's'){
				fprintf(stdout, "add-yield-s,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}else if(sync_opt == 'c'){
				fprintf(stdout, "add-yield-c,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}else if(sync_opt == 0){
				fprintf(stdout, "add-yield-none,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
			}
			break;

		default:
			
			break;
	}

	if (opt_yield == 0 && sync_opt== 0)
	{
		fprintf(stdout, "add-none,%llu,%llu,%llu,%llu,%llu,%llu\n", no_threads, no_iterations, ops, totruntim, timeperop, counter);
	}
	

	//printf("%llu,%llu,%llu,%llu,%llu,%llu\n",no_threads, no_iterations, ops, totruntim, timeperop, counter);
	if (sync_opt == 'm')
	{
		pthread_mutex_destroy(&count_lock);
	}

	free(thread_id);
	return 0;
}

