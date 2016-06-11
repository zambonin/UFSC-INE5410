#include <pthread.h>
#include <stdio.h>
#define MAX_THREADS 128
#define N 100

void* increment(void*);

int global_counter = 0;

int main(int argc, char **argv) {

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }


    if (MAX_THREADS * N != global_counter) {
        printf("Race condition! ");
    }
    printf("global_counter = %d.\n", global_counter);
    pthread_exit(NULL);

    return 0;

}

void* increment(void* arg) {

    for (int i = 0; i < N; i++) {
        global_counter++;
    }

    pthread_exit(NULL);

}
