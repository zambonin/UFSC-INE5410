#include <pthread.h>
#include <stdio.h>

static void *print_thread_id();

int main() {
  pthread_t thread;

  pthread_create(&thread, NULL, print_thread_id, NULL);
  pthread_join(thread, NULL);
  pthread_exit(NULL);
}

void *print_thread_id() {
  pthread_t tid = pthread_self();

  printf("New thread created with ID %u!\n", (unsigned int) tid);
  pthread_exit(NULL);
}
