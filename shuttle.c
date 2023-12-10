#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
#include <unistd.h>
#include <time.h>

#define NUM_SEATS 30
#define MAX_ATTENDEES 100

typedef struct {
    int id;
} Attendee;

Zem_t* seats;
Zem_t seat_mutex;
int num_attendees = 0;
int attendees_left = MAX_ATTENDEES;

void rsleep() {
    srand((unsigned int)time(NULL));
    int num = rand() % 3;
    printf("Sleeping for %d seconds...\n", num);
    sleep(num);
}

int grabSeat() {
    int seat = -1;
    Zem_wait(&seat_mutex);

    for (int i = 0; i < NUM_SEATS; ++i) {
        if (seats[i].value > 0) {
            seats[i].value = 0;
            seat = i;
            printf("Attendee has taken seat %d\n", seat);
            break;
        }
    }

    Zem_post(&seat_mutex);
    return seat;
}

void *shuttle_algorithm(void *arg) {
    //Attendee *attendee = (Attendee *)arg;

    int seat = -1;
    while (1) {
        seat = grabSeat();
        if (seat == -1){
            printf("Attendee could not find a vacant seat, waiting for an opening...\n");
            rsleep();
            continue;
        }
        printf("Attendee is sitting in seat %d\n", seat);
        break; // Break the loop after grabbing a seat
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_attendees>\n", argv[0]);
        return 1;
    }
    num_attendees = atoi(argv[1]);
    attendees_left = atoi(argv[1]);
    if (num_attendees < 3 || num_attendees > MAX_ATTENDEES) {
        printf("Number of attendees must be between 3 and %d\n", MAX_ATTENDEES);
        return 1;
    }

    pthread_t threads[NUM_SEATS];
    seats = (Zem_t*)malloc(NUM_SEATS * sizeof(Zem_t));

    Zem_init(&seat_mutex, 1);

    for (int i = 0; i < NUM_SEATS; i++) {
        Zem_init(&seats[i], 1);
        seats[i].value = 1;
    }

    while (attendees_left > 0) {
        int attendees_in_shuttle = (attendees_left >= NUM_SEATS) ? NUM_SEATS : attendees_left;

        Attendee attendees[attendees_in_shuttle];

        for (int i = 0; i < attendees_in_shuttle; i++) {
            attendees[i].id = i; // Set attendee IDs starting from 0
            pthread_create(&threads[i], NULL, shuttle_algorithm, (void *)&attendees[i]);
        }

        for (int i = 0; i < attendees_in_shuttle; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("Shuttle departing with %d passengers\n", attendees_in_shuttle);

        attendees_left -= attendees_in_shuttle; // Decrement the remaining attendees

        for (int i = 0; i < NUM_SEATS; i++) {
            seats[i].value = 1; // Reset the seats for the next shuttle
        }

        rsleep(); // Simulate shuttle travel time
    }

    free(seats);

    return 0;
}
