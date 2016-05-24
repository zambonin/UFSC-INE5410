#include <pthread.h>
#include <stdio.h>

void* printThreadID(void*);

int main(int argc, char **argv) {

    pthread_t thread;

    pthread_create(&thread, NULL, printThreadID, NULL);
    pthread_join(thread, NULL);
    pthread_exit(NULL);

    return 0;

}

void* printThreadID(void* arg) {

    pthread_t tid = pthread_self();

    printf("Nova thread criada com ID %u!\n", (unsigned int) tid);
    pthread_exit(NULL);

}
