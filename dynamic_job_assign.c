#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_JOBS 10000  // Total number of jobs (total number of enumeration)/(constant): Choosing appropriate job number is important to reduce overhead
#define MAX_SIZE TOTAL_JOBS  // Maximum size of the queue

// Defining the Queue structure
typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Function to initialize the queue
void initializeQueue(Queue* q) {
    q->front = -1;
    q->rear = 0;
}

// Function to check if the queue is empty
bool isEmpty(Queue* q) { return (q->front == q->rear - 1); }

// Function to check if the queue is full
bool isFull(Queue* q) { return (q->rear == MAX_SIZE); }

// Function to add an element to the queue (Enqueue operation)
void enqueue(Queue* q, int value) {
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}

// Function to remove an element from the queue (Dequeue operation)
void dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}

// Function to peek at the front element of the queue
int front_peek(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1;  // Returning -1 if queue is empty (could use another sentinel value)
    }
    return q->items[q->front + 1];
}

void perform_job(int job_id) {
    // Simulate some job processing (could call another .c file here)
    printf("Processing job %d\n", job_id);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int size, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);   
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

    Queue jobs;
    initializeQueue(&jobs);
    
    // Initialize job list (you can dynamically assign jobs here)
    for (int i = 0; i < TOTAL_JOBS; i++) {
        enqueue(&jobs, i);
    }

    // Master Node (rank 0) handles job distribution
    if (myrank == 0) {
        // Initial Spread of Jobs to All Workers
        for (int i = 1; i < size; i++) {
            if (!isEmpty(&jobs)) {
                int job_id = front_peek(&jobs);
                MPI_Isend(&job_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_REQUEST_NULL);
                dequeue(&jobs); // Dequeue the job after sending
            }
        }

        // Wait for workers to complete their jobs and send back a completion signal
        for (int i = 1; i < size; i++) {
            int result;
            MPI_Recv(&result, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Worker %d finished job %d\n", i, result);
        }
    }
    // Worker Nodes (rank > 0) process jobs
    else {
        int job_id;
        
        // Workers keep receiving jobs and reporting back completion
        while (1) {
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (job_id == -1) {  // If -1 is received, stop processing (no more jobs)
                break;
            }
            perform_job(job_id);  // Simulate job processing
            
            // Send a signal back to the master indicating job completion
            MPI_Isend(&job_id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_REQUEST_NULL);
        }
    }

    MPI_Finalize();
    return 0;
}
