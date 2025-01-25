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
    free(n -> next);
    free(n);
}

typedef struct LinkedList{
    Node *head;
    Node *tail;
    int size;
}LinkedList;

LinkedList *create_linked_list(){
    LinkedList *lst = malloc(sizeof(LinkedList));
    lst -> head = NULL;
    lst -> tail = NULL;
    lst -> size = 0;
    return lst;
}

void append_to_linked_list(LinkedList *lst, int x){
    if(lst -> size == 0){
        lst -> head = create_node(x);
    }
    else{
        lst -> tail -> next = create_node(x);
        lst -> tail = lst -> tail -> next;
    }
    lst -> size++;
}

int remove_first_from_linked_list(LinkedList *lst){
    Node *temp = lst -> head;
    lst -> head = lst -> head -> next;
    int x = temp -> data;
    delete_node(temp);
    return x;
}

int linked_list_size(LinkedList *lst){
    return lst -> size;
}

void delete_linked_list(Node *head){
    while(head){
        Node *temp = head;
        head = head -> next;
        delete_node(temp);
    }
}

typedef struct{
    LinkedList *lst;
    int size;
}Queue;

Queue *create_queue(){
    Queue *q = malloc(sizeof(Queue));
    q -> lst = NULL;
    q -> size = 0;
    return q;
}

int queue_size(Queue *q){
    return q -> size;
}

void enqueue(Queue *q, int n){
    append_to_linked_list(q -> lst, n);
    q -> size++;
}

int dequeue(Queue *q){
    int n = remove_first_from_linked_list(q -> lst);
    q -> size--;
    return n;
}

void delete_queue(Queue *q){
    delete_linked_list(q -> lst);
}

typedef struct Graph{
    Node **adjacency_list;
    int num_vertices;
} Graph;

Graph *create_graph(int num_vertices){
    Graph *g = malloc(sizeof(Graph));
    g -> adjacency_list = malloc(num_vertices * sizeof(Node *));
    g -> num_vertices = num_vertices;
    for(int i = 0; i < num_vertices; i++){
        g -> adjacency_list[i] = NULL;
    }
    return g;
}

void add_edge(Graph *g, int edge[1][2]){
    int u = edge[0][0];
    int v = edge[0][1];
    append_to_linked_list(g -> adjacency_list[u], v);
    append_to_linked_list(g -> adjacency_list[v], u);
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
    Queue *q = create_queue();
    enqueue(q, source);
    while(q -> size > 0){
        int vertex = dequeue(&q);
        Node *h = g.adjacency_list[vertex];
        Node *curr = h;
        while(curr){
            if(visited[curr -> data] == false){
                dist[curr -> data] = min(dist[curr -> data], dist[vertex] + 1);
                visited[curr -> data] = true;
            }
            curr = curr -> next;
        }
    }
    return dist;
}

int average_shortest_path_length(Graph g){
    double ASPL = 0;
    for(int i = 0; i < g.num_vertices; i++){
        double *distances = shortest_path(i, g);
        for(int j = 0; j < g.num_vertices; j++){
            ASPL += (distances[j] / 2.0);
        }
    }
    return (int)ASPL;
}

typedef struct ArrayHeap{
    double *arr;
    int size;
    int heap_type; // 0 if min heap, 1 if max heap
}ArrayHeap;

int heap_size(ArrayHeap h){
    return h.size;
}

double top(ArrayHeap h){
    return h.arr[0];
}

ArrayHeap create_heap(int heap_type){
    return (ArrayHeap){NULL, heap_type};
}

void heapify(double *arr, int heap_type, int length, int i){
    int minOrMax = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if(left < length){
        if(heap_type == 0){
            if(arr[left] < arr[minOrMax]){
                minOrMax = left;
            }
            if(arr[right] < arr[minOrMax]){
                minOrMax = right;
            }
        }
        else{
            if(arr[left] > arr[minOrMax]){
                minOrMax = left;
            }
            if(arr[right] > arr[minOrMax]){
                minOrMax = right;
            }
        }
    }
    heapify(arr, heap_type, length, minOrMax);
}

void heap_push(ArrayHeap h, double x){
    h.size++;
    h.arr = malloc(h.size * sizeof(double));
    h.arr[h.size - 1] = x;
    heapify(h.arr, h.heap_type, h.size, 0);
}

double heap_pop(ArrayHeap h){
    double val = top(h);
    for(int i = 1; i < h.size; i++){
        h.arr[i - 1] = h.arr[i];
    }
    heapify(h.arr, h.heap_type, h.size, 0);
    h.size--;
    h.arr = malloc(h.size * sizeof(double));
    return val;
}
