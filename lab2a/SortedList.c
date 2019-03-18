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

	SortedListElement_t *head = list;
	// fprintf(stderr, "%s\n", "accessing next of list");
	SortedListElement_t *curr = list -> next;

	// fprintf(stderr, "%s\n", "Sorted List Insert");
	

	if (list->next == NULL)
	{
		if (opt_yield & INSERT_YIELD)
		{
			sched_yield();
		}
		list->next = element;
		element->next = list;
		element -> prev = NULL;
		return;
	}

	while(curr != NULL){
		if (strcmp(element->key, curr->key) <= 0)
		{
			break;
		}
		head = curr;
		curr = curr -> next;
	}

	if (opt_yield & INSERT_YIELD)
	{
		sched_yield();
	}

	if (curr == NULL)
	{
		// fprintf(stderr, "%s\n", "curr is null");
		//opposite to if if list->next == NULL
		head -> next = element;
		element->prev = head;
		element->next = NULL;
		return;
	}

	// fprintf(stderr, "%s\n", "Exec reached here");
	head->next = element;
	curr->prev = element;

	element -> next = curr; 
	element -> prev = head;
}


int SortedList_delete(SortedListElement_t *element){
	
	if (opt_yield & DELETE_YIELD)
	{
		sched_yield();
	}


	//error
	if ((element->prev->next != element) || 
		(element->next->prev != element))
	{
		fprintf(stderr, "Pointing to a bad pointer.\n" );
		return 1;
	}

	//if no eror
	fprintf(stderr, "%s\n", "trying to delete");
	element->prev->next = element->next;
	element->next->prev = element->prev;
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){

	if (list == NULL || list->key == NULL)
	{
		fprintf(stderr, "list or list->key is null.\n");
		return NULL;
	}

	SortedListElement_t *curr = list->next;

	if (opt_yield & LOOKUP_YIELD){

		sched_yield();
	}

	while(curr != NULL){
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

	SortedListElement_t *prev = list->next;

	if (prev == NULL)
	{
		return counter;
	}

	SortedListElement_t *curr = prev->next;

	if (opt_yield & LOOKUP_YIELD)
	{
		sched_yield();
	}

	while(curr != NULL){

		//error checking
		if (curr->prev != prev && prev->next != curr)
		{
			fprintf(stderr, "Pointing to a bad pointer.\n" );
			return -1;
		}
		//increment total length
		counter++;
		//move to next blocks
		prev = curr;
		curr = curr -> next; 
	}

	return counter;
}
