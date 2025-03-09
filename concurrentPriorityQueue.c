#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"

SkipNode *createSkipNode(int height, int MAX_LEVEL, Graph *g){
    SkipNode *s = malloc(sizeof(SkipNode));
    s -> height = height;
    s -> ASPL = ASPL(g);
    s -> forward = malloc(sizeof(SkipNode *));
    for (int i = 0; i < height; i++)
    {
        s->forward[i] = malloc(sizeof(SkipNode));
    }
    s -> nodeLock = PTHREAD_MUTEX_INITIALIZER;
    s -> heightLock = PTHREAD_MUTEX_INITIALIZER;
    return s;
}

int randomLevel(float p, int MAX_LEVEL){
    srand(NULL);
    int l = 1;
    while(rand() / RAND_MAX < p){
        l++;
    }
    return min(MAX_LEVEL, l);

}

SkipNode *getLock(SkipNode *node1, float key, int level){
    return NULL;
}

void push(SkipQueue *q, Graph *g){
    SkipNode *saved[q -> MAX_LEVEL];
    float key = ASPL(g);
    SkipNode *node1 = q -> head;
    SkipNode *node2;
    for(int i = q -> MAX_LEVEL; i > 0; i--){
        node2 = node1 -> forward[i];
        while(node2 -> ASPL > key){
            node1 = node2;
            node2 = node1 -> forward[i];
        }
        saved[i] = node1;
    }
    node1 = getLock(node1, key, 1);
    node2 = node1 -> forward[0];
    if(node2 -> ASPL == key){
        node2 -> g = g;
        pthread_mutex_unlock(&node1 -> forward[1] -> nodeLock);
    }
    SkipNode *newNode = createSkipNode(randomLevel(q -> p, q -> MAX_LEVEL), q -> MAX_LEVEL, g);
    pthread_mutex_lock(&newNode -> nodeLock);
    for(int i = 1; i <= q -> MAX_LEVEL; i++){
        if(i != 1){
            node1 = getLock(saved[i], key, i);
        }
        newNode -> forward[i] = node1 -> forward[i];
        node1 -> forward[i] = newNode;
        pthread_mutex_unlock(&node1 -> forward[i] -> nodeLock);
    }
    pthread_mutex_unlock(&newNode -> nodeLock);
}