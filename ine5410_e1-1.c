#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

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
