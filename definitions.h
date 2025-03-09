#include <pthread.h>
#define min(a,b) (a < b ? a : b)

typedef struct Node{
    int data;
    Node *next;
} Node;

typedef struct LinkedList{
    Node *head;
    Node *tail;
    int size;
} LinkedList;

typedef struct Queue{
    LinkedList *lst;
    int size;
} Queue;

typedef struct Graph{
    Node **adjacency_list;
    int num_vertices;
} Graph;


typedef struct Pair{
    double key;
    Graph *g;
} Pair;

typedef struct Heap{
    Pair *arr;
    int size;
    int type;
} Heap;

typedef struct SkipNode{
    int height;
    float ASPL;
    Graph *g;
    SkipNode **forward;
    pthread_mutex_t nodeLock;
    pthread_mutex_t heightLock;
} SkipNode;


typedef struct SkipQueue{
    int type;
    int size;
    int MAX_LEVEL;
    float p;
    SkipNode *head;
}SkipQueue;

Node *createNode(int n);
void deleteNode(Node *n);
LinkedList *createLinkedList();
void appendToLinkedList(LinkedList *lst, int x);
int removeFirstFromLinkedList(LinkedList *lst);
int linkedListSize(LinkedList *lst);
void deleteLinkedList(LinkedList *lst);
Queue *createQueue();
int queueSize(Queue *q);
void enqueue(Queue *q, int n);
int dequeue(Queue *q);
void deleteQueue(Queue *q);
Graph *createGraph(int num_vertices);
void addEdge(Graph *g, int edge[1][2]);
void deleteGraph(Graph *g);
int *shortestPath(int source, Graph *g);
float ASPL(Graph *g);
Heap *initializeHeap(int type);
void *heapPush(Heap *h, Graph *g);
Graph *heapTop(Heap *h);
Graph *heapPop(Heap *h);
void deleteHeap(Heap *h);
SkipNode *createSkipNode(int height, int MAX_LEVEL, Graph *g);
int randomLevel(float p, int MAX_LEVEL);
SkipNode *getLock(SkipNode *node1, float key, int level);
void push(SkipQueue *q, Graph *g);