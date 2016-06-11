#include <pthread.h>
#include <stdio.h>

void* print_thread_id(void*);

int main(int argc, char **argv) {

    pthread_t thread;

    pthread_create(&thread, NULL, print_thread_id, NULL);
    pthread_join(thread, NULL);
    pthread_exit(NULL);

    return 0;

}

void* print_thread_id(void* arg) {

    pthread_t tid = pthread_self();

    printf("New thread created with ID %u!\n", (unsigned int) tid);
    pthread_exit(NULL);

}
