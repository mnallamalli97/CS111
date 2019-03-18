//Mehar Nallamalli
//804769644
//mnallamalli97@ucla.edu

#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int opt_yield;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	if (element == NULL || list == NULL)
	{
		fprintf(stderr, "element is null. \n" );
		return;
	}

	//SortedListElement_t *head = list;
	// fprintf(stderr, "%s\n", "accessing next of list");
	SortedListElement_t *curr = list->next;
	int strcmp_val;
	// fprintf(stderr, "%s\n", "Sorted List Insert");
	
	

	while(curr != list){
		strcmp_val = strcmp(element->key, curr->key);
		if (strcmp_val <= 0)
		{
			break;
		}
		curr = curr -> next;
	}

	if (opt_yield & INSERT_YIELD)
	{
		sched_yield();
	}

	element->next = curr;
	element->prev = curr->prev;
	curr->prev->next = element;
	curr->prev = element;


	// if (list->next == NULL)
	// {
	// 	if (opt_yield & INSERT_YIELD)
	// 	{
	// 		sched_yield();
	// 	}
	// 	list->next = element;
	// 	element->next = list;
	// 	element -> prev = NULL;
	// 	return;
	// }



	// if (curr == NULL)
	// {
	// 	// fprintf(stderr, "%s\n", "curr is null");
	// 	//opposite to if if list->next == NULL
	// 	head -> next = element;
	// 	element->prev = head;
	// 	element->next = NULL;
	// 	return;
	// }

	// // fprintf(stderr, "%s\n", "Exec reached here");
	// head->next = element;
	// curr->prev = element;

	// element -> next = curr; 
	// element -> prev = head;
}


int SortedList_delete(SortedListElement_t *element){
	
	if (element == NULL)
	{
		return 1;
	}

	//error
	if ((element->prev->next != element) || 
		(element->next->prev != element))
	{
		fprintf(stderr, "Pointing to a bad pointer.\n" );
		return 1;
	}


	if (opt_yield & DELETE_YIELD)
	{
		sched_yield();
	}

	//if no eror
	//fprintf(stderr, "%s\n", "trying to delete");
	element->prev->next = element->next;
	element->next->prev = element->prev;
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){

	if (list == NULL || key == NULL)
	{
		fprintf(stderr, "list or list->key is null.\n");
		return NULL;
	}

	SortedListElement_t *curr = list->next;


	while(curr != list){

		if (opt_yield & LOOKUP_YIELD){
			sched_yield();
		}


		//if same value
		if (strcmp(curr->key, key) == 0)
		{
			return curr;
		}

		curr = curr->next;
	}

	return NULL;
}


int SortedList_length(SortedList_t *list){
	int counter = 0;
	//if empty list, then return length 0
	if (list == NULL)
	{
		return counter;
	}

	SortedListElement_t *curr = list->next;



	while(curr != list){
		if (opt_yield & LOOKUP_YIELD)
		{
			sched_yield();
		}

		//increment total length
		counter++;

		curr = curr->next; 
	}

	return counter;
}


// int main(){
// 	SortedList_t* dummy = malloc(sizeof(SortedList_t));
// 	int i = 0;

	
// 		SortedListElement_t** element = malloc(5*sizeof(SortedListElement_t*));
// 		element[0] = malloc(sizeof(SortedListElement_t));
// 		element[1] = malloc(sizeof(SortedListElement_t));
// 		element[2] = malloc(sizeof(SortedListElement_t));
// 		element[3] = malloc(sizeof(SortedListElement_t));
// 		element[4] = malloc(sizeof(SortedListElement_t));
		
// 		element[0]->key = "a";
// 		element[1]->key = "b";
// 		element[2]->key = "c";
// 		element[3]->key = "d";
// 		element[4]->key = "e";
// 		for(i = 0; i < 5; i++)
// 			SortedList_insert(dummy, element[i]);



	

// 	SortedList_t* curr = dummy->next;
// 	while(curr != dummy) {
// 		printf("%s\n", curr->key);
// 		curr = curr->next;
// 	}
	
// }
