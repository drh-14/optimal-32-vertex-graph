#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "header.h"
#include <stdbool.h>

typedef struct Node{
    int data;
    Node *next;
} Node;

Node *create_node(int n){
    Node *node = malloc(sizeof(Node));
    node -> data = n;
    node -> next = NULL;
    return node;
}

void delete_node(Node *n){
    if(n -> next){
        free(n -> next);
    }
    free(n);
}

void add_first(Node *head, int n){
    Node *new_head = create_node(n);
    Node *temp = head;
    head = new_head;
    head -> next = temp;
}

void add_last(Node *head, int n){
    if(!head){
        head = create_node(n);
    }
    else{

    Node *curr = head;
    while(head -> next){
        curr = curr -> next;
    }
    curr -> next = create_node(n);
    }
}

int remove_first(Node *head){
    Node *temp = head;
    head = head -> next;
    delete_node(temp);
}

int remove_last(Node *head){
    Node *curr = head;
    while(curr -> next){
        curr = curr -> next;
    }
    int val = curr -> data;
    delete_node(curr);
    return val;
}


void delete_linked_list(Node *head){
    while(head){
        Node *temp = head;
        head = head -> next;
        delete_node(temp);
    }
}

typedef struct{
    Node *head;
    int size;
}Queue;

int size(Queue q){
    return q.size;
}

void enqueue(Queue *q, int n){
    add_last(q -> head, n);
    q -> size++;
}

int dequeue(Queue *q){
    int n = remove_first(q -> head);
    q -> size--;
    return n;
}

Queue create_queue(){
    return (Queue){NULL, 0};
}

void delete_queue(Queue *q){
    delete_linked_list(q -> head);
}

typedef struct Graph{
    Node **adjacency_list;
    int num_vertices;
    int num_edges;
} Graph;

Graph initialize_graph(int num_vertices, int num_edges, int **edge_list){
    return;
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
    Queue q = create_queue();
    enqueue(&q, source);
   
}

int average_shortest_path_length(Graph g){
    return 0;
}

typedef struct ArrayHeap{
    double *arr;
    int size;
}ArrayHeap;

double top(ArrayHeap h){
    return h.arr[0];
}

ArrayHeap *create_heap(){
    ArrayHeap *h = malloc(sizeof(ArrayHeap));
    h -> arr = NULL;
    h -> size = 0;
    return h;
}