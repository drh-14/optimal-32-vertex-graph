#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
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

int *shortestPath(int source, Graph *g){
    int dist[g->num_vertices];
    bool visited[g->num_vertices];
    for (int i = 0; i < g->num_vertices; i++){
        if (i == source){
            dist[i] = 0;
        }
        else{
            dist[i] = INFINITY;
            visited[i] = false;
        }
    }
    Queue *q = create_queue();
    enqueue(q, source);
    while (q->size > 0){
        int vertex = dequeue(&q);
        Node *h = g->adjacency_list[vertex];
        Node *curr = h;
        while (curr){
            if (visited[curr->data] == false){
                dist[curr->data] = min(dist[curr->data], dist[vertex] + 1);
                visited[curr->data] = true;
            }
            curr = curr->next;
        }
    }
    return dist;
}

float ASPL(Graph *g){
    double ASPL = 0;
    for (int i = 0; i < g->num_vertices; i++){
        double *distances = shortest_path(i, g);
        for (int j = 0; j < g->num_vertices; j++){
            ASPL += (distances[j] / 2.0);
        }
    }
    return ASPL;
}
