#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef INTLIST_H
#define INTLIST_H

typedef struct node {
	int data;
	struct node *next;
}Node;

typedef struct {
	Node *start;
	Node *end;
	int n;
}List;

Node * newNode(int );
void destroyNode(Node*);
List * newList(int );
void addNode(List*,int);
int indexInListOf(List*, int);
void printList(List*);
void destroyList(List*);
void deleteFrom(List*,int,double*);


#endif