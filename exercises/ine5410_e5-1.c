#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int size, rank;
  char msg[5] = "test";

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Bcast(msg, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

  printf("Received \"%s\" from %d!\n", msg, rank);
  MPI_Finalize();

  return 0;
}
