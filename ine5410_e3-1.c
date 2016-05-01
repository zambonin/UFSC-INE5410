#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define MAX_THREADS 100

void* func_thread();

int global_counter = 0;
sem_t sem;

int main(int argc, char **argv) {

    sem_init(&sem, 0, 1);
    int i;
    pthread_t threads[MAX_THREADS];

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, func_thread, NULL);
    }

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main thread: global_counter = %d.\n", global_counter);
    sem_destroy(&sem);

    return 0;

}

void* func_thread() {

    int i;
    pthread_t tid = pthread_self();

    sem_wait(&sem);
    for (i = 0; i < 100; i++) {
        global_counter++;
    }
    printf("Worker thread %ld: global_counter = %d.\n", tid, global_counter);
    sem_post(&sem);

    return 0;

}
