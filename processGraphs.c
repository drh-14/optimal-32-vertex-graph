#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "definitions.h"
#include <pthread.h>
#include <mpi.h>


Node *createNode(int n){
    Node *node = malloc(sizeof(Node));
    node->data = n;
    node->next = NULL;
    return node;
}

void deleteNode(Node *n){
    free(n->next);
    free(n);
}

LinkedList *createLinkedList(){
    LinkedList *lst = malloc(sizeof(LinkedList));
    lst->head = NULL;
    lst->tail = NULL;
    lst->size = 0;
    return lst;
}

void appendToLinkedList(LinkedList *lst, int x){
    if (lst -> size == 0){
        lst -> head = create_node(x);
    }
    else{
        lst->tail->next = create_node(x);
        lst->tail = lst->tail->next;
    }
    lst->size++;
}

int removeFirstFromLinkedList(LinkedList *lst){
    Node *temp = lst->head;
    lst->head = lst->head->next;
    int x = temp->data;
    delete_node(temp);
    return x;
}

int linkedListSize(LinkedList *lst){
    return lst->size;
}

void deleteLinkedList(LinkedList *lst){
    while (lst->head){
        Node *temp = lst->head;
        lst->head = lst->head->next;
        delete_node(temp);
    }
}

Queue *createQueue(){
    Queue *q = malloc(sizeof(Queue));
    q->lst = NULL;
    q->size = 0;
    return q;
}

int queueSize(Queue *q){
    return q->size;
}

void enqueue(Queue *q, int n){
    append_to_linked_list(q->lst, n);
    q->size++;
}

int dequeue(Queue *q){
    int n = remove_first_from_linked_list(q->lst);
    q->size--;
    return n;
}

void deleteQueue(Queue *q){
    delete_linked_list(q->lst);
}

Graph *createGraph(int num_vertices)
{
    Graph *g = malloc(sizeof(Graph));
    g->adjacency_list = malloc(num_vertices * sizeof(Node *));
    g->num_vertices = num_vertices;
    for (int i = 0; i < num_vertices; i++){
        g->adjacency_list[i] = NULL;
    }
    return g;
}

void addEdge(Graph *g, int edge[1][2]){
    int u = edge[0][0];
    int v = edge[0][1];
    append_to_linked_list(g->adjacency_list[u], v);
    append_to_linked_list(g->adjacency_list[v], u);
}

void deleteGraph(Graph *g){
    for (int i = 0; i < g->num_vertices; i++){
        delete_linked_list(g->adjacency_list[i]);
    }
}

int *shortestPath(int source, int n, int degree, int **adjList){
    int *dist = malloc(sizeof(int) * n);
    bool visited[n];
    for (int i = 0; i < n; i++){
        if (i == source){
            dist[i] = 0;
        }
        else{
            dist[i] = INT32_MAX;
            visited[i] = false;
        }
    }
    Queue *q = create_queue();
    enqueue(q, source);
    visited[source] = true;
    while (q->size > 0){
        int vertex = dequeue(q);
        for(int i = 1; i <= degree; i++){
            if (visited[adjList[vertex][i]] == false){
                dist[adjList[vertex][i]] = min(dist[adjList[vertex][i]], dist[vertex] + 1);
                visited[adjList[vertex][i]] = true;
                enqueue(q, adjList[vertex][i]);
            }
        }
    }
    return dist;
}

double ASPL(int n, int degree, int **adjList){
    double totalLength = 0.0;
    double ASPL = 0.0;
    int pairs = 0;
    for (int i = 0; i < n; i++){
        double *distances = shortest_path(i, n, degree, adjList);
        for (int j = i + 1; j < n; j++){
            totalLength += distances[j];
            pairs++;
        }
        free(distances);
    }
    return totalLength / pairs;
}
