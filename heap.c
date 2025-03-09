#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>

Heap *initializeHeap(int type){
    Heap *h = malloc(sizeof(Heap));
    h -> size = 0;
    h -> arr = malloc(sizeof(Pair *));
    h -> type = type;
}

void *heapPush(Heap *h, Graph *g){
    Pair p = {ASPL(g), g};
    h -> size++;
    h -> arr = realloc(h -> arr, h -> size * sizeof(Pair));
    h -> arr[h -> size - 1] = p; 
}

Graph *heapTop(Heap *h){
    return h -> arr[0].g;
}

Graph *heapPop(Heap *h){
    // remove first element
    Graph *g = h -> arr[0].g;
    for(int i = 1; i < h -> size; i++){
        h -> arr[i - 1] = h -> arr[i];
        h -> size--;
        h -> arr = realloc(h -> arr, sizeof(Pair) * h -> size);
    }
    // heapify

    return g;
}

void deleteHeap(Heap *h){
    free(h -> arr);
}