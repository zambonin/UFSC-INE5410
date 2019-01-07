#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RANDNUM_W 521288629
#define RANDNUM_Z 362436069

typedef float *vector_t;

static unsigned int randum_w = RANDNUM_W;
static unsigned int randum_z = RANDNUM_Z;

static int npoints, dimension, ncentroids, seed, too_far, has_changed;
static float mindistance;
static vector_t *data, *centroids;
static int *map, *dirty;

void srandnum(int /*_seed*/);
unsigned int randnum(void);
float v_distance(vector_t /*a*/, vector_t /*b*/);
void populate(void);
void compute_centroids(void);
int *kmeans(void);

int main(int argc, char **argv) {
  if (argc != 6) {
    printf("Usage: npoints dimension ncentroids mindistance seed\n");
    exit(1);
  }

  npoints = strtol(argv[1], NULL, 0);
  dimension = strtol(argv[2], NULL, 0);
  ncentroids = strtol(argv[3], NULL, 0);
  mindistance = strtol(argv[4], NULL, 0);
  seed = strtol(argv[5], NULL, 0);

  srandnum(seed);

  data = calloc((unsigned int)npoints, sizeof(vector_t));
  for (int i = 0; i < npoints; i++) {
    data[i] = calloc((unsigned int)dimension, sizeof(float));
    for (int j = 0; j < dimension; j++) {
      data[i][j] = randnum() & 65535U;
    }
  }

  map = kmeans();

  for (int i = 0; i < ncentroids; i++) {
    printf("Partition %d:\n", i);
    for (int j = 0; j < npoints; j++) {
      if (map[j] == i) {
        printf("%d ", j);
      }
    }
    printf("\n");
  }

  free(map);
  for (int i = 0; i < npoints; i++) {
    free(data[i]);
  }
  free(data);

  return 0;
}

void srandnum(int _seed) {
  unsigned int w = ((unsigned int)_seed * 104623) & 0xffffffff;
  randum_w = (w) ? w : RANDNUM_W;
  unsigned int z = ((unsigned int)_seed * 48947) & 0xffffffff;
  randum_z = (z) ? z : RANDNUM_Z;
}

unsigned int randnum(void) {
  randum_z = 36969U * (randum_z & 65535U) + (randum_z >> 16U);
  randum_w = 18000U * (randum_w & 65535U) + (randum_w >> 16U);
  unsigned int u = (randum_z << 16U) + randum_w;
  return (u);
}

float v_distance(vector_t a, vector_t b) {
  float distance = 0;

  for (int i = 0; i < dimension; i++) {
    distance += powf(a[i] - b[i], 2);
  }

  return sqrtf(distance);
}

void populate(void) {
  float tmp, distance;
  too_far = 0;

  for (int i = 0; i < npoints; i++) {
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
}

void compute_centroids(void) {
  int population;
  has_changed = 0;

  /* Compute means. */
  for (int i = 0; i < ncentroids; i++) {
    if (!dirty[i]) {
      continue;
    }
    memset(centroids[i], 0, sizeof(float) * (unsigned int)dimension);
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
    if (population > 1) {
      for (int k = 0; k < dimension; k++) {
        centroids[i][k] *= 1.0 / population;
      }
    }
    has_changed = 1;
  }
  memset(dirty, 0, sizeof(int) * (unsigned int)ncentroids);
}

int *kmeans(void) {
  too_far = 0;
  has_changed = 0;

  map = calloc((unsigned int)npoints, sizeof(int));
  dirty = calloc((unsigned int)ncentroids, sizeof(int));
  centroids = calloc((unsigned int)ncentroids, sizeof(vector_t));

  for (int i = 0; i < ncentroids; i++) {
    centroids[i] = calloc((unsigned int)dimension, sizeof(float));
  }
  for (int i = 0; i < npoints; i++) {
    map[i] = -1;
  }
  for (int i = 0; i < ncentroids; i++) {
    dirty[i] = 1;
    int j = (int)(randnum() % (unsigned int)npoints);
    for (int k = 0; k < dimension; k++) {
      centroids[i][k] = data[j][k];
    }
    map[j] = i;
  }

  /* Map unmapped data points. */
  for (int i = 0; i < npoints; i++) {
    if (map[i] < 0) {
      map[i] = (int)(randnum() % (unsigned int)ncentroids);
    }
  }

  /* Cluster data. */
  do {
    populate();
    compute_centroids();
  } while (too_far && has_changed);

  for (int i = 0; i < ncentroids; i++) {
    free(centroids[i]);
  }
  free(centroids);
  free(dirty);

  return map;
}
