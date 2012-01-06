#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
	size_t n;
	int *dist;
	size_t *neighbours;
} graph;

graph graph_alloc(size_t n)
{
	graph g;

	g.n = n;
	g.dist = malloc(n*n*sizeof(int));
	g.neighbours = malloc(n*n*sizeof(size_t));

	return g;
}

void graph_free(graph g)
{
	free(g.dist);
	free(g.neighbours);
}

static inline int graph_get_dist(graph g, size_t from, size_t to)
{
	return g.dist[g.n*from + to];
}

static inline void graph_set_dist(graph g, size_t from, size_t to, short dist)
{
	g.dist[g.n*from + to] = dist;
}

float sq(float f)
{
	return f*f;
}

static inline int graph_get_neighbour(graph g, size_t from, size_t index)
{
	return g.neighbours[g.n*from + index];
}

// bullshit
graph current_graph;
size_t current_from;

int dist_cmp(const void *a, const void *b) 
{
        size_t a2 = *(size_t *) a;
        size_t b2 = *(size_t *) b;

        return graph_get_dist(current_graph, current_from, a2) - graph_get_dist(current_graph, current_from, b2);
}

void graph_sort_neighbours(graph g)
{
	current_graph = g;
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = 0; j < g.n; ++j)
			g.neighbours[g.n*i + j] = j;
		current_from = i;
		qsort(&g.neighbours[g.n*i], g.n, sizeof(size_t), &dist_cmp);
	}
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
	graph_sort_neighbours(g);
	return g;
}

unsigned int tour_length(graph g, size_t *tour)
{
	unsigned int total = 0;
	for(size_t i = 0; i < g.n-1; ++i)
		total += graph_get_dist(g, tour[i], tour[i+1]);

	total += graph_get_dist(g, tour[0], tour[g.n-1]);
	return total;
}

void print_tour(graph g, size_t *tour)
{
	for(size_t i = 0; i < g.n; ++i)
		fprintf(stderr, "%d->", tour[i]);
	fprintf(stderr, "%d (%d)\n", tour[0], tour_length(g, tour));
}

void print_kattis(graph g, size_t *tour)
{
	for(size_t i = 0; i < g.n; ++i)
		printf("%d\n", tour[i]);
}

size_t *greedy_tsp(graph g)
{
	size_t *tour = malloc(sizeof(size_t)*g.n);
	bool used[g.n];
	memset(used, 0, g.n*sizeof(bool));

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
	}

	return tour;
}

void print_next_list(size_t *next_list)
{
	size_t next = 0;
	while(true)
	{
		fprintf(stderr, "%d->", next);
		next = next_list[next];
		if(next == 0)
			break;
	}
	fprintf(stderr, "0\n");
}

void shuffle(size_t *tour, size_t n)
{
	for(size_t i = n-1; i > 0; --i)
	{
		size_t idx = rand() % (i+1);
		size_t tmp = tour[i];
		tour[i] = tour[idx];
		tour[idx] = tmp;
	}
}

// reverses edges between from and to
void reverse(graph g, size_t *next_list, size_t from, size_t to)
{
	size_t nodes[g.n];
	nodes[0] = from;

	size_t i = 1;
	while(true)
	{
		nodes[i] = next_list[nodes[i-1]];
		if(nodes[i] == to)
		{
			break;
		}
		++i;
	}
	while(i > 0)
	{
		next_list[nodes[i]] = nodes[i-1];
		--i;
	}
}

void swap(graph g, size_t *next_list, size_t a, size_t b)
{
//	printf("swap(%d, %d)\n", a, b);
	size_t a_next = next_list[a], b_next = next_list[b];

	next_list[a] = b;
//	print_next_list(next_list);

	reverse(g, next_list, a_next, b);
	next_list[a_next] = b_next;
}

bool opt2(graph g, size_t *next_list)
{
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = next_list[next_list[i]]; next_list[j] != i; j = next_list[j])
		{
			if(graph_get_dist(g, i, next_list[i]) + graph_get_dist(g, j, next_list[j])
				> graph_get_dist(g, i, j) + graph_get_dist(g, next_list[i], next_list[j]))
			{
				swap(g, next_list, i, j);
				return true;
			}
		}
	}
	return false;
}

void solve_tsp(graph g, size_t *tour)
{
//	fputs("\n", stderr);
	for(size_t i = 0; i < g.n; ++i)
		tour[i] = i;

	shuffle(tour, g.n);
//	print_tour(g, tour);

	size_t nexts[g.n];
	for(size_t i = 0; i < g.n-1; ++i)
	{
		nexts[tour[i]] = tour[i+1];
	}
	nexts[tour[g.n-1]] = tour[0];

	for(int i = 0; i < 2000; ++i)
		if(!opt2(g, nexts))
			break;
	//while(opt2(g, nexts));

	// ll => tour
	size_t next = 0;
	while(true)
	{
		*tour++ = next;
		next = nexts[next];
		if(next == 0)
			break;
	}
	
}

int main(int argc, char *argv[])
{
	graph g = read_kattis();
	if(g.n <= 3) // all solutions identical
	{
		for(size_t i = 0 ; i < g.n; ++i)
			printf("%d\n", i);
		graph_free(g);
		return 0;
	}

	fputs("[distance matrix]\n", stderr);
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = 0; j < g.n; ++j)
			fprintf(stderr, "%3d ", graph_get_dist(g, i, j));
		fputc('\n', stderr);
	}

	fputs("\n[neighbour matrix]\n", stderr);
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = 0; j < g.n; ++j)
			fprintf(stderr, "%3d ", graph_get_neighbour(g, i, j));
		fputc('\n', stderr);
	}

	size_t *greedy = greedy_tsp(g);
	size_t *best = greedy;
	fprintf(stderr, "\n[greedy tour]\n");
	print_tour(g, greedy);
	size_t tour[g.n];
	size_t *tour_ptr = tour;

	fprintf(stderr, "\n[tours]\n");
	for(size_t i = 0; i < 1; ++i)
	{
		solve_tsp(g, tour);
		print_tour(g, tour);
		if(tour_length(g, tour) < tour_length(g, best))
		{
			size_t *tmp = best;
			best = tour_ptr;
			tour_ptr = tmp;
		}
	}

	fprintf(stderr, "\n[kattis (best)]\n");
	print_kattis(g, best);

	free(greedy);

	graph_free(g);

	return 0;
}
