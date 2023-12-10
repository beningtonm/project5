#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//#include <semaphore.h>  // Not used, commented out

#include "common.h"  // Custom header file
#include "common_threads.h"  // Custom header file for threading utilities
#include "zemaphore.h"  // Custom header file for Zemaphore implementation

// Global variables
Zem_t mutex;  // Mutex to protect critical sections
Zem_t emptypot;  // Semaphore representing an empty pot
Zem_t fullpot;   // Semaphore representing a full pot
int servings;    // Counter for servings in the pot

// Function representing child thread behavior
void *child(void *arg) { 
    long long int value = (long long int) arg;
    int id = (int) value;
    printf("child started: %d\n",id);
    
    while(1) {
        Zem_wait(&mutex);  // Acquire the mutex lock
        
        printf("checking the pot: %d\n",id);
        if (servings == 0) {
            Zem_post(&emptypot);  // Signal that the pot is empty
            Zem_wait(&fullpot);   // Wait for the pot to be full
        }
        
        --servings;  // Decrease the number of servings available
        Zem_post(&mutex);  // Release the mutex lock
        
        printf("eating now\n");
        sleep(2);  // Simulate eating time
    }
    return NULL;
}

// Function representing mom behavior to fill the pot
void mom(int M) {
    while(1) {
        Zem_wait(&emptypot);  // Wait for the pot to be empty
        printf("filling the pot\n");
        servings = M;  // Fill the pot with servings
        sleep(5);  // Simulate time taken to fill the pot
        Zem_post(&fullpot);  // Signal that the pot is full
    }
}

int main(int argc, char *argv[]) {
    // Initialize semaphores and variables
    Zem_init(&mutex, 1);   // Initialize mutex with value 1 for mutual exclusion
    Zem_init(&emptypot, 0);  // Initialize emptypot semaphore with value 0
    Zem_init(&fullpot, 0);   // Initialize fullpot semaphore with value 0
    servings = 0;  // Initialize servings to 0
    
    printf("parent: begin\n");
    
    // Create child threads
    for (int i = 0; i < 10; ++i) {
        pthread_t c;
        long long int me = i;
        Pthread_create(&c, NULL, child, (void *)me);  // Create child thread
    }
    
    mom(10);  // Start the mom thread to fill the pot
    
    printf("parent: end\n");
    return 0;
}
