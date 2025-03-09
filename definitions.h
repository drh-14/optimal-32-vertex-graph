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
    SkipList *lst;
    int MAX_LEVEL;
    float p;
    SkipNode *head;
}SkipQueue;

typedef struct Node Node;
Node *create_node(int n);
void delete_node(Node *n);
typedef struct LinkedList LinkedList;
LinkedList *create_linked_list();
void append_to_linked_list(LinkedList *lst, int x);
int remove_first_from_linked_list(LinkedList *lst);
int linked_list_size(LinkedList *lst);
void delete_linked_list(LinkedList *lst);
typedef struct Queue Queue;
Queue *create_queue();
int queue_size(Queue *q);
void enqueue(Queue *q, int n);
int dequeue(Queue *q);
void delete_queue(Queue *q);
typedef struct Graph Graph;
Graph create_graph(int num_vertices);
void add_edge(Graph *g, int edge[1][2]);
void delete_graph(Graph *g);
int *shortest_path(int source, Graph *g);
float ASPL(Graph *g);
typedef struct SkipNode SkipNode;
SkipNode *create_skip_node(double key, int MAX_LEVEL);
typedef struct SkipList SkipList;
SkipList *create_skip_list(int MAX_LEVEL);
void insert_into_skip_list(SkipList *lst, Graph *g, double key);
void delete_from_skip_list(SkipList *lst, SkipNode *node);
void delete_skip_list(SkipList *lst);
int generate_height(float p, int MAX_LEVEL);
typedef struct SkipHeap SkipHeap;
void heap_push(SkipHeap *h, Graph *g, double ASPL);
SkipNode *heap_pop(SkipHeap *h);
void delete_heap(SkipHeap *h);
