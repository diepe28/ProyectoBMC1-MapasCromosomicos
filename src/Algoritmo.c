#include "Algoritmo.h"
#include "GenomeToolsSupport.h"
#define MAX_DISTANCE 0.499999999

const double EPSILON = 0.000000001;
double** mapList;
int numMaps;
char resumeStr[2048];

double*** groupsData;
int* numberOfMapsPerGroup;
int numberOfGroups;

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
		if(!equals(matrix[gene][i],-1)) {
			addNode(list, i);
		}
	}
	
	//Recorre columna
	for(i = 0; i < gene; i++){
		if(!equals(matrix[i][gene],-1)) {
			addNode(list, i);
		}		
	}
	return list;
}

int isGeneInMap(double * positions, int gene){
	return !equals(positions[gene], -1);
}

void createAdjacentNodes(double** matrix, int nGenes){
	int i = 0;
	adjacentNodes = (List**) malloc(nGenes * sizeof(List*));
	for(; i < nGenes; i++){
		adjacentNodes[i] = createAdjacentNodesFor(i, matrix, nGenes);
	}
}

int isPositionOcuppiedOrTooFar(List* inMap, double* positions, double position){
	Node* current = inMap->start;
	
	while(current){
		if(positions[current->data] == position || fabs(position - positions[current->data]) >= 0.5 )
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
			if(!isPositionOcuppiedOrTooFar(inMap, positions, geneAdjPosition)){
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
			if(geneAdjPosition > 0 && !isPositionOcuppiedOrTooFar(inMap, positions, geneAdjPosition)){
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
		if(equals(nextGene, -1)){//we got solution
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

void createMapsForLG (double** matrix, int nGenes){
	int startGene, i, nMaps = pow(2, nGenes-1); //a lo sumo, entre 2 cuando eliminemos repetidos
	double* positions = (double*) (malloc(nGenes * sizeof(double)));
	List* inMap; 

	createAdjacentNodes(matrix, nGenes);
	printAdjancentNodes(nGenes);

	//Si hay al menos nodo sin vecinos, hay algo malo. Mejor detener.
	for(i = 0; i < nGenes; i++)
		if(adjacentNodes[i]->n == 1){
			printf("el nodo %d no tiene vecinos\n", i);
			return;
		}
	
	// maybe if map list is not null, destroy it
	mapList = (double**) malloc(nMaps * sizeof(double *));
	numMaps = 0;
	
	for(startGene = 0; startGene < nGenes; startGene++){
		resetPositions(positions, nGenes);
		inMap = newList(startGene);
		positions[startGene] = 0;
		createMapsAux(matrix, nGenes, positions, startGene, adjacentNodes[startGene]->start->next, inMap);
		destroyList(inMap);
	}

	//liberando memoria
	for(i = 0; i < nGenes; i++)
		destroyList(adjacentNodes[i]);
	free(adjacentNodes);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////PREDICTION OF GENES//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


int predictFor(int gene1/*row*/, int gene2/*col*/, double** mat, int nGenes, double *value){
	List* adjancents1 = adjacentNodes[gene1];
	List* adjancents2 = adjacentNodes[gene2];
	Node* current = adjancents1->start;
	int commonGene1 = -1, commonGene2 = -1;
	double abs1, abs2, sum1, sum2;

	if(adjancents1->n < 2 || adjancents2->n < 2) return 0;
	
	while(current){
		// if value of the matrix at mat[current->data][gene2] is not -1, then it is a gene in common 
		if (!equals(mat[current->data][gene2], -1) ){
			if(commonGene1 == -1)
				commonGene1 = current->data;
			else{
				commonGene2 = current->data;
				break;
			}
		}
		current = current->next;
	}

	// not possible to predict
	if(equals(commonGene2, -1)) return 0;

	abs1 = fabs(mat[commonGene1][gene1] - mat[commonGene1][gene2]);
	sum1 = mat[commonGene1][gene1] + mat[commonGene1][gene2];

	abs2 = fabs(mat[commonGene2][gene1] - mat[commonGene2][gene2]);
	sum2 = mat[commonGene2][gene1] + mat[commonGene2][gene2];

	if(equals(abs1,abs2) || equals(abs1,sum2) ){
		*value = abs1;
		return 1;
	}

	if(equals(sum1,abs2) || equals(sum1,sum2) ){
		*value = sum1;
		return 1;
	}

	return 0;
}

int predict(double** mat, int nGenes){
	int predictedProbs = 0, i, j;
	double probability = -1;
	char message[50];

	resumeStr[0] = 0;
	createAdjacentNodes(mat, nGenes);
	printAdjancentNodes(nGenes);
	
	//Si hay al menos nodo sin vecinos, hay algo malo. Mejor detener.
	for(i = 0; i < nGenes; i++)
		if(adjacentNodes[i]->n == 1)
			goto salida;
	
	//just upper right triangle of matriz
	for(i = 0; i < nGenes-1; i++){
		for(j = i+1; j < nGenes; j++){
			if(equals(mat[i][j], -1)){
				if(predictFor(i,j,mat,nGenes, &probability)){
					sprintf(message, "Se predice M[%d,%d] = %0.5f\n",i,j,probability);
					strcat(resumeStr, message);
					mat[i][j] = probability;
					predictedProbs++;
				}
			}
		}
	}

	
	salida:
	//liberando memoria
	for(i = 0; i < nGenes; i++)
		destroyList(adjacentNodes[i]);
	free(adjacentNodes);
		
	if(predictedProbs == 0)
		strcat(resumeStr, "Datos insuficientes o completos, no se predicen valores.\n");
	
	return predictedProbs;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////CLUSTERING OF LINKAGE GROUPS/////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double ** createMatrixForLG(List* lg, double** matrix, int nGenes){
	int * arrayLG = listToArray(lg), n, i, j;
	double** mat = (double**) (malloc(lg->n*sizeof(double*)));

	//Asigna memoria y Diagonal en 0
	for(i = 0; i < lg->n; i++){
		mat[i] = (double*)(malloc(lg->n*sizeof(double*)));
		mat[i][i] = 0;
	}

	//Recorre arrayLG y asigna en posicion correcta
	for(i = 0; i < lg->n; i++){
		for(j = i+1; j < lg->n; j++){
			mat[i][j] = mat[j][i] = matrix[arrayLG[i]][arrayLG[j]];;
		}
	}
	
	for(i = 0; i < lg->n; i++){
		for(j = 0; j < lg->n; j++){
			printf("%f  ",mat[i][j]);
		}
		printf("\n");
	}

	return mat;
}

//dado el mapa de un LG lo transforma en un mapa con -1s en cada gen que no sea parte del LG
double* rellenarMapa(double *sourceMap, int nGenesLK, int nGenesTotal, int* lk){
	double * targetMap = (double*) (malloc(nGenesTotal * sizeof(double)));
	int i;
	for(i = 0; i < nGenesTotal; targetMap[i++] = -1);

	for(i = 0; i < nGenesLK; i++){
		targetMap[lk[i]] = sourceMap[i];
	}

	return targetMap;
}

void createMapsForAllGenes(double** matrix, int nGenes){
	int n,numLG = 0, i,j;
	int* visited = (int*) (malloc(nGenes*sizeof(int)));
	List** linkageGroups = (List**) (malloc(nGenes/2 * sizeof(List*)));
	
	memset(visited, 0, nGenes);

	//Clustering (!) genes into linkage groups
	for(n = 0; n < nGenes-1; n++){
		if(!visited[n]){
			visited[n] = 1;
			linkageGroups[numLG] = newList(n);

			//explorando fila
			for(i = n, j = n+1; j < nGenes; j++){
				if(!equals(matrix[i][j],-1) && matrix[i][j] < MAX_DISTANCE){
					addNode(linkageGroups[numLG], j);
					visited[j] = 1;
				}
			}
			//explorando columna
			for(j = n,i = 0; i < n; i++){
				if(!equals(matrix[i][j],-1) && matrix[i][j] < MAX_DISTANCE){
					addNode(linkageGroups[numLG], i);
					visited[i] = 1;
				}
			}
			numLG++;
		}
	}

	groupsData = (double***) (malloc(numLG * sizeof(double**)));
	numberOfMapsPerGroup = (int*) (malloc(numLG * sizeof(int)));
	numberOfGroups = numLG;
	                               
	double** matForLG;
	double * completeMap;

	//adding data to groups data
	for(i = 0; i < numLG; i++){
		//Creating maps for linkage group[i]
		//printList(linkageGroups[i]);
		matForLG = createMatrixForLG(linkageGroups[i], matrix, nGenes);
		createMapsForLG(matForLG, linkageGroups[i]->n);
		groupsData[i] = (double**) (malloc(numMaps*sizeof(double*)));
		numberOfMapsPerGroup[i] = numMaps;

		//Completing each map to the original numberOfgenes
		for(j = 0; j < numMaps; j++){
			printPositions(mapList[j], linkageGroups[i]->n);			
			completeMap = rellenarMapa(mapList[j],linkageGroups[i]->n, nGenes, listToArray(linkageGroups[i]));
			groupsData[i][j] = completeMap; 
			printf("relleno: \n");
			//printPositions(completeMap, nGenes);
			free(mapList[j]);
		}
		free(mapList);		
	}

	for(i = 0; i < numberOfGroups; i++){
		double** LG = groupsData[i];
		int mapsPerGroup = numberOfMapsPerGroup[i];
		for(j = 0; j < mapsPerGroup; j++){
			double * map = LG[j];
			printPositions (map, nGenes);
		}
	}
	
	                               
	
}




/*
#include "Algoritmo.h"

extern double** mapList;
extern int numMaps;

int main ( int arc, char **argv ) {
	int i, numGenes = 4, probsPredicted;
	double** mat = (double**) (malloc(numGenes * sizeof(double*))); 
	
	for(i = 0; i < numGenes; i++){
		mat[i] = (double*) malloc(numGenes * sizeof(double));
	}


	mat[0][0] = -1; 	mat[0][1] = 0.4; 	mat[0][2] = -1;		mat[0][3] = 0.2 ;	mat[0][4] = -1;
	mat[1][0] = 0.4; 	mat[1][1] = -1; 	mat[1][2] = 0.2;	mat[1][3] = -1;		mat[1][4] = -1;
	mat[2][0] = -1; 	mat[2][1] = 0.2; 	mat[2][2] = -1; 	mat[2][3] = -1;		mat[2][4] = 0.1;
	mat[3][0] = 0.2; 	mat[3][1] = -1; 	mat[3][2] = -1; 	mat[3][3] = -1;		mat[3][4] = -1;
	mat[4][0] = -1; 	mat[4][1] = -1; 	mat[4][2] = 0.1; 	mat[4][3] = -1;		mat[4][4] = -1;


	mat[0][0] = -1; 	mat[0][1] = 0.2; 	mat[0][2] = 0.3;	mat[0][3] = 0.4;	
	mat[1][0] = 0.2; 	mat[1][1] = -1; 	mat[1][2] = -1;		mat[1][3] = -1;		
	mat[2][0] = 0.3; 	mat[2][1] = -1; 	mat[2][2] = -1; 	mat[2][3] = 0.1;		
	mat[3][0] = 0.4; 	mat[3][1] = -1; 	mat[3][2] = 0.1; 	mat[3][3] = -1;		


	mat[0][0] = 0;   	mat[0][1] = 0.05; 	mat[0][2] = 0.08;	mat[0][3] = 0.12;	
	mat[1][0] = 0.05; 	mat[1][1] = 0; 		mat[1][2] = 0.13;	mat[1][3] = 0.17;		
	mat[2][0] = 0.08; 	mat[2][1] = 0.13; 	mat[2][2] = 0;   	mat[2][3] = -1;		
	mat[3][0] = 0.12; 	mat[3][1] = 0.17; 	mat[3][2] = -1; 	mat[3][3] = 0;
	
	probsPredicted = predict(mat, numGenes);
	
	createMaps(mat, numGenes);
	for(i = 0; i < numMaps; i++){
		printMap(mapList[i], numGenes);
		printf ("\n\n");
	}
	

	for(i = 0; i < numGenes; i++){
		free(mat[i]);
	}

	free(mat);
	return 0;
}

*/














