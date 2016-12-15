CC = gcc
CFLAGS = -Wall -Wextra -march=native -O3 -pipe -g -lm
N = $(shell nproc)

K_ARGS = 128 2 8 2.0 987

all: seq pth mpi compare

seq: $(basename km_seq.c)

pth: CFLAGS += -lpthread
pth: $(basename km_pth.c)

mpi: CC = mpicc
mpi: $(basename km_mpi.c)

compare: SHELL := /bin/bash
compare:
	@diff -q <(./km_seq $(K_ARGS)) <(./km_pth $(K_ARGS) $(N))
	@diff -q <(./km_seq $(K_ARGS)) <(mpirun -np $(N) ./km_mpi $(K_ARGS))

clean:
	@rm km_{seq,pth,mpi}
