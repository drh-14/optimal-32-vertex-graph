#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "header.h"


typedef struct{
    int data;
    Node next;
} Node;

typedef struct{
    int size;
    Node head;
    Node tail;
} LinkedList;

void append_to_linked_list(LinkedList lst, int x){
    lst.tail.next = (Node){x, NULL};
}

void remove_first(LinkedList lst){
    lst.head = lst.head.next;
}


typedef struct{
    int *q;
    int size;
}Queue;

typedef struct{
    int **adjacency_list;
    int num_vertices;
    int num_edges;
} Graph;

typedef struct{
    int heap_type;
    int size;
    int *arr;
}ArrayHeap;

void insert_into_heap(ArrayHeap h, double x){

}

void remove_top(ArrayHeap h){
    return 0;
}

void delete_heap(ArrayHeap h){
    free(h.arr);
}

Graph build_graph(int num_vertices, int num_edges, int **edge_list){
    int **adjacency_list = malloc(sizeof(int *) * num_vertices);
    int current_index[num_vertices];
    for(int i = 0; i < num_vertices; i++){
        current_index[i] = 0;
    }
    for(int i = 0; i < num_edges; i++){
        int u = edge_list[i][0];
        int v = edge_list[i][1];
        current_index[u]++;
        current_index[v]++;
        realloc(adjacency_list[u], current_index[u] * sizeof(int));
        realloc(adjacency_list[v], current_index[v] * sizeof(int));
        adjacency_list[u][current_index[u] - 1] = v;
        adjacency_list[v][current_index[v] - 1] = u;
    }
    return (Graph){adjacency_list, num_vertices, num_edges};
}

void delete_graph(Graph g){
    for(int i = 0; i < g.num_vertices; i++){
        free(g.adjacency_list[i]);
    }
    free(g.adjacency_list);
}

int *shortest_path(int source, Graph g){
    int dist[g.num_vertices];
    for(int i = 0; i < g.num_vertices; i++){
        if(i == source){
            dist[i] = 0;
        }
        else{
            dist[i] = INFINITY;
        }
    }
    int prev[g.num_vertices];
    prev[source] = NULL;

    return 0;
}

int average_shortest_path_length(Graph g){
    return 0;
}