#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_HEIGHT 2

void binary_proc_tree(int /*height*/);

int main() {
  binary_proc_tree(MAX_HEIGHT);

  return 0;
}

void binary_proc_tree(int height) {
  for (int i = 0; i < height; i++) {
    // NOLINTNEXTLINE(misc-redundant-expression)
    if (fork() != 0 && fork() != 0) { // if both nodes are parents then
      break;                          // extra nodes cannot be created
    }                                 // at that level
  }

  printf("Process %d created %d.\n", getppid(), getpid());
  while (wait(NULL) > 0) {
  }
}
