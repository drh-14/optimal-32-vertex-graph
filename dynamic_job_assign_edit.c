#include <C:\MSMPI\SDK\Include\mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_JOBS 20  // Total number of jobs
#define MAX_SIZE TOTAL_JOBS  // Maximum size of the queue
#define JOB_ALLOCATION_PERCENTAGE 0.25  // First 25% of jobs pre-assigned

// Defining the Queue structure
typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Function to initialize the queue
void initializeQueue(Queue* q, int start) {
    q->front = start - 1;
    q->rear = start;
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

void perform_job(int rank, int job_id, double time_start) {
    double time_now = MPI_Wtime();
    printf("At time %.6f, rank %d is processing job %d\n", time_now - time_start, rank, job_id + 1); // + 1 since considering job 1 first task (rank 1 task 1, etc)
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int size, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    double start_time = MPI_Wtime();

    int num_workers = size - 1;
    int initial_jobs = (int)(TOTAL_JOBS * JOB_ALLOCATION_PERCENTAGE);
    int jobs_per_worker = (int)(initial_jobs / num_workers); // must be divisable

    Queue jobs;
    initializeQueue(&jobs, initial_jobs);

    // Master core process
    if (myrank == 0) {
        int job_index = initial_jobs;

        // Enqueue the first batch of jobs after the auto-assigned jobs
        for (int i = initial_jobs; i < TOTAL_JOBS; i++) {
            enqueue(&jobs, i);
        }

        int worker_availability[size];  
        for (int i = 1; i < size; i++) worker_availability[i] = 1;  // Mark all workers as available

        while (!isEmpty(&jobs)) { // Continue assigning jobs while the queue is not empty
        
            for (int i = 1; i < size; i++) {  
                if (worker_availability[i] == 1 && !isEmpty(&jobs)) {  
                    int job_id = front_peek(&jobs);  // Get next job from queue
                    MPI_Send(&job_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);  // Assign job to worker i
                    dequeue(&jobs);  // Remove assigned job from queue
                    worker_availability[i] = 0;  // Mark worker as busy
                }
            }

            MPI_Status status;
            int completed_job;
            int flag;

            // Check if any worker has completed their job
            for (int i = 1; i < size; i++) {
                MPI_Iprobe(i, 1, MPI_COMM_WORLD, &flag, &status);  // Check if worker i has sent completion signal
                if (flag) {  
                    MPI_Recv(&completed_job, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);  // Receive completion message
                    worker_availability[i] = 1;  // Mark worker as available
                }
            }
        }

        // Once all jobs are completed, send termination signals to all workers
        int stop_signal = -1;
        for (int i = 1; i < size; i++) {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } 
    
    // Worker processes (rank > 0) execute assigned jobs
    else {
        int start_job = (myrank - 1) * jobs_per_worker;
        int end_job = start_job + jobs_per_worker;

        // Process pre-assigned jobs
        for (int job_id = start_job; job_id < end_job; job_id++) {
            perform_job(myrank, job_id, start_time);
        }

        // Notify master that the initial jobs are done
        int dummy_signal = 0;
        MPI_Send(&dummy_signal, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

        // Dynamic job assignment
        int job_id;
        while (1) {
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive job from master
            if (job_id == -1) break; // Exit loop if termination signal is received
            
            perform_job(myrank, job_id, start_time); // Execute assigned job
            MPI_Send(&job_id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD); // Notify master when job is completed
        }
    }

    MPI_Finalize();
    return 0;
}