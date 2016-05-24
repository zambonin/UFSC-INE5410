#include <pthread.h>
#include <stdio.h>
#define MAX_THREADS 5

void* printThreadID(void*);

int main(int argc, char **argv) {

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, printThreadID, NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_exit(NULL);

    return 0;

}

void* printThreadID(void* arg) {

    pthread_t tid = pthread_self();

    printf("Nova thread criada com ID %u!\n", (unsigned int) tid);
    pthread_exit(NULL);

}