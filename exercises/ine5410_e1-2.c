#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void n_childs(int);

int main(int argc, char** argv) {

    n_childs(4);

    return 0;

}

void n_childs(int n) {

    for (int i = 0; i < n; i++) {
        if (fork() == 0)    // create one and only one children per iteration
            break;
    }

    printf("Processo pai %d criou %d.\n", getppid(), getpid());
    while (wait(NULL) > 0) {}

}
