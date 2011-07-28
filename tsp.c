#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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

int main(int argc, char *argv[])
{
	graph g = read_kattis();
	graph_free(g);
	for(size_t i = 0; i < g.n; ++i)
	{
		for(size_t j = 0; j < g.n; ++j)
			printf("%3d ", graph_get_dist(g, i, j));
		putchar('\n');
	}
	return 0;
}
