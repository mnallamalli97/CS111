//Mehar Nallamalli
//804769644
//mnallamalli97@ucla.edu



#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>

#include "SortedList.c"

int opt_yield = 0;
int* spin_l = NULL;
int delete_val = 0;
long long no_threads = 1;
long long no_iterations = 1;
long long no_lists = 1;
char sync_opt = 0;
int sync_f = 0;
pthread_mutex_t* mutex = NULL;
long total_elements = 0;
unsigned long long totalmutex = 0ULL;


//array of elements
/* this is the critical data
 * */
SortedListElement_t **elements_opt = NULL;
SortedListElement_t **lists_opt = NULL;
char **ptr_keys = NULL;
unsigned long long *mutex_times = NULL;

//dynamically setup threads
pthread_t *thread_id = NULL;
int sublist_length = 0;

//int i;

//taken from http://www.cse.yorku.ca/~oz/hash.html
int hash(const char *str)
{
	int hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

/* convert a number parameter to a long long
 * */
void read_num_parameter(long long *no, const char *arg)
{
	if (arg != NULL)
	{
		*no = atoll(arg);
	}
	else {
		*no = 1;
	}

}

/* calculate a time difference
* */
unsigned long long time_delta(struct timespec *initial, struct timespec *final)
{
	unsigned long long curr_time;

	curr_time = ((final->tv_sec * 1000000000) + (final->tv_nsec)) -
		((initial->tv_sec * 1000000000) + (initial->tv_nsec));

	return curr_time;
}

/* take mutex lock
*/
void take_mutex(int list, int thrd)
{
	/* for calculating the time difference
	* */
	struct timespec start;
	struct timespec end;

	/* gather the time to take the mutex
	* */
	clock_gettime(CLOCK_MONOTONIC, &start);
	pthread_mutex_lock(&mutex[list]);
	clock_gettime(CLOCK_MONOTONIC, &end);

	/* update time for the current thread
	* */
	mutex_times[thrd] += time_delta(&start, &end);
}

/* mutex unlock
*/
void unlock_mutex(int list)
{
	pthread_mutex_unlock(&mutex[list]);
}

/* take spinlock
*/
void take_spinlock(int list, int thrd)
{
	/* for calculating the time difference
	* */
	struct timespec begin;
	struct timespec end;

	clock_gettime(CLOCK_MONOTONIC, &begin);

	while (__sync_lock_test_and_set(&spin_l[list], 1));

	clock_gettime(CLOCK_MONOTONIC, &end);

	mutex_times[thrd] += time_delta(&begin, &end);
}

/* spinlock open
*/
void open_spinlock(int list)
{
	__sync_lock_release(&spin_l[list]);
}

/* counts the length of a sublist
 * that belongs to a thread
 * */
long long count_sublist_length(int list)
{
	/* tracks the length of the list currently
	* being counted
	* */
	long long length_of_current = 0LL;

	/* update the length of the list*/
	length_of_current = SortedList_length(lists_opt[list]);

	if (length_of_current < 0LL) {
		fprintf(stderr, "list is corrupted\n");
		exit(2);
	}

	return length_of_current;
}

void find_sublist_length(int trd) {

	long long length_of_lists = 0LL;
	long i;

	switch (sync_opt)
	{
										/* mutex style */
	case 'm':


		/* for every list */
		for (i = 0; i < no_lists; ++i)
		{
			take_mutex(i, trd);

			length_of_lists += count_sublist_length(i);

			unlock_mutex(i);
		}
		break;
										/* spin lock style */
	case 's':

		/* for every list*/
		for (i = 0; i < no_lists; ++i)
		{

			take_spinlock(i, trd);
			length_of_lists += count_sublist_length(i);
			open_spinlock(i);
		}
		break;
										/* no locks */
	default:
		for (i = 0; i < no_lists; ++i)
		{
			length_of_lists += count_sublist_length(i);
		}
	}

	if (length_of_lists < no_iterations)
	{
		fprintf(stderr, "not all items were inserted.\n");
		fprintf(stderr, "length_of_lists = %ld\n", length_of_lists);
		fprintf(stderr, "no_threads * no_iterations = %ld\n", 
				no_threads * no_iterations);
		exit(2);
	}
}

/* setup the yield options 
 * */
void yield_flag_enabled(char* str) {

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
	fflush(stdout);
}

void* create_thread(void* curr) {

	/* our thread id number which is used as an index
	 * */
	int trd = *((int*)curr);

	long i;
	struct timespec mutext_b;
	struct timespec mutext_e;
	long curr_time;
	int sub;

	switch(sync_opt)
	{
		/* use the mutex technique
		* */
	case 'm':
		/* from the thread trd
		* upto total_elements skipping the no_threads
		* each time
		* */
		for (i = trd; i < total_elements; i += no_threads)
		{
			/* get the hash for this thread's partition of
			* the list
			* */
			sub = hash(elements_opt[i]->key) % no_lists;

			/* find the correct partition and set
			* timers for accessing the list
			* */
			take_mutex(sub, trd);
			SortedList_insert(lists_opt[sub], elements_opt[i]);
			unlock_mutex(sub);
		}
		break;
	case 's':
		/* from the thread trd
		* upto total_elements skipping the no_threads
		* each time
		* */
		for (i = trd; i < total_elements; i += no_threads)
		{
			/* get the hash for this thread's partition of
			* the list
			* */
			sub = hash(elements_opt[i]->key) % no_lists;
			/* find the correct partition and set
			* timers for accessing the list
			* */
			take_spinlock(sub, trd);
			SortedList_insert(lists_opt[sub], elements_opt[i]);
			open_spinlock(sub);
		}
		break;
	default:
		for (i = trd; i < total_elements; i += no_threads)
		{
			/* get the hash for this thread's partition of
			* the list
			* */
			sub = hash(elements_opt[i]->key) % no_lists;
			SortedList_insert(lists_opt[sub], elements_opt[i]);
		}
		break;
	}


	//for(i = 0; i < no_lists; i++)
	//{
//	for (i = trd; i < total_elements; i += no_threads)
//	{
		/* get the hash for this thread's partition of
		 * the list
		 * */
//		sub = hash(elements_opt[i]->key) % no_lists;
		find_sublist_length(trd);
//	}


	SortedListElement_t* current_element;

	switch(sync_opt)
	{
	case 'm':
		for (i = trd; i < total_elements; i += no_threads)
		{
			sub = hash(elements_opt[i]->key) % no_lists;
			take_mutex(sub, trd);

			current_element = SortedList_lookup
							(lists_opt[sub], elements_opt[i]->key);

			if (current_element == NULL)
			{
				fprintf(stderr, "cannot look up element\n");
				exit(2);
			}

			if (SortedList_delete(current_element) != 0)
			{
				fprintf(stderr, "cannot delete element\n");
				exit(2);
			}
			unlock_mutex(sub);		
		}
		break;
	case 's':
		for (i = trd; i < total_elements; i += no_threads)
		{
			sub = hash(elements_opt[i]->key) % no_lists;
			take_spinlock(sub, trd);

			current_element = SortedList_lookup(lists_opt[sub], elements_opt[i]->key);

			if (current_element == NULL)
			{
				fprintf(stderr, "cannot look up element\n");
				exit(2);
			}

			if (SortedList_delete(current_element) != 0)
			{
				fprintf(stderr, "cannot delete element\n");
				exit(2);
			}
			open_spinlock(sub);
		}
		break;
	default:
		for (i = trd; i < total_elements; i += no_threads)
		{
			current_element = SortedList_lookup(lists_opt[sub], elements_opt[i]->key);

			if (current_element == NULL)
			{
				fprintf(stderr, "cannot look up element\n");
				exit(2);
			}

			if (SortedList_delete(current_element) != 0)
			{
				fprintf(stderr, "cannot delete element\n");
				exit(2);
			}
		}
		break;
	}

	pthread_exit(NULL);
}

void handler(int signum) {
	if (signum == SIGSEGV)
	{
		fprintf(stdout, "Segfault caught.");
		fflush(stdout);
		exit(2);
	}

}


char* key_gen() {
	char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int len = 25;
	int keylen = 5;
	int i;
	char* ke = malloc((keylen + 1) * sizeof(char));
	for (i = 0; i < keylen; i++) {
		ke[i] = alpha[rand() % len];
	}
	ke[keylen] = '\0';

	return ke;
}

int main(int argc, char **argv) {

	/* variables that will be tracking the options both
	* locally and from the getopt library.
	* */

	int opt;
	struct timespec begin;
	struct timespec end;
	int i;

	static struct option long_options[] =
	{
		{ "threads", required_argument, NULL, 't' },
		{ "iterations", required_argument, NULL, 'i' },
		{ "yield", required_argument, NULL, 'y' },
		{ "sync", required_argument, NULL, 's' },
		{ "lists", required_argument, NULL, 'l' },
		{ 0, 0, 0, 0 }		// handles invalid args
	};


	while ((opt = getopt_long(argc, argv, "", long_options, 0)) != -1)
	{

		/* what did the user specify for the optional arguments
		* */
		switch (opt)
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
		case 'l':
			read_num_parameter(&no_lists, optarg);
			break;
		default:
			print_usage(optarg);
			exit(1);
		}
	}

	//fprintf(stderr, "everything allocated\n");
	total_elements = no_iterations * no_threads;
	mutex_times = malloc(no_threads * sizeof(long long));
	elements_opt = malloc(total_elements * sizeof(SortedListElement_t*));
	ptr_keys = malloc(total_elements * sizeof(char*));
	lists_opt = malloc(no_lists * sizeof(SortedListElement_t*));
	thread_id = malloc(no_threads * sizeof(pthread_t));

	switch (sync_opt) {
	case 'm':
		mutex = malloc(sizeof(pthread_mutex_t) * no_lists);
		for (i = 0; i < no_lists; i++) {
			pthread_mutex_init(&mutex[i], NULL);
		}
		break;

	case 's':
		spin_l = malloc(sizeof(int) * no_lists);
		for (i = 0; i < no_lists; i++) {
			spin_l[i] = 0;
		}
		break;

	}


	for (i = 0; i < no_threads; i++) {
		mutex_times[i] = 0;
	}


	//fprintf(stderr, "setting all lists to null\n");
	for (i = 0; i < no_lists; ++i)
	{
		lists_opt[i] = malloc(sizeof(SortedListElement_t));
		lists_opt[i]->prev = lists_opt[i];
		lists_opt[i]->next = lists_opt[i];
		lists_opt[i]->key = NULL;
	}

	if (ptr_keys == NULL)
	{
		fprintf(stderr, "No memory for keys\n");
		exit(1);
	}

	if (thread_id == NULL)
	{
		fprintf(stderr, "Don't have access to memory.\n");
		exit(1);
	}


	//TODO: generate the random keys
	//fprintf(stderr, "generated random keys\n");
	char ran;
	for (i = 0; i < total_elements; ++i)
	{
		elements_opt[i] = malloc(sizeof(SortedListElement_t));
		elements_opt[i]->prev = NULL;
		elements_opt[i]->next = NULL;
		elements_opt[i]->key = key_gen();
	}

	signal(SIGSEGV, handler);



	clock_gettime(CLOCK_MONOTONIC, &begin);


	//fprintf(stderr, "%s\n", "Creating threads");
	//create threads
	int* thread_no = (int*)malloc(no_threads * sizeof(int));
	for (i = 0; i < no_threads; i++) {
		thread_no[i] = i;
		int ans = pthread_create(&thread_id[i], NULL, create_thread, (void*)(&thread_no[i]));
		if (ans != 0)
		{
			fprintf(stderr, "Error with creating thread\n");
			exit(1);
		}
	}

	//wait for threads to finish
	//fprintf(stderr, "%s\n", "Reaping threads");
	for (i = 0; i < no_threads; i++) {
		int ans = pthread_join(thread_id[i], NULL);
		if (ans != 0)
		{
			fprintf(stderr, "Error with joining thread\n");
			strerror(ans);
			exit(1);
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &end);

	totalmutex = 0ULL;

//	if (sync_opt == 'm')
//	{
		for (i = 0; i < no_threads; i++) {
			totalmutex = totalmutex + mutex_times[i];
		}
//	}


	//fprintf(stderr, "finding length of all lists\n");
	for (i = 0; i < no_lists; ++i)
	{
		//to double check, make sure sorted list is empty
		int ret = SortedList_length(lists_opt[i]);
		//fprintf(stderr, "Length of list %i is: %d \n", i, ret);
		if (ret != 0)
		{
			fprintf(stderr, "The list is not empty.\n");
			exit(2);
		}
	}


	//fprintf(stderr, "value of sync_opt: %d \n", sync_opt);


	//NEED TO PRINT THE RESULTS TO .CSV FILE
	fprintf(stdout, "list-");

	//switch condition for opt_yield. should be 8 cases + default
	//none, i,d,l,id,il,dl,idl

	switch (opt_yield) {
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


	switch (sync_opt) {
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

	unsigned long long totaloperations = no_iterations * no_threads * 3;
	unsigned long long totaltime = ((end.tv_sec * 1000000000) + (end.tv_nsec)) - ((begin.tv_sec * 1000000000) + (begin.tv_nsec));
	unsigned long long averagetime = totaltime / totaloperations;
	unsigned long long averagemutex = totalmutex / no_threads;


	fprintf(stdout, ",%lld,%lld,%lld,%llu,%llu,%llu,%llu \n", 
			no_threads, no_iterations, no_lists, totaloperations, totaltime, averagetime, averagemutex);
	fflush(stdout);


	for (i = 0; i < no_lists; ++i)
	{
		free(lists_opt[i]);
	}
	//free(lists_opt);
	free(mutex);
	free(spin_l);

	free(elements_opt);
	free(ptr_keys);
	free(lists_opt);
	free(thread_id);
	free(thread_no);

	

	//if everything went well
	exit(0);
}

