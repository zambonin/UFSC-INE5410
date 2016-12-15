#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int size, rank;
  char *msg = "test";
  MPI_Status st;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Bcast(&msg, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      MPI_Recv(&msg, 2, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
      printf("Received \"%s\" from %d!\n", msg, st.MPI_SOURCE);
    }
  } else {
    MPI_Send(&msg, 2, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
