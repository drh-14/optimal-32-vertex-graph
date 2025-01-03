#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "header.h"
#include <stdbool.h>

typedef union{
    int a;
    double b;
}Number;

typedef struct Node{
    Number data;
    Node *next;
} Node;

void delete_node(Node *n){
    if(n -> next){
        free(n -> next);
    }
    free(n);
}

typedef struct LinkedList{
    int size;
    Node *head;
    Node *tail;
} LinkedList;

LinkedList *initialize_linked_list(){
    LinkedList *lst = malloc(sizeof(LinkedList));
    lst -> size = 0;
    lst -> head = NULL;
    lst -> tail = NULL;
    return lst;
}

void add_first(LinkedList *lst, Number x){
    Node *node = malloc(sizeof(Node));
    node -> data = x;
    if(lst -> size == 0){
        node -> next = NULL;
        lst -> head = node;
        lst -> tail = node;
    }
    else{
        node -> next = lst -> head;
        lst -> head = node; 
    }
    lst -> size++;
}

Number remove_first(LinkedList *lst){
    Number val = lst -> head -> data;
    Node *n = lst -> head;
    lst -> head = lst -> head -> next;
    delete_node(n);
    lst -> size--;
    return val;
}

void add_last(LinkedList *lst, Number x){
    Node *node = malloc(sizeof(Node));
    node -> data = x;
    node -> next = NULL;
    if(lst -> size == 0){
        lst -> head = node;
        lst -> tail = node;
    }
    else{
        lst -> tail -> next = node;
        lst -> tail = node;
    }
    lst -> size++;
}

void delete_linked_list(LinkedList *lst){
    while(lst -> size > 0){
        remove_first(lst);
    }
}

typedef struct{
    LinkedList lst;
    int size;
}Queue;

int size(Queue q){
    return q.lst.size;
}

void enqueue(Queue *q, Number x){
    add_last(&(q -> lst), x);
    q -> size++;
}

Number dequeue(Queue *q){
    Number n = remove_first(&(q -> lst));
    q -> size--;
    return n;
}

int queue_size(Queue q){
    return q.size;
}

Queue initialize_queue(){
    return (Queue){initialize_linked_list(), 0};
}

void delete_queue(Queue *q){
    delete_linked_list(&(q -> lst));
}

typedef struct Graph{
    Node **adjacency_list;
    int num_vertices;
    int num_edges;
} Graph;

Graph initialize_graph(int num_vertices, int num_edges, int **edge_list){
    LinkedList **adjacency_list = malloc(num_vertices * sizeof(LinkedList *));
    for(int i = 0; i < num_vertices; i++){
        adjacency_list[i] = initialize_linked_list();
    }
    for(int j = 0; j < num_edges; j++){
        int u = edge_list[j][0];
        int v = edge_list[j][1];
        add_last(adjacency_list[u], (Number)v);
        add_last(adjacency_list[v], (Number)u);
    }
    return (Graph){adjacency_list, num_vertices, num_edges};
}

void delete_graph(Graph *g){
    for(int i = 0; i < g -> num_vertices; i++){
        delete_linked_list(g -> adjacency_list[i]);
    }
}

int *shortest_path(int source, Graph g){
    int dist[g.num_vertices];
    bool visited[g.num_vertices];
    for(int i = 0; i < g.num_vertices; i++){
        if(i == source){
            dist[i] = 0;
        }
        else{
            dist[i] = INFINITY;
            visited[i] = false;
        }
    }
   
}

int average_shortest_path_length(Graph g){
    return 0;
}