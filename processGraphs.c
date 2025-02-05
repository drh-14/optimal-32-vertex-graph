#define min(a,b) (a < b ? a : b)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <functions.h>

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

void delete_linked_list(LinkedList *lst){
    while(lst -> head){
        Node *temp = lst -> head;
        lst -> head =  lst -> head -> next;
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

int *shortest_path(int source, Graph *g){
    int dist[g -> num_vertices];
    bool visited[g -> num_vertices];
    for(int i = 0; i < g -> num_vertices; i++){
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
        Node *h = g -> adjacency_list[vertex];
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

int average_shortest_path_length(Graph *g){
    double ASPL = 0;
    for(int i = 0; i < g -> num_vertices; i++){
        double *distances = shortest_path(i, g);
        for(int j = 0; j < g -> num_vertices; j++){
            ASPL += (distances[j] / 2.0);
        }
    }
    return (int)ASPL;
}

typedef struct SkipNode{
    int value;
    SkipNode **forward;
}SkipNode;

SkipNode *create_skip_node(int val, int height){
    SkipNode *s = malloc(sizeof(SkipNode));
    s -> value = val;
    s -> forward = malloc(height * sizeof(SkipNode *));
    for(int i = 0; i < height; i++){
        s -> forward[i] = NULL;
    }
    return s;
}

typedef struct SkipList{
    int MAX_LEVEL;
    SkipNode *header;
}SkipList;

SkipList *create_skip_list(int MAX_LEVEL){
    SkipList *s = malloc(sizeof(SkipList));
    s -> MAX_LEVEL = MAX_LEVEL;
    s -> header = create_skip_node(NULL, MAX_LEVEL);
    return s;
}

SkipNode *search_skip_list(SkipList *lst, int key){
    //todo
}

void insert_into_skip_list(SkipList *lst, int key){
    //todo
}

void delete_from_skip_list(SkipList *lst, int key){
    //todo
}

int random_level(float p, int MAX_LEVEL){
    srand(time(NULL));
    int new_level = 1;
    while((float)rand() / RAND_MAX < p){
        new_level++;
    }
    return min(new_level, MAX_LEVEL);
}