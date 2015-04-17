#include "IntList.h"

Node * newNode(int value){
	Node * node = (Node*) malloc(sizeof(Node));
	node->data = value;
	node->next = NULL;
	return node;
}

List * newList(int value){
	List * list = (List*) malloc(sizeof(List));
	list->start = newNode(value);
	list->end = list->start;
	list->n = 1;
	return list;
}


void addNode(List* this,int value){
	this->end->next = newNode(value);
	this->n++;
	this->end = this->end->next;
}

int indexInListOf(List* this, int value){
	Node * current = this->start;
	int index = 0;
	
	while(current){
		if(current->data == value)
			return index;
		index++;
		current = current->next;
	}
	
	return -1;
}

void printList(List* this){
	Node * current = this->start;
	while(current){
		printf("%d -> ", current->data);
		current = current -> next;
	}
	printf("\n");
}

void destroyNode(Node* this){
	this->next = NULL;
	free(this);
}

void destroyList(List* this){
	Node* current = this->start;
	while(current){
		this->start = current->next;
		destroyNode(current);
		current = this->start;
	}
}

void deleteFrom(List* this, int value, double * positions){
	Node* current = this->start, *prev;
	int found = 0;

	while(current){
		if(found){
			prev->next = current->next;
			positions[current->data] = -1;
			destroyNode(current);
			current = prev;
			this->n--;
		}
		if(current->data == value){
			found = 1;
			prev = current;
			this->end = current;
		}
		current = current->next;
	}
}

int* listToArray(List* this){
	Node* current = this->start;
	int* array = (int*) (malloc(this->n* sizeof(int))), i;

	while(current){
		array[i++] = current->data;
		current = current->next;
	}

	return array;
}


