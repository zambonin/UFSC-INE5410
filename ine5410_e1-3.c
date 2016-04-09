#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void binary_proc_tree(int);

int main(int argc, char** argv) {

    binary_proc_tree(2);

    return 0;

}

void binary_proc_tree(int height) {

    for (int i = 0; i < height; i++) {
        if (fork() != 0 && fork() != 0) // if both nodes are parents then extra
            break;                      // nodes can't be created at that level
    }

    printf("Processo pai %d criou %d.\n", getppid(), getpid());
    while (wait(NULL) > 0) {}

}
