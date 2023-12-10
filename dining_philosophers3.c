#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
#include <unistd.h>
#include <time.h>

#define MAX_PHILOSOPHERS 20

Zem_t* forks;
Zem_t* seats;
Zem_t fork_mutex;
Zem_t seat_mutex;
int num_philosophers = 0;

void rsleep() {
    // Seed the random number generator using current time
    srand((unsigned int)time(NULL));
    
    // Generate a random number between 0 and 2
    int num = rand() % 3; // Generates a number in the range [0, 2]
    
    printf("Sleeping for %d seconds...\n", num);
    sleep(num); // Sleep for num seconds
}

int grabSeat(int pid) {
    int seat = -1; // Start with an invalid seat in case philosopher cannot find a seat
    Zem_wait(&seat_mutex); // Acquire the seat_mutex lock

    // Iterate through the available seats to find an empty one
    for (int i = 0; i < num_philosophers - 1; ++i) {
        if (seats[i].value > 0) {
            seats[i].value = 0; // Occupy the seat
            seat = i;
            printf("Philosopher %d has taken seat %d\n", pid, seat);
            break;
        }
    }

    Zem_post(&seat_mutex); // Release the seat_mutex lock
    return seat;
}

void leaveSeat(int pid, int seat) {
    printf("Philosopher %d is leaving the table. Releasing seat %d.\n", pid, seat);
    Zem_post(&seats[seat]); // Leave the table
}

int grabForks(int pid, int seat) {
    // Validate the philosopher number
    if (pid < 0 || pid >= num_philosophers){
        printf("Invalid Philosopher ID: %d \n", pid);
        return -1;
    }
    // Validate the seat number
    if (seat < 0 || seat >= num_philosophers - 1){
        printf("Invalid Seat ID: %d \n", seat);
        return -1;
    }

    Zem_wait(&fork_mutex); // Acquire the fork mutex lock
    // Check if left and right fork is available
    if ((forks[seat].value > 0) && forks[(seat + 1) % num_philosophers].value > 0 ) {
        Zem_wait(&forks[seat]); // Pick up the left fork
        Zem_wait(&forks[(seat + 1) % num_philosophers]); // Pick up the right fork
    }
    Zem_post(&fork_mutex); // Release the mutex lock


    return 0;
}

void releaseForks(int seat) {
    Zem_post(&forks[seat]); // left fork
    Zem_post(&forks[(seat + 1) % num_philosophers]); // right fork
}

typedef struct {
    int id;
} Philosopher;

void *algorithm3(void *arg) {
    Philosopher *philosopher = (Philosopher *)arg;

    int seat = -1;
    while (1) {
        seat = grabSeat(philosopher->id);
        if (seat == -1){
            printf("Philosopher %d could not find a vacant seat, waiting for an opening...\n", philosopher->id);
            rsleep();
            continue;
        }
        printf("Philosopher %d is sitting in seat %d\n",philosopher->id, seat);
        grabForks(philosopher->id, seat);

        printf("Philosopher %d is eating\n", philosopher->id);
        rsleep(); // Simulate eating time

        releaseForks(seat);
        leaveSeat(philosopher->id, seat); // Philosopher leaves the table after eating

        printf("Philosopher %d is thinking\n", philosopher->id);
        // rsleep(); // Simulate thinking time
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
    seats = (Zem_t*)malloc((num_philosophers - 1) * sizeof(Zem_t)); // Create seats for each philosopher
    Philosopher philosophers[num_philosophers];

    Zem_init(&fork_mutex, 1); // Initialize mutex
    Zem_init(&seat_mutex, 1); // Initialize mutex

    for (int i = 0; i < num_philosophers; i++) {
        Zem_init(&forks[i], 1);
    }
    for (int i = 0; i < num_philosophers - 1; i++) {
        Zem_init(&seats[i], 1); // Initialize each seat
        seats[i].value = 1; // Initially set all seats as available
    }

    for (int i = 0; i < num_philosophers; i++) {
        philosophers[i].id = i;

        pthread_create(&threads[i], NULL, algorithm3, (void *)&philosophers[i]);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    free(forks); // Free the allocated memory for forks
    free(seats); // Free the allocated memory for seats

    return 0;
}
