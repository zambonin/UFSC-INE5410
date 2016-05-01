#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define MAX_THREADS 100

int produce();
void consume(int*);
void* producer();
void* consumer();

int buffer[MAX_THREADS];
int i = 0, f = 0, max_prod = 0, max_cons = 0;
sem_t full, empty, lock_prod, lock_cons;

int main(int argc, char **argv) {

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX_THREADS);
    sem_init(&lock_prod, 0, 1);
    sem_init(&lock_cons, 0, 1);

    int j;
    pthread_t threads[MAX_THREADS];

    for (j = 0; j < MAX_THREADS/2; j++) {
        pthread_create(&threads[j], NULL, producer, NULL);
    }

    for (j = MAX_THREADS/2; j < MAX_THREADS; j++) {
        pthread_create(&threads[j], NULL, consumer, NULL);
    }

    for (j = 0; j < MAX_THREADS; j++) {
        pthread_join(threads[j], NULL);
    }

    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&lock_prod);
    sem_destroy(&lock_cons);

    return 0;

}

int produce() {

    printf("Produzido %d\n", f);
    max_prod++;
    return f;

}

void consume(int* a) {

    printf("Consumido %d\n", *a);
    max_cons++;
    a = 0;

}

void* producer() {

    while (1) {
        sem_wait(&empty);
        sem_wait(&lock_prod);
        f = (f + 1) % MAX_THREADS;
        buffer[f] = produce();
        sem_post(&lock_prod);
        sem_post(&full);
    }

}

void* consumer() {

    while (1) {
        sem_wait(&full);
        sem_wait(&lock_cons);
        i = (i + 1) % MAX_THREADS;
        consume(&buffer[i]);
        sem_post(&lock_cons);
        sem_post(&empty);
    }

}
