#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#define N_CHILDS 4

void n_childs(int);

int main(int argc, char **argv) {
    n_childs(N_CHILDS);

    return 0;
}

void n_childs(int n) {
    for (int i = 0; i < n; i++) {
        if (fork() == 0) { // create one and only one child
            break;         // process per iteration
        }
    }

    printf("Process %d created %d.\n", getppid(), getpid());
    while (wait(NULL) > 0) {
    }
}
