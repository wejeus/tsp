#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
	size_t n;
	unsigned short *dist;
} graph;

graph graph_alloc(size_t n)
{
	graph g;
	g.n = n;
	g.dist = malloc(n*n*sizeof(unsigned short));
	return g;
}

void graph_free(graph g)
{
	free(g.dist);
}

int graph_get_dist(graph g, size_t from, size_t to)
{
	return g.dist[g.n*from + to];
}

void graph_set_dist(graph g, size_t from, size_t to, short dist)
{
	g.dist[g.n*from + to] = dist;
}

float sq(float f)
{
	return f*f;
}

graph read_kattis()
{
	int n;
	scanf("%d", &n);
	float pos[n][2];
	for(size_t i = 0; i < n; ++i)
	{
		scanf("%f %f", &pos[i][0], &pos[i][1]);
	}

	graph g = graph_alloc(n);
	for(size_t i = 0; i < n; ++i)
	{
		for(size_t j = 0; j < n; ++j)
		{
			float dist = sqrt(sq(pos[i][0] - pos[j][0])
					+sq(pos[i][1] - pos[j][1]));
			int d = round(dist);
			graph_set_dist(g, i, j, d);
			graph_set_dist(g, j, i, d);
		}
	}
	return g;
}

unsigned int tour_length(size_t *tour, graph g)
{
	unsigned int total = 0;
	for(size_t i = 0; i < g.n-1; ++i)
		total += graph_get_dist(g, tour[i], tour[i+1]);

	total += graph_get_dist(g, tour[0], tour[g.n-1]);
	return total;
}

void print_tour(size_t *tour, graph g)
{
	for(size_t i = 0; i < g.n; ++i)
		printf("%d\n", tour[i]);
}

size_t *greedy_tsp(graph g)
{
	size_t *tour = malloc(sizeof(size_t)*g.n);
	bool used[g.n];
	memset(used, 0, g.n*sizeof(bool));

	// 
	//	for(size_t i = 0; i < g.n; ++i)
	//		tour[i] = i;
	tour[0] = 0;
	used[0] = true;
	for (size_t i = 1; i < g.n; ++i)
	{
		int best = -1;
		for(size_t j = 0; j < g.n; ++j)
		{
			if(used[j])
				continue;
			if(best == -1 || graph_get_dist(g, tour[i-1],j)
					< graph_get_dist(g, tour[i-1], best))
				best = j;
		}
		tour[i] = best;
		used[best] = true;
		printf("path: %d\n", tour[i]);
	}

	return tour;
}

size_t *solve_tsp(graph g)
{
	size_t *tour = malloc(sizeof(size_t)*g.n);
	bool used[g.n];
	memset(used, 0, g.n*sizeof(bool));

	// 
	//	for(size_t i = 0; i < g.n; ++i)
	//		tour[i] = i;
	tour[0] = 0;
	used[0] = true;
	for (size_t i = 1; i < g.n; ++i)
	{
		int best = -1;
		for(size_t j = 0; j < g.n; ++j)
		{
			if(used[j])
				continue;
			if(best == -1 || graph_get_dist(g, tour[i-1],j)
					< graph_get_dist(g, tour[i-1], best))
				best = j;
		}
		tour[i] = best;
		used[best] = true;
		printf("path: %d\n", tour[i]);
	}

	return tour;
}

int main(int argc, char *argv[])
{
	graph g = read_kattis();
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = 0; j < g.n; ++j)
			printf("%3d ", graph_get_dist(g, i, j));
		putchar('\n');
	}

	size_t *path = solve_tsp(g);

	print_tour(path, g);
	printf("\nlength=%d\n", tour_length(path, g));
	free(path);
	graph_free(g);

	return 0;
}
