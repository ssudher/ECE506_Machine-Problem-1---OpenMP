#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <omp.h>
#include "timer.h"
#include "sort.h"
#include "edgelist.h"
// Order edges by id of a source vertex, 
// using the Counting Sort
// Complexity: O(E + V)
void countSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges) {

    
    int i;
    int key;
    int pos;

    // auxiliary arrays, allocated at the start up of the program
    int *vertex_cnt = (int*)malloc(numVertices*sizeof(int)); // needed for Counting Sort


    for(i = 0; i < numVertices; ++i) {
        vertex_cnt[i] = 0;
    }

    // count occurrence of key: id of a source vertex
    for(i = 0; i < numEdges; ++i) {
        key = edges[i].src;
        vertex_cnt[key]++;
    }

    // transform to cumulative sum
    for(i = 1; i < numVertices; ++i) {
        vertex_cnt[i] += vertex_cnt[i - 1];
    }

    // fill-in the sorted array of edges
    for(i = numEdges - 1; i >= 0; --i) {
        key = edges[i].src;
        pos = vertex_cnt[key] - 1;
        edges_sorted[pos] = edges[i];
        vertex_cnt[key]--;
    }


    free(vertex_cnt);

}

int grab_the_digit(int num,int dig)
{
	int mod = 1;
	int i;
	for(i = 1; i<dig; i++)
	{
		mod = mod*10;
	}
	return((num/mod)%10);
}


void transfer(struct Edge *org, struct Edge *transf, int numEdges)
{
	int i;
	//parallel
     #pragma omp parallel for num_threads(4) private(i)
	for(i=0; i<numEdges; i++)
	{
		transf[i].src = org[i].src;
		transf[i].dest = org[i].dest;
	}	
}

void my_countSort(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges, int digit) 
{
    
	int i;
    	int key;
    	int pos;
    	int dig;
	int vertex_cnt_t[10*4] = {0};
	int vertex_cnt[10] = {0,0,0,0,0,0,0,0,0,0}; 
	int num_of_threads = 4;
	//parallel
	#pragma omp parallel for num_threads(num_of_threads) private(i)
    	for(i = 0; i < numEdges; ++i) {
		int num_thread = omp_get_thread_num();
		dig = grab_the_digit(edges[i].src, digit);
        	key = dig;
        	vertex_cnt_t[(10*num_thread)+key]++;
    	}
	
	int t;
	int s;
	
	#pragma omp parallel for num_threads(num_of_threads) private(i)
	for(i = 0; i<10; i++)
	{
		s = 0;
		for(t=0; t<num_of_threads; t++)
		{
			s = s + vertex_cnt_t[(t*10)+i];
		}
		vertex_cnt[i] = s;
	}
	
    	for(i = 1; i < 10; ++i) {
        	vertex_cnt[i] += vertex_cnt[i - 1];
    	}
	
	for(i = numEdges - 1; i >= 0; --i) {
        	dig = grab_the_digit(edges[i].src, digit);
		key = dig;
        	pos = vertex_cnt[key] - 1;
        	edges_sorted[pos] = edges[i];
        	vertex_cnt[key]--;
    	}

    	transfer(edges_sorted, edges, numEdges);
}

void printdebug(struct Edge *edge_sorted, int numEdges)
{
	int i;
	printf("\n---------------------------------\n");
	for(i=0; i<numEdges; i++)
	{
		printf("%d -> %d\n",edge_sorted[i].src,edge_sorted[i].dest);
	}
	printf("\n---------------------------------\n");
}


void radixSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges) 
{
	int total_digits = 0;
	int n= numVertices;

	//find the maximum number of digits in the src
	while(n>0)
	{
		n=n/10;
		total_digits++;	
	}	
	int digi = 1;
	
	for(digi; digi<=total_digits; digi++)
	{
		my_countSort(edges_sorted,edges,numVertices,numEdges,digi);
	}		
}
