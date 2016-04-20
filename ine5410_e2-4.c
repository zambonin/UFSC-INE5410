#include <pthread.h>
#include <stdio.h>
#define MAX_THREADS 128

void* increment(void*);

int global_counter = 0;
pthread_mutex_t mutex;

int main(int argc, char **argv) {

    pthread_t threads[MAX_THREADS];

    pthread_mutex_init(&mutex,  NULL);
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%d\n", global_counter);
    pthread_mutex_destroy(&mutex);

    return 0;

}

void* increment(void* arg) {

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 100; i++) {
        global_counter++;
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);

}
