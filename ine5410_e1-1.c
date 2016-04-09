#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void first_fork();

int main(int argc, char** argv) {

    first_fork();

    return 0;

}

void first_fork() {

    fork();
    printf("Novo processo criado!\n");
    wait(NULL);

}
