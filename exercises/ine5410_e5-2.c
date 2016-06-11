#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double monte_carlo_pi(unsigned int);

int main(int argc, char **argv) {

    int n_procs, rank;
    unsigned int n_points, chunk;
    double part_sum, sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 2) {
        printf("Usage: %s <number of points>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    n_points = atoi(argv[1]);

    if (rank == 0) {
        chunk = n_points - (n_procs - 1) * (n_points / n_procs);
    }
    chunk = n_points / n_procs;

    srand(time(NULL));
    part_sum = monte_carlo_pi(chunk);
    MPI_Reduce(&part_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("pi = %.040f\n", 4 * sum / (double) n_points);
    }

    MPI_Finalize();

    return 0;

}

double monte_carlo_pi(unsigned int n) {

    double x, p = 0.0, xmin = 0.0, xmax = 1.0;

    for (int i = 0; i < n; i++) {
        x = (double) rand() / (RAND_MAX * (xmax - xmin) + xmin);
        p += 1.0 / (1.0 + x * x);
    }

    return p;

}
