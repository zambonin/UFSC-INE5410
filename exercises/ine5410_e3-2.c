#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX_THREADS 8
#define MAX_PRODUCTIONS 64

static int produce();
void consume(int * /*a*/);
static void *producer();
static void *consumer();

static int buffer[MAX_THREADS];
static int i = 0, f = 0, max_prod = 0, max_cons = 0;
static sem_t full, empty, lock_prod, lock_cons;

int main() {
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, MAX_THREADS);
  sem_init(&lock_prod, 0, 1);
  sem_init(&lock_cons, 0, 1);

  pthread_t threads[MAX_THREADS];

  for (int j = 0; j < MAX_THREADS / 2; j++) {
    pthread_create(&threads[j], NULL, producer, NULL);
  }

  for (int j = MAX_THREADS / 2; j < MAX_THREADS; j++) {
    pthread_create(&threads[j], NULL, consumer, NULL);
  }

  for (int j = 0; j < MAX_THREADS; j++) {
    pthread_join(threads[j], NULL);
  }

  sem_destroy(&full);
  sem_destroy(&empty);
  sem_destroy(&lock_prod);
  sem_destroy(&lock_cons);

  return 0;
}

int produce() {
  printf("P%d ", f + 1);
  max_prod++;
  return f + 1;
}

void consume(int *a) {
  printf("C%d ", *a);
  max_cons++;
  *a = 0;

  printf("|");
  for (int j = 0; j < MAX_THREADS; j++) { // prints buffer status
    printf("%d|", buffer[j]);
  }
  printf("\n");
}

void *producer() {
  int c = 0;
  while (++c != MAX_PRODUCTIONS) {
    sem_wait(&empty);
    sem_wait(&lock_prod);
    f = (f + 1) % MAX_THREADS;
    buffer[f] = produce();
    sem_post(&lock_prod);
    sem_post(&full);
  }
  return 0;
}

void *consumer() {
  int c = 0;
  while (++c != MAX_PRODUCTIONS) {
    sem_wait(&full);
    sem_wait(&lock_cons);
    i = (i + 1) % MAX_THREADS;
    consume(&buffer[i]);
    sem_post(&lock_cons);
    sem_post(&empty);
  }
  return 0;
}
