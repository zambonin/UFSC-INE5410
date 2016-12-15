#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void first_fork(void);

int main() {
  first_fork();

  return 0;
}

void first_fork() {
  fork();
  printf("A new process was created!\n");
  wait(NULL);
}
