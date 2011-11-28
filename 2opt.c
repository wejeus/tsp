
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	float x;
	float y;
} pair;

void printCoordinates(pair*, size_t);
void printGraphMatrix(int**, size_t);
void readGraph();
float sq(float);

float sq(float f) {
	return f*f;
}

/**
 * Reads n points in the cartesian plane and store distances for each point to
 * every other point in a graph represented by an upper triangular matrix.
 * I.e (i->value, x->not used (undefined)):
 * i i i
 * x i i
 * x x i
 */
void readGraph() {
	// Read number of entries that will follow and later parsed
	int n;
	scanf("%d", &n);
	
	pair *coordinates = malloc(n*sizeof(pair));
	
	// Read coorinates for points from stdin
	for(size_t i = 0; i < n; ++i) {
		scanf("%f %f", &coordinates[i].x, &coordinates[i].y);
	}
	
	// Debug
	printCoordinates(coordinates, n);	

	// The graph is an uppger triangular matrix
	int **graph = malloc((n-1)*sizeof(int*));
	
	// Determine distance for each node to all other nodes
	for(size_t i = 0; i < n-1; i++) {
		graph[i] = malloc((n-1)*sizeof(int*));
		for(size_t j = i; j < n-1; j++) {
			printf("*");
			float dist = sqrt(sq(coordinates[i].x - coordinates[i+1].x) + sq(coordinates[i].y - coordinates[i+1].y));
			int d = round(dist);
			graph[i][j] = d;
		}
		printf("\n");
	}

	printGraphMatrix(graph, n-1);
}

/* Print a vector of coordinates, newline on each entry */
void printCoordinates(pair coordinates[], size_t length) {
	for (size_t i = 0; i < length; i++) {
		printf("(%f, %f)\n", coordinates[i].x, coordinates[i].x);
	}
}

/* Print the matrix representing a graph */
void printGraphMatrix(int **graph, size_t n) {
	for (size_t i = 0; i < n; i++) {
		for (size_t j = i; j < n; j++) {
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {

	readGraph();

	exit(0);
}