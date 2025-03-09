#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include "definitions.h"
#include <pthread.h>
#include <mpi.h>


SkipNode *createSkipNode(double key, int height, int MAX_LEVEL, float p, Graph *g){
    SkipNode *s = malloc(sizeof(SkipNode));
    int height = generate_height(p, MAX_LEVEL);
    s -> height = height;
    s -> value = key;
    s -> forward = malloc(sizeof(SkipNode *));
    for (int i = 0; i < height; i++)
    {
        s->forward[i] = malloc(sizeof(SkipNode));
    }
    s -> nodeLock = PTHREAD_MUTEX_INITIALIZER;
    s -> heightLock = PTHREAD_MUTEX_INITIALIZER;
    return s;
}

SkipList *createSkipList(int MAX_LEVEL, float p){
    SkipList *s = malloc(sizeof(SkipList));
    s->MAX_LEVEL = MAX_LEVEL;
    s->levelHint = MAX_LEVEL;
    s->header = createSkipNode(NAN, MAX_LEVEL, MAX_LEVEL, p, 01);
    return s;
}

SkipNode *getLock(SkipNode *node, double key, int level){
    SkipNode *node2 = node -> forward[level];
    while(node2 -> value < key){
        node = node2;
        node2 = node -> forward[level];
    }
    pthread_mutex_lock(&node -> heightLock);
    node2 = node -> forward[level];
    while(node2 -> value < key){
        pthread_mutex_unlock(&node -> heightLock);
        node = node2;
        lock(&node -> heightLock);
    }
    return node;
}

int randomLevel(float p, SkipQueue *s){
    srand(time(NULL));
    int l = 1;
    while ((float)rand() / RAND_MAX < p){
        l++;
    }
    return min(l, s -> lst -> MAX_LEVEL);
}

void insertIntoSkipList(SkipList *lst, double key){
    
}

void deleteFromSkipList(SkipList *lst, double key){

}

void deleteSkipList(SkipList *lst){

    
}