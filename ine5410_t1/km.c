#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define RANDNUM_W 521288629
#define RANDNUM_Z 362436069

unsigned int randum_w = RANDNUM_W;
unsigned int randum_z = RANDNUM_Z;

typedef struct {
    int chunk_start, chunk_end;
} chunk_positions;

void srandnum(int seed) {
    unsigned int w, z;
    w = (seed * 104623) & 0xffffffff;
    randum_w = (w) ? w : RANDNUM_W;
    z = (seed * 48947) & 0xffffffff;
    randum_z = (z) ? z : RANDNUM_Z;
}

unsigned int randnum(void) {
    unsigned int u;
    randum_z = 36969 * (randum_z & 65535) + (randum_z >> 16);
    randum_w = 18000 * (randum_w & 65535) + (randum_w >> 16);
    u = (randum_z << 16) + randum_w;
    return (u);
}

typedef float* vector_t;

int npoints, dimension, ncentroids, seed, too_far, has_changed, nthreads;
float mindistance;
vector_t *data, *centroids;
int *map, *dirty;
pthread_t* threads;

float v_distance(vector_t a, vector_t b) {
    int i;
    float distance = 0;
    for (i = 0; i < dimension; i++)
        distance +=  pow(a[i] - b[i], 2);
    return sqrt(distance);
}

void* populate(void* arg) {
    int i, j;
    float tmp, distance;

    chunk_positions *cp = (chunk_positions *) arg;
    int cs = cp->chunk_start;
    int ce = cp->chunk_end;

    for (i = cs; i < ce; i++) {
        distance = v_distance(centroids[map[i]], data[i]);
        /* Look for closest cluster. */
        for (j = 0; j < ncentroids; j++) {
            /* Point is in this cluster. */
            if (j == map[i]) 
                continue;
            tmp = v_distance(centroids[j], data[i]);
            if (tmp < distance) {
                map[i] = j;
                distance = tmp;
                dirty[j] = 1;
            }
        }
        /* Cluster is too far away. */
        if (distance > mindistance)
            too_far = 1;
    }
    pthread_exit(NULL);
}

void* compute_centroids(void* arg) {
    int i, j, k, population;

    chunk_positions *cp = (chunk_positions *) arg;
    int cs = cp->chunk_start;
    int ce = cp->chunk_end;

    /* Compute means. */
    for (i = cs; i < ce; i++) {
        if (!dirty[i])
            continue;
        memset(centroids[i], 0, sizeof(float) * dimension);
        /* Compute cluster's mean. */
        population = 0;
        for (j = 0; j < npoints; j++) {
            if (map[j] != i)
                continue;
            for (k = 0; k < dimension; k++)
                centroids[i][k] += data[j][k];
            population++;
        }
        if (population > 1) {
            for (k = 0; k < dimension; k++)
                centroids[i][k] *= 1.0/population;
        }
        has_changed = 1;
    }
    pthread_exit(NULL);
}

chunk_positions* create_chunks(int nelements) {
    chunk_positions* chunks =
        (chunk_positions *) malloc(sizeof(chunk_positions) * (nthreads + 1));
    int chunk_size = nelements / nthreads;
    int last_chunk = nelements % nthreads;

    for (int i = 0; i < nthreads; i++) {
        chunks[i].chunk_start = (chunk_size * i);
        chunks[i].chunk_end = (chunk_size * (i+1));
    }

    if (last_chunk) {
        chunks[nthreads].chunk_start = (chunk_size * nthreads);
        chunks[nthreads].chunk_end = (chunk_size * nthreads) + last_chunk;
    }

    return chunks;
}

int* kmeans(void) {
    int i, j, k;
    too_far = 0;
    has_changed = 0;

    if (!(map  = calloc(npoints, sizeof(int))))
        exit (1);
    if (!(dirty = malloc(ncentroids * sizeof(int))))
        exit (1);
    if (!(centroids = malloc(ncentroids * sizeof(vector_t))))
        exit (1);
    if (!(threads = malloc((nthreads + 1) * sizeof(pthread_t))))
        exit (1);

    for (i = 0; i < ncentroids; i++)
        centroids[i] = malloc(sizeof(float) * dimension);
    for (i = 0; i < npoints; i++)
        map[i] = -1;
    for (i = 0; i < ncentroids; i++) {
        dirty[i] = 1;
        j = randnum() % npoints;
        for (k = 0; k < dimension; k++)
            centroids[i][k] = data[j][k];
        map[j] = i;
    }
    /* Map unmapped data points. */
    for (i = 0; i < npoints; i++)
        if (map[i] < 0)
            map[i] = randnum() % ncentroids;

if (nthreads > npoints || nthreads > ncentroids) {
    if (npoints > ncentroids) {
        nthreads = ncentroids;
    } else {
        nthreads = npoints;
    }
}

    chunk_positions* p_chunks = create_chunks(npoints);
    chunk_positions* c_chunks = create_chunks(ncentroids);

    /* This should be optimized to reuse threads. */
    do { /* Cluster data. */
        too_far = 0;
        for (i = 0; i <= nthreads && p_chunks[i].chunk_end; i++) {
            pthread_create(&threads[i], NULL, populate,
                    (void *) &p_chunks[i]);
        }
        for (i = 0; i <= nthreads && p_chunks[i].chunk_end; i++) {
            pthread_join(threads[i], NULL);
        }

        has_changed = 0;
        for (i = 0; i <= nthreads && c_chunks[i].chunk_end; i++) {
            pthread_create(&threads[i], NULL, compute_centroids,
                    (void *) &c_chunks[i]);
        }
        for (i = 0; i <= nthreads && c_chunks[i].chunk_end; i++) {
            pthread_join(threads[i], NULL);
        }
        memset(dirty, 0, ncentroids * sizeof(int));
    } while (too_far && has_changed);

    free(p_chunks);
    free(c_chunks);

    for (i = 0; i < ncentroids; i++)
        free(centroids[i]);
    free(centroids);
    free(dirty);
    free(threads);

    return map;
}

int main(int argc, char **argv) {
    int i, j;

    if (argc != 7) {
        printf("Usage: npoints dimension ncentroids mindistance seed\n");
        exit (1);
    }

    npoints = atoi(argv[1]);
    dimension = atoi(argv[2]);
    ncentroids = atoi(argv[3]);
    mindistance = atoi(argv[4]);
    seed = atoi(argv[5]);
    nthreads = atoi(argv[6]);

    srandnum(seed);

    if (!(data = malloc(npoints*sizeof(vector_t))))
        exit(1);

    for (i = 0; i < npoints; i++) {
        data[i] = malloc(sizeof(float) * dimension);
        for (j = 0; j < dimension; j++)
            data[i][j] = randnum() & 0xffff;
    }

    map = kmeans();

    for (i = 0; i < ncentroids; i++) {
        printf("Partition %d:\n", i);
        for (j = 0; j < npoints; j++)
            if (map[j] == i)
                printf("%d ", j);
        printf("\n");
    }

    free(map);
    for (i = 0; i < npoints; i++)
        free(data[i]);
    free(data);

    return (0);
}
