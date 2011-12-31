
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Declarations --------------------------------------------------------------------- */

typedef struct {
	float x;
	float y;
} pair;

typedef struct {
	int from;
	int to;
} directed_edge;

typedef struct node {
	struct node *next;
	struct node *prev;
	int id;
} node_t;

typedef struct {
	node_t *path; 		// LinkedList of nodes
	int **matrix;			
	int **nearest_neighbours;	// Sorted neighbour lists (ALWAYS IGNORE FIRST ENTRY (SELF))
	int size;				// Number of vertrices (complete graph, num edges = size*size)
} tsp_graph_t;
	
void printCoordinates(pair*, size_t); // TODO change to vector
void printEdgeMatrix(int**, size_t);
void readGraph(tsp_graph_t *graph);
float sq(float);
void m_free(tsp_graph_t *type);
void printPath(tsp_graph_t *graph);
void constructSequentialPath(tsp_graph_t *graph);
node_t* findNode(node_t *aux[], int id);
void printFullPath(node_t **node_ar, int size);
void swap(tsp_graph_t *graph, node_t *n1, node_t *n2);

size_t nthClosestNeighbour(tsp_graph_t *graph, size_t n); // gets 1st, 2nd, 3rd neighbour etc.


/* Definitions ---------------------------------------------------------------------- */

float sq(float f) { return f*f; }

int graphDist(tsp_graph_t *graph, size_t from, size_t to)
{
		return graph->matrix[from][to];
}

tsp_graph_t *current_graph;
size_t current_from;
int distCompare(const void *a, const void *b)
{
	size_t a2 = *(size_t *) a;
	size_t b2 = *(size_t *) b;

	return graphDist(current_graph, current_from, a2) - graphDist(current_graph, current_from, b2);
}

void fillNearestNeighbours(tsp_graph_t *graph)
{
	size_t n = graph->size;
	// Allocate multidim matrix
	graph->nearest_neighbours = (int **) malloc(n*sizeof(int *));
	for(size_t i = 0; i < n; ++i)
		graph->nearest_neighbours[i] = (int *) malloc(n*sizeof(int));

	current_graph = graph;
	for(size_t i = 0; i < n; ++i)
	{
		for(size_t j = 0; j < n; ++j)
			graph->nearest_neighbours[i][j] = j;
		current_from = i;
		qsort(graph->nearest_neighbours[i], graph->size, sizeof(int), &distCompare);
	}
}

/**
 * Reads n points in the cartesian plane and store distances for each point to
 * every other point in a graph represented by a matrix.
 */
void readGraph(tsp_graph_t *graph) {
	// Read number of entries that will follow and later parsed
	int n;
	scanf("%d", &n);

	pair *coordinates = malloc(n*sizeof(pair));
	
	// Read coorinates for points from stdin
	for(size_t i = 0; i < n; ++i) {
		scanf("%f %f", &coordinates[i].x, &coordinates[i].y);
	}
	
	graph->size = n;
	
	// Allocate multidim matrix
	graph->matrix = malloc(n*sizeof(int*));
	for(size_t i = 0; i < n; i++) {
		graph->matrix[i] = malloc(n*sizeof(int));
	}

	
	// Determine distance for each node to all other nodes
	for(size_t i = 0; i < n; i++) {
		for(size_t j = 0; j < n; j++) {
			float dist = sqrt(sq(coordinates[i].x - coordinates[j].x) + sq(coordinates[i].y - coordinates[j].y));
			int d = round(dist);
			graph->matrix[i][j] = d;
		}
	}

	constructSequentialPath(graph);
	// fillNearestNeighbours(graph);
	
	free(coordinates);
}

void constructSequentialPath(tsp_graph_t *graph) {
	node_t *start = malloc(sizeof(node_t));
	graph->path = start;
	graph->path->id = 0;
	
	for (size_t i = 1; i < graph->size; ++i) {
		node_t *last = graph->path;
		graph->path->next = malloc(sizeof(node_t));
		graph->path = graph->path->next;
		
		graph->path->id = i;
		graph->path->prev = last;
	}
	
	// Reset pointer and close loop and update prev pointer for start node
	graph->path->next = start;
	start->prev = graph->path;
	graph->path = start;
}

void printPath(tsp_graph_t *graph) {
	node_t *current = graph->path;
	int length = 0;
	for (size_t i = 0; i < graph->size; ++i) {
		if (i != 0) {
			printf("%d(%d)", current->id, length += graph->matrix[current->prev->id][current->id]);
		} else {
			printf("%d(0)", current->id);
		}
		if (i != graph->size-1) {
			printf(" -> ");
		}
		current = current->next;
	}
	printf(" -> (closed)\n");
}

void printFullPath(node_t **node_ar, int size) {
	for (size_t i = 0; i < size; ++i) {
		node_t *current = node_ar[i];
		
		int p,n;
		if (current->prev != NULL) 
			p = current->prev->id;
		else
			p = -1;
			
		if (current->next != NULL)
			n = current->next->id;
		else
			n = -1;
			
		printf("(%d), prev=%d, next=%d\n", current->id, p, n);
	}
}

/*
Algorithm: starts in node with no inclining edges, go backwards
reverseing edges from previous node untill a node with no inclining edges.
(there should must be one due to edge reversing and some edges have been
removed). The must now be a node in the list of nodes involved in the edge
exchange that points to this one, add the this edges (but reversed). Done.

Assumed size is bigger than 4 (unknown otherwise)
Returns the number of edges reversed or -1 on failure.
*/
int reverseEdges(tsp_graph_t *graph, node_t *aux[], directed_edge e1, directed_edge e2) {
	int rev_edges = 0;
	node_t *node;
	
	for (size_t i = 0; i < 4; ++i) {
		if (aux[i]->next == NULL && e1.from != aux[i]->id && e2.from != aux[i]->id) {
			node = aux[i];
			break;
		}
	}
	
	// Init
	node_t *last_prev = node;
	node->next = node->prev;
	node = node->next;
	++rev_edges;

	while(1) {
		if (node->prev != NULL) {
			node->next = node->prev;
			node->prev = last_prev;
			last_prev = node;

			// Move backward by going forward =)
			node = node->next;
			++rev_edges;
		} else {
			node->prev = last_prev;
			node_t *final_node;
			directed_edge last_aux;
			/* either e1 or e2 points to this node */
			if (e1.to == node->id) {
				final_node = findNode(aux, e1.from);
				last_aux = e2;
			} else {
				final_node = findNode(aux, e2.from);
				last_aux = e1;
			}
			// printf("Marked node (after rev) is: %d -> %d\n", node->id, final_node->id);
			final_node->prev = node;
			node->next = final_node;
			
			node_t *from = findNode(aux, last_aux.from);
			node_t *to = findNode(aux, last_aux.to);
			
			from->next = to;
			to->prev = from;
			
			break;
		}
	}
	
	return rev_edges;
}

node_t* findNode(node_t *aux[], int id) {
	for (size_t i = 0; i < 4; ++i) {
		if (aux[i]->id == id) {
			return aux[i];
		}
	}
	return 0;
}

/* Print a vector of coordinates, newline on each entry */
void printCoordinates(pair coordinates[], size_t length) {
	for (size_t i = 0; i < length; i++) {
		printf("(%f, %f)\n", coordinates[i].x, coordinates[i].x);
	}
}

/* Prints a upper triangular matrix representing a graph */
void printEdgeMatrix(int **matrix, size_t n) {
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			printf("%3d ", matrix[i][j]);
		}
		printf("\n");
	}
}

void m_free(tsp_graph_t *type) {
	free(type->path);
	for(size_t i = 0; i < type->size; ++i)
		free(type->matrix[i]);
	free(type->matrix);
	for(size_t i = 0; i < type->size; ++i)
		free(type->nearest_neighbours[i]);
	free(type->nearest_neighbours);
	free(type);
}

/* 
swaps edges between edge (n1, n1->next) and (n2, n2->next)
giving (n1, n2), (n1->next, n2->next), or we have disjoint cycles 
*/
void swap(tsp_graph_t *graph, node_t *n1, node_t *n2) {
	
	node_t *tmpN1Next = n1->next;
	node_t *tmpN2Next = n2->next;
	
	n1->next = NULL;
	tmpN1Next->prev = NULL;	
	n2->next = NULL;
	tmpN2Next->prev = NULL;
	
	directed_edge e1, e2;

	// Create new edge pair in such a way that one node has two inclining
	// edges and one has none.
	e1.from = n1->id;
	e1.to = n2->id;
	e2.from = tmpN2Next->id;
	e2.to = tmpN1Next->id;
	
	printf("aux edges, E1: (%d->%d) E2: (%d->%d)\n", e1.from, e1.to, e2.from, e2.to);
	
	node_t **aux = malloc(4*sizeof(node_t*));	
	aux[0] = n1;
	aux[2] = tmpN1Next;
	aux[1] = n2;
	aux[3] = tmpN2Next;
	
	
	int numrev = reverseEdges(graph, aux, e1, e2);
	printf("Number of reversed edges was: %d\n", numrev);
}


int main(int argc, char *argv[]) {
	
	tsp_graph_t *graph = malloc(sizeof(tsp_graph_t));
	
	readGraph(graph);
	
	printEdgeMatrix(graph->matrix, graph->size);
	puts("distance order");
	// printEdgeMatrix(graph->nearest_neighbours, graph->size);
	printPath(graph);
	
	// (only for debug) statically picks two edges and swaps them
	node_t **node_ar = malloc(graph->size*sizeof(node_t*));
	node_t *start = graph->path;
	for (size_t i = 0; i < graph->size; ++i) {
		node_ar[i] = graph->path;
		graph->path = graph->path->next;
	}
	graph->path = start;
	
		printFullPath(node_ar, graph->size);
	swap(graph, node_ar[0], node_ar[3]);
		printFullPath(node_ar, graph->size);
		printPath(graph);

	// m_free(graph);
	exit(0);
}
