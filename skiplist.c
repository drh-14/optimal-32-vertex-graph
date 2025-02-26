#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "functions.h"
#include <pthread.h>
#include <mpi.h>

typedef struct SkipNode
{
    Graph *g;
    float value;
    SkipNode **forward;
    int height;
} SkipNode;

SkipNode *create_skip_node(double key, int height, int MAX_LEVEL, float p)
{
    SkipNode *s = malloc(sizeof(SkipNode));
    int height = generate_height(p, MAX_LEVEL);
    s->height = height;
    s->value = key;
    s->forward = malloc(sizeof(SkipNode *));
    for (int i = 0; i < height; i++)
    {
        s->forward[i] = malloc(sizeof(SkipNode));
    }
    return s;
}

MPI_Win lock_memory(int comm, int rank, void *ptr, size_t size)
{
    MPI_Win window;
    MPI_Win_create(ptr, size, 1, MPI_INFO_NULL, comm, &window);
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, window);
    return window;
}


void unlock_memory(int rank, MPI_Win window)
{
    MPI_Win_unlock(rank, window);
    MPI_Win_free(&window);
}

SkipNode *get_lock(int comm, int rank, SkipNode *s, double key, int i){
    SkipNode *node = s -> forward[i];
    while(s -> value < key){
        s = node;
        node = s -> forward[i];
    }
    lock_node(comm, rank, s -> forward[i]);
    node = s -> forward[i];
    while(node -> value < key){
        unlock_node(rank, s -> forward[i]);
        s = node;
        lock_node(comm, rank, s -> forward[i]);
        node = s -> forward[i];
    }
    return s;
}

typedef struct SkipList
{
    int MAX_LEVEL;
    int level_hint;
    float p;
    SkipNode *header;
} SkipList;

SkipList *create_skip_list(int MAX_LEVEL, float p)
{
    SkipList *s = malloc(sizeof(SkipList));
    s->MAX_LEVEL = MAX_LEVEL;
    s->level_hint = MAX_LEVEL;
    s->header = create_skip_node(NAN, MAX_LEVEL, MAX_LEVEL, p);
    return s;
}

void insert_into_skip_list(int comm, int rank, SkipList *lst, Graph *g, double key)
{
    SkipNode *update[lst -> MAX_LEVEL];
    SkipNode *s = lst -> header;
    int L = lst -> level_hint;
    for(int i = L; i > 0; i--){
        SkipNode *node = s -> forward[i];
        while(node -> value < key){
            s = node;
            node = s -> forward[i];
            update[i] = s;
        }
    }
    s = get_lock(comm, rank, s, key, 1);
    if(s -> forward[1] -> value == key){
        unlock_node(rank, &s);
    }
    SkipNode *node = create_skip_node(key, random_level(), lst -> MAX_LEVEL, lst -> p);
    lock_field(comm, rank, s -> height);
    for(int i = L + 1; i > node -> height - 1; i--){
        update[i] = lst -> header;
    }
    for(int i = 1; i <= node -> height; i++){
        if(i != 1){
            s = get_lock(comm, rank, update[i], key, i);
        }
        node -> forward[i] = s -> forward[i];
        s -> forward[i] = node;
        unlock_node(rank, s -> forward[i]);
    }
    unlock_node(rank, node -> height);
    L = lst -> level_hint;
    if(L < lst -> MAX_LEVEL && lst -> header -> forward[L + 1]){
        MPI_Win window = lock_memory(comm, rank, &lst -> level_hint, sizeof(lst -> level_hint));
        while(lst -> level_hint < lst -> MAX_LEVEL && lst -> header -> forward[lst -> level_hint + 1]){
            lst -> level_hint++;
        }
        unlock_memory(rank, window);
    }
}

void delete_from_skip_list(SkipList *lst, double key)
{
    SkipNode *node;
    SkipNode *update[lst -> MAX_LEVEL];
    SkipNode *s = lst -> header;
    int L = lst -> level_hint;
    for(int i = L; i > 0; i--){
        node = s -> forward[i];
        while(node -> value < key){
            s = node;
            node = s -> forward[i];
        }
        update[i] = s;
    }
    node = s;
    do{
        node = node -> forward[0];
        
    }while(node);
}

void delete_skip_list(SkipList *lst)
{
    
}

int generate_height(float p, int MAX_LEVEL)
{
    srand(time(NULL));
    int new_level = 1;
    while ((float)rand() / RAND_MAX < p)
    {
        new_level++;
    }
    return min(new_level, MAX_LEVEL);
}