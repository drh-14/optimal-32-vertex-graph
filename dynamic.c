#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_JOBS 30  // Total number of jobs (total number of enumeration)/(constant)
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
        return -1;
    }
    return q->items[q->front + 1];
}

// Simulated job execution function
void perform_job(int job_id) {
    printf("Node processing job %d\n", job_id);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int size, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);   
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

    Queue jobs;
    initializeQueue(&jobs);

    // Populate job queue with job IDs (0 to TOTAL_JOBS - 1)
    for (int i = 0; i < TOTAL_JOBS; i++) {
        enqueue(&jobs, i);
    }

    if (myrank == 0) {  // Master process
        int worker_availability[size];  
        for (int i = 1; i < size; i++) worker_availability[i] = 1;  // Mark all workers as available

        while (!isEmpty(&jobs)) {  // Continue assigning jobs while jobs remain
            // Assign jobs to available workers
            for (int i = 1; i < size; i++) {  
                if (worker_availability[i] == 1 && !isEmpty(&jobs)) {  
                    int job_id = front_peek(&jobs);  // Get next job from queue
                    printf("Master sending job %d to Node %d\n", job_id, i);
                    MPI_Send(&job_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);  // Assign job to worker
                    worker_availability[i] = 0;  // Mark worker as busy
                    dequeue(&jobs);  // Remove assigned job from queue
                }
            }

            MPI_Status status;
            int completed_job;
            int flag;

            // Check if any worker has completed their job
            for (int i = 1; i < size; i++) {
                MPI_Iprobe(i, 1, MPI_COMM_WORLD, &flag, &status);  // Check for completion signal
                if (flag) {  
                    MPI_Recv(&completed_job, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);  // Receive completion message
                    printf("Master received completion signal from Node %d for job %d\n", i, completed_job);
                    worker_availability[i] = 1;  // Mark worker as available
                }
            }
        }

        // Send termination signals to all workers
        int stop_signal = -1;
        for (int i = 1; i < size; i++) {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } 
    
    else {  // Worker processes
        int job_id;
        while (1) {
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // Receive job from master
            if (job_id == -1) break;  // Termination signal

            printf("Node %d received job %d from master\n", myrank, job_id);
            perform_job(job_id);  // Execute assigned job
            printf("Node %d completed job %d, sending signal to master\n", myrank, job_id);
            MPI_Send(&job_id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);  // Notify master that job is complete
        }
    }

    MPI_Finalize();
    return 0;
}
