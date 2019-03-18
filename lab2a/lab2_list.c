//Mehar Nallamalli
//804769644
//mnallamalli97@ucla.edu

#include "SortedList.c"
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>


int opt_yield;
int spin_l;
int delete_val = 0;
long long no_threads = 1;
long long no_iterations = 1;
char sync_opt;
int sync_f = 0;
pthread_mutex_t mutex;
long total_elements;

//need an empty list
SortedList_t *head;
//array of elements
SortedListElement_t **elements_opt;
char **ptr_keys;



void read_num_parameter(long long *no, const char *arg)
{
	if (arg != NULL)
	{
		*no = atoll(arg);
	}else{
		*no = 0;
	}
	
}



void yield_flag_enabled(char* str){

	if (strchr(str, 'i') != NULL)
	{
		opt_yield = opt_yield | INSERT_YIELD;
	}

	if (strchr(str, 'd') != NULL)
	{
		opt_yield = opt_yield | DELETE_YIELD;
	}

	if (strchr(str, 'l') != NULL)
	{
		opt_yield = opt_yield | LOOKUP_YIELD;
	}

}

void print_usage(const char *exe)
{
	printf("Usage: %s [--threads=N] [--iterations=M]\n", exe);
}


void* create_thread(void* curr){
	//SortedListElement_t* tmp = curr;
	//SortedListElement_t* store = curr;
	//SortedListElement_t *ptr;
	//char *current_key = malloc(sizeof(char)*256);


	if (sync_opt == 'm')
	{
		pthread_mutex_lock(&mutex);

	}else if(sync_opt == 's'){
		while(__sync_lock_test_and_set(&spin_l, 1));
	}else{
		int trd = *(int*) curr;
		long i;

		for (i= trd; i < total_elements; i += no_threads)
		{
			SortedList_insert(head, elements_opt[i]);
		}

		SortedListElement_t* current_element;

		for (i= trd; i < total_elements; i+=no_threads)
		{
			current_element = SortedList_lookup(head, elements_opt[i]->key);

			if (current_element == NULL)
			{
				fprintf(stderr, "cannot look up element\n" );
				exit(2);
			}

			delete_val = SortedList_delete(current_element);
			if (delete_val != 0)
			{
				fprintf(stderr, "cannot delete element\n" );
				exit(2);
			}
		}

	}

		if (sync_opt == 'm')
		{
			pthread_mutex_unlock(&mutex);
		}

		if (sync_opt == 's')
		{
			__sync_lock_release(&spin_l);
		}

	pthread_exit(NULL);
}

void handler(int signum){
	if (signum == SIGSEGV)
	{
		fprintf(stdout, "Segfault caught.");
		exit(2);
	}

}


int main(int argc, char **argv){

	/* variables that will be tracking the options both
	 * locally and from the getopt library.
	 * */

	signal(SIGSEGV, handler);

	int opt;
	
	long long no_threads = 1;
	long long no_iterations = 1;
	long i = 0;
	opt_yield = 0;
	sync_opt = 0;
	struct timespec begin;
	struct timespec end;

	static struct option long_options[] =
	{
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", required_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{0, 0, 0, 0}		// handles invalid args
	};


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
			for (i = 0; i < strlen(optarg); ++i)
			{
				yield_flag_enabled(optarg);
			}		
			break;
		case 's':
				sync_opt = optarg[0];
				// switch(sync_opt){
				// 	case 'm':
				// 		pthread_mutex_init(&mutex, NULL);
				// 		break;
				// 	case 's':
				// 		sync_f = 1;
				// 		break;
				// }	
			break;
		default:
			print_usage(optarg);
			exit(1);
		}
	}

	
	total_elements = no_iterations * no_threads;

	elements_opt = malloc(total_elements * sizeof(SortedListElement_t*));
	ptr_keys = malloc(total_elements*sizeof(char*));


	if (ptr_keys == NULL)
	{
		fprintf(stderr, "No memory for keys\n");
		exit(1);
	}

	//TODO: generate the random keys
	for (i = 0; i < total_elements; ++i)
	{
		elements_opt[i] = malloc(sizeof(SortedListElement_t));
		elements_opt[i]->prev = NULL;
		elements_opt[i]->next = NULL;
		elements_opt[i]->key = 'a';
	}


	pthread_t *thread_id;
	//dynamically setup threads
	thread_id = malloc(no_threads * sizeof(pthread_t));
	if (thread_id == NULL)
	{
		fprintf(stderr, "Don't have access to memory.\n" );
		exit(1);
	}

	clock_gettime(CLOCK_MONOTONIC, &begin);


	//fprintf(stderr, "%s\n", "Creating threads");
	//create threads
	for(i = 0; i < no_threads; i++){

		int ans = pthread_create(&thread_id[i], NULL, create_thread, (void*) elements_opt + no_iterations *i );
		if (ans != 0)
		{
			fprintf(stderr, "Error with creating thread\n");
			exit(1);
		}
	}

	//wait for threads to finish
	//fprintf(stderr, "%s\n", "Reaping threads");
	for(i = 0; i < no_threads; i++){
		int ans = pthread_join(thread_id[i], NULL);
		if (ans != 0)
		{
			fprintf(stderr, "Error with joining thread\n");
			exit(1);
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &end);



	//to double check, make sure sorted list is empty
	int ret = SortedList_length(head);
	//fprintf(stderr, "Length of list is: %d \n", ret);
	if (ret != 0)
	{
		fprintf(stderr, "The list is not empty.\n");
		exit(2);
	}


	//fprintf(stderr, "value of sync_opt: %d \n", sync_opt);


	//NEED TO PRINT THE RESULTS TO .CSV FILE
	fprintf(stdout, "list-");
	
	//switch condition for opt_yield. should be 8 cases + default
	//none, i,d,l,id,il,dl,idl

	switch(opt_yield){
		case 0:
			fprintf(stdout, "none");
			break;
		case 1:
			fprintf(stdout, "i");
			break;
		case 2:
			fprintf(stdout, "d");
			break;
		case 3: 
			fprintf(stdout, "id");
			break;
		case 4:
			fprintf(stdout, "l");
			break;
		case 5:
			fprintf(stdout, "il");
			break;
		case 6:
			fprintf(stdout, "dl");
			break;
		case 7:
			fprintf(stdout, "idl");
			break;
		default:
			break;
	}


	switch(sync_opt){
		case 0:
			fprintf(stdout, "-none");
			break;
		case 's':
			fprintf(stdout, "-s");
			break;
		case 'm':
			fprintf(stdout, "-m");
			break;
		default:
			break;
	}


	//print the timings

	long totaloperations = no_iterations * no_threads * 3;
	long totaltime = ((end.tv_sec*1000000000) + (end.tv_nsec)) - ((begin.tv_sec*1000000000) + (begin.tv_nsec));
	long averagetime = totaltime/totaloperations;

	fprintf(stdout, ",%d,%d,1,%ld,%ld,%ld \n", no_threads, no_iterations, totaloperations, totaltime, averagetime);
    


//if everything went well
exit(0);
}