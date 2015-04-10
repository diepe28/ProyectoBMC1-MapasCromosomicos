#include "Algoritmo.h"

const double EPSILON = 0.000000001;
double** mapList;
int numMaps;

int equals(double a, double b){
	return (fabs(a - b) < EPSILON);
}

void printPositions(double *positions, int nGenes){
	int i = 0;
	for(; i < nGenes; i++){
		printf("G%d en: %f ", i, positions[i]);	
	}
	printf("\n");
}

void printMap(double *positions, int nGenes){
	int i = 0,  n = 0, index;
	double distance, minDistance = 999999, current = -1;
	
	while(n++ < nGenes) {
		minDistance = 999999;
		for(i = 0; i < nGenes; i++){
			distance = positions[i] - current;
			if(positions[i] > current && distance < minDistance){
				minDistance = distance;
				index = i;
			}
		}
		current = positions[index];
		printf("G%d en: %f ", index, positions[index]);
	}
	
	printf("\n");
}

void printAdjancentNodes(int nGenes){
	int i = 0;
	Node * current;
	
	for(; i < nGenes; i++){
		current = adjacentNodes[i]->start->next;
		printf("Adjacent nodes for: %d ", i);
		while(current){
			printf("  ->  %d", current->data);
			current = current->next;
		}
		printf("\n");
	}
}

//Para un gen, construye su lista de nodos vecinos
List* createAdjacentNodesFor(int gene, double ** matrix, int nGenes){
	List * list = newList(gene);
	int i;
	
	//Recorre Fila
	for(i = gene+1; i < nGenes; i++){
		if(matrix[gene][i] != -1) {
			addNode(list, i);
		}
	}
	
	//Recorre columna
	for(i = 0; i < gene; i++){
		if(matrix[i][gene] != -1) {
			addNode(list, i);
		}		
	}
	return list;
}

int isGeneInMap(double * positions, int gene){
	return positions[gene] != -1;
}

void createAdjacentNodes(double** matrix, int nGenes){
	int i = 0;
	adjacentNodes = (List**) malloc(nGenes * sizeof(List*));
	for(; i < nGenes; i++){
		adjacentNodes[i] = createAdjacentNodesFor(i, matrix, nGenes);
	}
}

int isPositionOcuppied(List* inMap, double* positions, double position){
	Node* current = inMap->start;
	
	while(current){
		if(positions[current->data] == position)
			return 1;
		current = current->next;
	}
	
	return 0;
}

int nextGeneInMap(List * inMap, double*positions, double currentPosition, int nGenes){
	Node* current = inMap->start->next;
	double minDistance = 99999, distance;
	int nextGene = -1;
	
/*	if(inMap->n == nGenes)
		return -1;
*/
	while(current){
		if(positions[current->data] > currentPosition){
			distance = positions[current->data] - currentPosition;
			if(distance < minDistance){
				minDistance = distance;
				nextGene = current->data;
			}
		}
		
		current = current->next;
	}
	
	return nextGene;
}


void printState(int nGenes, double* positions, int currentGene, Node * geneAdj, List* inMap){
	printf("*****************************************************\n");
	printPositions(positions, nGenes);
	printList(inMap);
	printf("Current gene is %d and it geneAdj is %d \n", currentGene, geneAdj->data);
	printf("*****************************************************\n\n");
}

void createMapsAux(double ** matrix, int nGenes, double* positions, int currentGene, Node * geneAdj, List* inMap){
	double distance, coPercentage, geneAdjPosition;
	int nextGene, lastGeneInMap;
	
	if(geneAdj != NULL){
		//printState(nGenes, positions, currentGene, geneAdj, inMap);
		coPercentage = matrix[currentGene][geneAdj->data];
		if(isGeneInMap(positions, geneAdj->data)){//gene adjancent is already in map
			distance = fabs(positions[currentGene] - positions[geneAdj->data]);
			
			if(!equals(distance,coPercentage)) {
				printf("ERROR IN MAP   ");
				printf("distance between %d and %d: is  %f, coPercentage: %f , ==? %d \n", currentGene, geneAdj->data, distance, coPercentage, equals(distance,coPercentage));
				return;
			}
			createMapsAux(matrix, nGenes, positions, currentGene, geneAdj->next, inMap);
		}else{//geneAdj not in map
			//going to the rigth
			geneAdjPosition = positions[currentGene] + coPercentage;
			if(!isPositionOcuppied(inMap, positions, geneAdjPosition)){
				positions[geneAdj->data] = geneAdjPosition;
				lastGeneInMap = inMap->end->data;
				addNode(inMap, geneAdj->data);
				createMapsAux(matrix, nGenes, positions, currentGene, geneAdj->next, inMap);
			}
			
			//bactrack, going to the left
			//printList(inMap);
			deleteFrom (inMap, lastGeneInMap, positions);
			//printList(inMap);
			geneAdjPosition = positions[currentGene] - coPercentage;
			//printf("viendo si el gen: %d puede estar en la posicion: %f \n", geneAdj->data, geneAdjPosition);
			if(geneAdjPosition > 0 && !isPositionOcuppied(inMap, positions, geneAdjPosition)){
				//printf("it is possible to go left\n");
				nextGene =  geneAdj->data;
				addNode(inMap, nextGene);
				positions[nextGene] = geneAdjPosition;
				//createMapsAux(matrix, nGenes, positions, currentGene, geneAdj->next, inMap);
				createMapsAux(matrix, nGenes, positions, nextGene, adjacentNodes[nextGene]->start->next, inMap);
			}
			
		}
	}else{// no more nodes adjacent to current gene
		nextGene = nextGeneInMap(inMap, positions, positions[currentGene], nGenes);
		//printf("next gene is: %d\n", nextGene);
		if(nextGene == -1){//we got solution
			printf("////////////WE GOT SOLUTION///////////\n");
			//printMap(positions, nGenes);
			mapList[numMaps] = (double*) malloc(nGenes * sizeof(double));
			memcpy(mapList[numMaps++], positions, nGenes*sizeof(double));
			printf("///////////////////////////////////////\n\n");
			return;
		}
		createMapsAux(matrix, nGenes, positions, nextGene, adjacentNodes[nextGene]->start->next, inMap);
	}
}

void resetPositions(double * positions, int nGenes){
	int i = 0;
	for(; i < nGenes; positions[i++] = -1);
}

void createMaps(double** matrix, int nGenes){
	int startGene, nMaps = pow(2, nGenes-1); //a lo sumo, entre 2 cuando eliminemos repetidos
	double* positions = (double*) (malloc(nGenes * sizeof(double)));
	List* inMap; 
	
	createAdjacentNodes(matrix, nGenes);
	printAdjancentNodes(nGenes);
	
	mapList = (double**) malloc(nMaps * sizeof(double *));
	numMaps = 0;
	
	for(startGene = 0; startGene < nGenes; startGene++){
		resetPositions(positions, nGenes);
		inMap = newList(startGene);
		positions[startGene] = 0;
		createMapsAux(matrix, nGenes, positions, startGene, adjacentNodes[startGene]->start->next, inMap);
		destroyList(inMap);
	}
}

/*
#include "Algoritmo.h"

extern double** mapList;
extern int numMaps;

int main ( int arc, char **argv ) {
	int i, numGenes = 5;
	double** mat = (double**) (malloc(numGenes * sizeof(double*))); 
	
	for(i = 0; i < numGenes; i++){
		mat[i] = (double*) malloc(numGenes * sizeof(double));
	}

	mat[0][0] = -1; 	mat[0][1] = 0.4; 	mat[0][2] = -1;		mat[0][3] = 0.2 ;	mat[0][4] = -1;
	mat[1][0] = 0.4; 	mat[1][1] = -1; 	mat[1][2] = 0.2;	mat[1][3] = -1;		mat[1][4] = -1;
	mat[2][0] = -1; 	mat[2][1] = 0.2; 	mat[2][2] = -1; 	mat[2][3] = -1;		mat[2][4] = 0.1;
	mat[3][0] = 0.2; 	mat[3][1] = -1; 	mat[3][2] = -1; 	mat[3][3] = -1;		mat[3][4] = -1;
	mat[4][0] = -1; 	mat[4][1] = -1; 	mat[4][2] = 0.1; 	mat[4][3] = -1;		mat[4][4] = -1;

	//mat[0][0] = -1; 	mat[0][1] = 0.2; 	mat[0][2] = 0.3;	mat[0][3] = 0.4;	
	//mat[1][0] = 0.2; 	mat[1][1] = -1; 	mat[1][2] = -1;		mat[1][3] = -1;		
	//mat[2][0] = 0.3; 	mat[2][1] = -1; 	mat[2][2] = -1; 	mat[2][3] = 0.1;		
	//mat[3][0] = 0.4; 	mat[3][1] = -1; 	mat[3][2] = 0.1; 	mat[3][3] = -1;		

	createMaps(mat, numGenes);
	for(i = 0; i < numMaps; i++){
		printMap(mapList[i], numGenes);
		printf ("\n\n");
	}
*/















