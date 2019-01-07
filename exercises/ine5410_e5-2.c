#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double monte_carlo_pi(int /*n*/);

int main(int argc, char **argv) {
  int n_procs, rank, n_points, chunk;
  double part_sum, sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 2) {
    printf("Usage: %s <number of points>\n", argv[0]);
    MPI_Finalize();
    return 1;
  }

  n_points = strtol(argv[1], NULL, 0);

  if (rank == 0) {
    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    chunk = n_points - (n_procs - 1) * (n_points / n_procs);
  }
  chunk = n_points / n_procs;

  struct timespec ts;
  if (timespec_get(&ts, TIME_UTC) == 0) {
    printf("Error getting time!");
    MPI_Finalize();
    return 1;
  }

  srand(ts.tv_nsec ^ ts.tv_sec); // NOLINT(hicpp-signed-bitwise)
  part_sum = monte_carlo_pi(chunk);
  MPI_Reduce(&part_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("pi = %.040f\n", 4 * sum / n_points);
  }

  MPI_Finalize();

  return 0;
}

double monte_carlo_pi(int n) {
  double x, p = 0.0, xmin = 0.0, xmax = 1.0;

  for (int i = 0; i < n; i++) {
    x = random() / (RAND_MAX * (xmax - xmin) + xmin);
    p += 1.0 / (1.0 + x * x);
  }

  return p;
}
