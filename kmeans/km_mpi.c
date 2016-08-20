#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RANDNUM_W 521288629
#define RANDNUM_Z 362436069

unsigned int randum_w = RANDNUM_W;
unsigned int randum_z = RANDNUM_Z;

void srandnum(int seed) {
    unsigned int w = (seed * 104623) & 0xffffffff;
    randum_w = (w) ? w : RANDNUM_W;
    unsigned int z = (seed * 48947) & 0xffffffff;
    randum_z = (z) ? z : RANDNUM_Z;
}

unsigned int randnum(void) {
    randum_z = 36969 * (randum_z & 65535) + (randum_z >> 16);
    randum_w = 18000 * (randum_w & 65535) + (randum_w >> 16);
    unsigned int u = (randum_z << 16) + randum_w;
    return (u);
}

typedef float *vector_t;

int npoints, dimension, ncentroids, seed, too_far, has_changed;
float mindistance;
vector_t *data, *centroids, *g_centroids;
int *map, *dirty;
int *g_dirty, *g_map, g_too_far, g_population;
int rank, size, chunk_size, rmd, cs, ce;

float v_distance(vector_t a, vector_t b) {
    float distance = 0;

    for (int i = 0; i < dimension; i++) {
        distance += pow(a[i] - b[i], 2);
    }

    return sqrt(distance);
}

static void populate(void) {
    float tmp, distance;
    too_far = 0;

    for (int i = cs; i < ce; i++) {
        distance = v_distance(centroids[map[i]], data[i]);
        /* Look for closest cluster. */
        for (int j = 0; j < ncentroids; j++) {
            /* Point is in this cluster. */
            if (j == map[i]) {
                continue;
            }
            tmp = v_distance(centroids[j], data[i]);
            if (tmp < distance) {
                map[i] = j;
                distance = tmp;
                dirty[j] = 1;
            }
        }
        /* Cluster is too far away. */
        if (distance > mindistance) {
            too_far = 1;
        }
    }

    MPI_Allreduce(dirty, g_dirty, ncentroids, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    MPI_Allreduce(&too_far, &g_too_far, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Recv(g_map, npoints, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);

            int cs = chunk_size * i;
            int ce = cs + chunk_size - 1;

            if (i == size - 1) {
                ce += rmd;
            }

            for (int j = cs; j < ce; j++) {
                map[j] = g_map[j];
            }
        }
    } else {
        MPI_Send(map, npoints, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Bcast(map, npoints, MPI_INT, 0, MPI_COMM_WORLD);
}

static void compute_centroids(void) {
    int population;
    has_changed = 0;

    /* Compute means. */
    for (int i = 0; i < ncentroids; i++) {
        if (!g_dirty[i]) {
            continue;
        }
        memset(centroids[i], 0, sizeof(float) * dimension);
        /* Compute cluster's mean. */
        population = 0;
        for (int j = 0; j < npoints; j++) {
            if (map[j] != i) {
                continue;
            }
            for (int k = 0; k < dimension; k++) {
                centroids[i][k] += data[j][k];
            }
            population++;
        }

        MPI_Allreduce(&population, &g_population, 1, MPI_INT, MPI_SUM,
                      MPI_COMM_WORLD);
        MPI_Allreduce(centroids[i], g_centroids[i], dimension, MPI_FLOAT,
                      MPI_SUM, MPI_COMM_WORLD);

        for (int k = 0; k < dimension; k++) {
            centroids[i][k] = g_centroids[i][k];
        }

        if (g_population > 1) {
            for (int k = 0; k < dimension; k++) {
                centroids[i][k] *= 1.0 / g_population;
            }
        }
        has_changed = 1;
    }

    memset(dirty, 0, ncentroids * sizeof(int));
}

static void create_chunks() {
    chunk_size = npoints / size;
    rmd = npoints % size;

    cs = chunk_size * rank;
    ce = cs + chunk_size - 1;

    if (rank == size - 1) {
        ce += rmd;
    }
}

int *kmeans(void) {
    too_far = 0;
    has_changed = 0;

    if (!(map = calloc(npoints, sizeof(int)))) {
        exit(1);
    }
    if (!(dirty = malloc(ncentroids * sizeof(int)))) {
        exit(1);
    }
    if (!(centroids = malloc(ncentroids * sizeof(vector_t)))) {
        exit(1);
    }
    if (!(g_map = calloc(npoints, sizeof(int)))) {
        exit(1);
    }
    if (!(g_dirty = malloc(ncentroids * sizeof(int)))) {
        exit(1);
    }
    if (!(g_centroids = malloc(ncentroids * sizeof(vector_t)))) {
        exit(1);
    }
    for (int i = 0; i < ncentroids; i++) {
        centroids[i] = malloc(sizeof(float) * dimension);
        g_centroids[i] = malloc(sizeof(float) * dimension);
    }

    for (int i = 0; i < npoints; i++) {
        map[i] = -1;
    }
    for (int i = 0; i < ncentroids; i++) {
        dirty[i] = 1;
        int j = randnum() % npoints;
        for (int k = 0; k < dimension; k++) {
            centroids[i][k] = data[j][k];
        }
        map[j] = i;
    }
    /* Map unmapped data points. */
    for (int i = 0; i < npoints; i++) {
        if (map[i] < 0) {
            map[i] = randnum() % ncentroids;
        }
    }

    create_chunks();

    do { /* Cluster data. */
        populate();
        MPI_Barrier(MPI_COMM_WORLD);

        compute_centroids();
        MPI_Barrier(MPI_COMM_WORLD);
    } while (g_too_far && has_changed);

    for (int i = 0; i < ncentroids; i++) {
        free(centroids[i]);
        free(g_centroids[i]);
    }

    free(centroids);
    free(g_centroids);
    free(dirty);
    free(g_dirty);
    free(g_map);

    return map;
}

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("Usage: npoints dimension ncentroids mindistance seed\n");
        exit(1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    npoints = atoi(argv[1]);
    dimension = atoi(argv[2]);
    ncentroids = atoi(argv[3]);
    mindistance = atoi(argv[4]);
    seed = atoi(argv[5]);

    srandnum(seed);

    if (!(data = malloc(npoints * sizeof(vector_t)))) {
        exit(1);
    }

    for (int i = 0; i < npoints; i++) {
        data[i] = malloc(sizeof(float) * dimension);
        for (int j = 0; j < dimension; j++) {
            data[i][j] = randnum() & 0xffff;
        }
    }

    map = kmeans();

    if (rank == 0) {
        for (int i = 0; i < ncentroids; i++) {
            printf("Partition %d:\n", i);
            for (int j = 0; j < npoints; j++) {
                if (map[j] == i) {
                    printf("%d ", j);
                }
            }
            printf("\n");
        }
    }

    free(map);
    for (int i = 0; i < npoints; i++) {
        free(data[i]);
    }
    free(data);

    MPI_Finalize();

    return (0);
}