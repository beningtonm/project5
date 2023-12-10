#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
#include <unistd.h>
#include <time.h>

#define MAX_PHILOSOPHERS 20

Zem_t* forks;
Zem_t mutex;
int num_philosophers = 0;

void rsleep() {
    // Seed the random number generator using current time
    srand((unsigned int)time(NULL));
    
    // Generate a random number between 0 and 2
    int num = rand() % 3; // Generates a number in the range [0, 2]
    
    printf("Sleeping for %d seconds...\n", num);
    sleep(num); // Sleep for num seconds
}


void grabForks(int pid) {
    Zem_wait(&mutex); // Acquire the mutex lock
    
    // Check if left and right fork is available
    if ((forks[pid].value > 0) && forks[(pid + 1) % num_philosophers].value > 0 ) {
        Zem_wait(&forks[pid]); // Pick up the left fork
        Zem_wait(&forks[(pid+ 1) % num_philosophers]); // Pick up the right fork
    }
    Zem_post(&mutex); // Release the mutex lock
}

void releaseForks(int pid) {
    Zem_post(&forks[pid]); // left fork
    Zem_post(&forks[(pid + 1) % num_philosophers]); // right fork
}

typedef struct {
    int id;
} Philosopher;

void *algorithm2(void *arg) {
    Philosopher *philosopher = (Philosopher *)arg;

    while (1) {
        grabForks(philosopher->id);

        printf("Philosopher %d is eating\n", philosopher->id);
        //rsleep(); // Simulate eating time

        releaseForks(philosopher->id);

        printf("Philosopher %d is thinking\n", philosopher->id);
        //rsleep(); // Simulate thinking time
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_philosophers>\n", argv[0]);
        return 1;
    }

    num_philosophers = atoi(argv[1]);
    if (num_philosophers < 3 || num_philosophers > MAX_PHILOSOPHERS) {
        printf("Number of philosophers must be between 3 and %d\n", MAX_PHILOSOPHERS);
        return 1;
    }

    pthread_t threads[num_philosophers];
    forks = (Zem_t*)malloc(num_philosophers * sizeof(Zem_t));
    Philosopher philosophers[num_philosophers];

    Zem_init(&mutex, 1); // Initialize mutex

    for (int i = 0; i < num_philosophers; i++) {
        Zem_init(&forks[i], 1);
    }

    for (int i = 0; i < num_philosophers; i++) {
        philosophers[i].id = i;

        pthread_create(&threads[i], NULL, algorithm2, (void *)&philosophers[i]);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    free(forks); // Free the allocated memory for forks

    return 0;
}
