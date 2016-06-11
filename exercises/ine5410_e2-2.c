#include <pthread.h>
#include <stdio.h>
#define MAX_THREADS 5

void* print_thread_id(void*);

int main(int argc, char **argv) {

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, print_thread_id, NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_exit(NULL);

    return 0;

}

void* print_thread_id(void* arg) {

    pthread_t tid = pthread_self();

    printf("New thread created with ID %u!\n", (unsigned int) tid);
    pthread_exit(NULL);

}