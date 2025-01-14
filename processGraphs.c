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

Graph create_graph(int num_vertices, int num_edges, int **edge_list){
    Node **adjacency_list = malloc(num_vertices * sizeof(Node *));
    for(int i = 0; i < num_edges; i++){
        int u = edge_list[i][0];
        int v = edge_list[i][1];
        add_last(adjacency_list[u], v);
        add_last(adjacency_list[v], u);
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
    Queue q = create_queue();
    enqueue(&q, source);
    while(q.size > 0){
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
    return 0;
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