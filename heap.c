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
    fixHeap(h);
}

Graph *heapTop(Heap *h){
    return h -> arr[0].g;
}

void minHeapify(Heap *h, int i){
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if(left < h -> size && h -> arr[left].key < h -> arr[smallest].key){
        smallest = left;
    }
    if(right < h -> size && h -> arr[right].key < h -> arr[smallest].key){
        smallest = right;
    }
    if(smallest != i){
        Pair p = h -> arr[i];
        h -> arr[i] = h -> arr[smallest];
        h -> arr[smallest] = p;
        minHeapify(h, smallest);
    }
}

void maxHeapify(Heap *h, int i){
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if(left < h -> size && h -> arr[left].key < h -> arr[largest].key){
        largest = left;
    }
    if(right < h -> size && h -> arr[right].key < h -> arr[largest].key){
       largest = right;
    }
    if(largest != i){
        Pair p = h -> arr[i];
        h -> arr[i] = h -> arr[largest];
        h -> arr[largest] = p;
        maxHeapify(h, largest);
    }
}

void *fixHeap(Heap *h){
    for(int i = h -> size / 2 - 1; i >= 0; i--){
        h -> type ? minHeapify(h, i) : maxHeapify(h, i);
    }
}

Graph *heapPop(Heap *h){
    // remove first element
    Graph *g = h -> arr[0].g;
    for(int i = 1; i < h -> size; i++){
        h -> arr[i - 1] = h -> arr[i];
        h -> size--;
        h -> arr = realloc(h -> arr, sizeof(Pair) * h -> size);
    }
    fixHeap(h);
    return g;
}

void deleteHeap(Heap *h){
    free(h -> arr);
}