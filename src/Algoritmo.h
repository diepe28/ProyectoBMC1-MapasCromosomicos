#include "IntList.h"

#ifndef ALGORITMO_H
#define ALGORITMO_H

typedef struct{
	char * name;
	double position;
}Gene;

// Lista de vecinos de cada nodo
List** adjacentNodes;
extern const double EPSILON;
extern double ** mapList;
extern int numMaps;
extern char resumeStr[2048];

extern double*** groupsData;
extern int* numberOfMapsPerGroup;
extern int numberOfGroups;

int equals(double, double);
void printPositions(double *, int );
void printMap(double *, int );
void printAdjancentNodes(int );
List* createAdjacentNodesFor(int , double ** , int );
int isGeneInMap(double * , int );
void createAdjacentNodes(double** , int );
int isPositionOcuppied(List* , double* , double );
int nextGeneInMap(List * , double*, double , int );
void printState(int , double* , int , Node * , List* );
void createMapsAux(double ** , int , double* , int , Node * , List* );
void resetPositions(double * , int );
void createMapsForLG(double** , int );

int predictFor(int, int, double**, int, double*);
int predict(double** , int );

void createMapsForAllGenes(double** , int );

#endif