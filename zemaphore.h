#ifndef __zemaphore_h__
#define __zemaphore_h__

#include <pthread.h>

typedef struct __Zem_t {
    int value;              // Integer value to represent the semaphore count
    pthread_cond_t cond;    // Condition variable for waiting threads
    pthread_mutex_t lock;   // Mutex lock for mutual exclusion
} Zem_t;

// Function to initialize the semaphore with a given value
void Zem_init(Zem_t *z, int value) {
    z->value = value;               // Initialize semaphore value
    pthread_cond_init(&z->cond, NULL);   // Initialize condition variable
    pthread_mutex_init(&z->lock, NULL);  // Initialize mutex lock
}

// Function for a thread to wait on the semaphore
void Zem_wait(Zem_t *z) {
    pthread_mutex_lock(&z->lock);   // Lock the mutex to ensure mutual exclusion
    
    while (z->value <= 0) {    // While semaphore value is non-positive
        pthread_cond_wait(&z->cond, &z->lock);   // Wait on the condition variable
    }
    z->value--;     // Decrement semaphore value to signify a resource is taken
    
    pthread_mutex_unlock(&z->lock); // Unlock the mutex after modification
}

// Function to post (signal) the semaphore, allowing one waiting thread to proceed
void Zem_post(Zem_t *z) {
    pthread_mutex_lock(&z->lock);   // Lock the mutex
    
    z->value++;     // Increment semaphore value to signify a released resource
    pthread_cond_signal(&z->cond); // Signal waiting thread (if any)
    
    pthread_mutex_unlock(&z->lock); // Unlock the mutex after modification
}

#ifdef __APPLE__
typedef Zem_t sem_t;    // Typedef sem_t to Zem_t for Apple platforms

#define Sem_wait(s)    Zem_wait(s)    // Map Sem_wait to Zem_wait
#define Sem_post(s)    Zem_post(s)    // Map Sem_post to Zem_post
#define Sem_init(s, v) Zem_init(s, v) // Map Sem_init to Zem_init
#endif

#endif // __zemaphore_h__
