#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
#include <unistd.h>
#include <time.h>

// Set cap to 20
#define MAX_PHILOSOPHERS 20

// initialize the starting variables
Zem_t *forks;
Zem_t mutex;
int num_philosophers = 0;

// Function to sleep for a random amount of time between 0 and 2 seconds
void rsleep()
{
    // Seed the random number generator using current time
    srand((unsigned int)time(NULL));

    // Generate a random number between 0 and 2
    int num = rand() % 3; // Generates a number in the range [0, 2]

    printf("Sleeping for %d seconds...\n", num);
    sleep(num); // Sleep for num seconds
}

// Function to think for a given number of seconds, puts the process to sleep.
// If -1 is passed, then sleep for a random amount of time between 0 and 2 seconds.
void think(int seconds)
{
    if (seconds == -1)
    {
        rsleep(); // If -1 is passed, sleep for
    }
    else if (seconds >= 0)
    {
        sleep(seconds); //
    }
    else
    {
        printf("Invalid sleep time!\n");
    }
}

// Function for the philosophers to attempt to grab both forks. It For algorithm 1, each
// Philosopher will try to grab the right fork, and then the left fork.

// This will result in a deadlock frequently.
void grabForks(int pid)
{
    Zem_wait(&forks[(pid + num_philosophers - 1) % num_philosophers]); // right fork
    Zem_wait(&forks[pid]);                                             // left fork
}

// This function mirrors the grabForks function above. This is different however because
// it is designed to prevent the deadlock situation because the philosopher at poisition 0
// Will always try to grab the forks in reverse order. 

// I ran this for a while without issues. 
void patchedGrabForks(int pid)
{
    if (pid == 0)
    {
        Zem_wait(&forks[pid]); // right fork
        Zem_wait(&forks[(pid + num_philosophers - 1) % num_philosophers]); // left fork
    }
    else
    {
        Zem_wait(&forks[(pid + num_philosophers - 1) % num_philosophers]); // right fork
        Zem_wait(&forks[pid]);                                             // left fork
    }
}
// Philosopher gives up both forks
void releaseForks(int pid)
{
    Zem_post(&forks[(pid + num_philosophers - 1) % num_philosophers]); // right fork
    Zem_post(&forks[pid]);                                             // left fork
}

// Simple Philosopher structure. Just holds a PID. 
typedef struct
{
    int pid; // philosopher id
} Philosopher;

// Algorithm 1, in which each philosopher tries to aquire the forks 
void *algorithm1(void *arg)
{
    Philosopher *philosopher = (Philosopher *)arg; // Create philosopher object

    while (1)
    {
        grabForks(philosopher->pid);
        //patchedGrabForks(philosopher->pid);

        printf("Philosopher %d is eating\n", philosopher->pid);
        think(-1); // Simulate eating time

        releaseForks(philosopher->pid);

        printf("Philosopher %d is thinking\n", philosopher->pid);
        think(-1); // Simulate thinking time
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invalid arguments (should be 1 argument for number of philosophers): %s <number_of_philosophers>\n", argv[0]);
        return 1;
    } // Validate arguments

    num_philosophers = atoi(argv[1]);
    if (num_philosophers < 3 || num_philosophers > MAX_PHILOSOPHERS)
    {
        printf("Number of philosophers must be between 3 and %d\n", MAX_PHILOSOPHERS);
        return 1;
    } // Validate number of philosophers



    pthread_t threads[num_philosophers]; // Array to hold the pthreads for each philosopher
    forks = (Zem_t *)malloc(num_philosophers * sizeof(Zem_t)); // Allocate the memory for the array of Zem_t for the forks the philosophers will try to grab
    Philosopher philosophers[num_philosophers]; // Array of the philosopher structs 

    for (int i = 0; i < num_philosophers; i++)
    {
        Zem_init(&forks[i], 1); // Initialize the zemaphore for each fork at the table
    }

    for (int i = 0; i < num_philosophers; i++)
    {
        philosophers[i].pid = i; // initialize the pid for each philosopher

        pthread_create(&threads[i], NULL, algorithm1, (void *)&philosophers[i]); // start the tread for each philosopher
    }

    for (int i = 0; i < num_philosophers; i++)
    {
        pthread_join(threads[i], NULL); // join the threads
    }

    free(forks); // Free the allocated memory for forks

    return 0;
}
