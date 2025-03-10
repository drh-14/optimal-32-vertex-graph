#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>  

int total_jobs;  


//----- QUEUE-----
typedef struct {
    int *items;
    int front;
    int rear;
    int capacity;
} Queue;
void initializeQueue(Queue* q, int capacity) {
    q->items = (int*) malloc(capacity * sizeof(int));
    q->capacity = capacity;
    q->front = -1;
    q->rear = 0;
}
bool isEmpty(Queue* q) { 
    return (q->front == q->rear - 1); 
}
bool isFull(Queue* q) { 
    return (q->rear == q->capacity); 
}
void enqueue(Queue* q, int value) {
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}
void dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}
int front_peek(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1;
    }
    return q->items[q->front + 1];
}



int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <start_job> <end_job> <total_jobs>\n", argv[0]);
        exit(1);
    }
    int start_job = atoi(argv[1]);
    int end_job   = atoi(argv[2]);
    total_jobs    = atoi(argv[3]);

    if (start_job > end_job) {
        fprintf(stderr, "Error: start_job must be <= end_job\n");
        exit(1);
    }
    
    int num_jobs = end_job - start_job + 1;

    MPI_Init(&argc, &argv);
    int size, myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);   
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    Queue jobs;
    initializeQueue(&jobs, num_jobs);

    for (int i = start_job; i <= end_job; i++) {
        enqueue(&jobs, i);
    }
    if (myrank == 0) {
        int worker_availability[size];
        for (int i = 1; i < size; i++)
            worker_availability[i] = 1;  

        while (!isEmpty(&jobs)) {
            for (int i = 1; i < size; i++) {
                if (worker_availability[i] == 1 && !isEmpty(&jobs)) {
                    int job_id = front_peek(&jobs);
                    printf("Master sending job %d to Node %d\n", job_id, i);
                    MPI_Send(&job_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    worker_availability[i] = 0;
                    dequeue(&jobs);
                }
            }

            MPI_Status status;
            int completed_job;
            int flag;
            for (int i = 1; i < size; i++) {
                MPI_Iprobe(i, 1, MPI_COMM_WORLD, &flag, &status);
                if (flag) {
                    MPI_Recv(&completed_job, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                    printf("Master received completion signal from Node %d for job %d\n", i, completed_job);
                    worker_availability[i] = 1;
                }
            }
        }

        int stop_signal = -1;
        for (int i = 1; i < size; i++) {
            MPI_Send(&stop_signal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else { 
        int job_id;
        while (1) {
            MPI_Recv(&job_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (job_id == -1) break;
            printf("Node %d received job %d from master\n", myrank, job_id);

            char command[100];
            sprintf(command, "./genreg 32 3 4 -s -m %d %d", job_id, total_jobs);
            printf("Node %d executing: %s\n", myrank, command);
            system(command);

            printf("Node %d completed job %d, sending signal to master\n", myrank, job_id);
            MPI_Send(&job_id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
    }
    free(jobs.items);
    MPI_Finalize();
    return 0;
}
