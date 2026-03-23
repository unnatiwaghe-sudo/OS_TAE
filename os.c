#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t intersection_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_cross = PTHREAD_COND_INITIALIZER;
int is_busy = 0;

void* car_process(void* arg) {
    int id = *((int*)arg);
    const char* dirs[] = {"NORTH", "EAST", "SOUTH", "WEST"};
    const char* my_dir = dirs[rand() % 4];

    // STEP 1: Arrival
    printf("[TIME: %d] Car %d arrived from %s lane.\n", (int)time(NULL) % 100, id, my_dir);
    
    pthread_mutex_lock(&intersection_lock);
    
    // STEP 2: Checking Resource (Waiting)
    while (is_busy) {
        printf("  -- Car %d (%s) is STOPPED. Waiting for intersection to clear...\n", id, my_dir);
        pthread_cond_wait(&can_cross, &intersection_lock);
    }
    
    // STEP 3: Occupying Resource (Crossing)
    is_busy = 1;
    printf("  >>> [ACTION] Car %d (%s) is now CROSSING the intersection.\n", id, my_dir);
    pthread_mutex_unlock(&intersection_lock);

    sleep(2); // Simulated driving time

    // STEP 4: Releasing Resource (Leaving)
    pthread_mutex_lock(&intersection_lock);
    is_busy = 0;
    printf("  <<< [EXIT] Car %d (%s) has cleared the intersection safely.\n", id, my_dir);
    
    pthread_cond_signal(&can_cross); 
    pthread_mutex_unlock(&intersection_lock);

    free(arg);
    return NULL;
}

int main() {
    pthread_t cars[5];
    setvbuf(stdout, NULL, _IONBF, 0); // Ensures messages show up instantly
    srand(time(NULL));

    printf("--- STARTING 4-WAY INTERSECTION SIMULATION ---\n\n");

    for (int i = 0; i < 5; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&cars[i], NULL, car_process, id);
        usleep(400000); // Cars arrive every 0.4 seconds to create a queue
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(cars[i], NULL);
    }

    printf("\n--- ALL CARS HAVE CROSSED. NO DEADLOCK DETECTED ---\n");
    return 0;
}