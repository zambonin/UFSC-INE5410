CC = gcc
CFLAGS = -Wall -Wextra -march=native -O3 -pipe -g
LIBS = -lm -lpthread

K_ARGS = 128 2 8 2.0 987

default: all compare

all:
	@$(CC) $(CFLAGS) $(LIBS) km_seq.c -o km_seq
	@$(CC) $(CFLAGS) $(LIBS) km_pth.c -o km_pth
	@mpicc $(CFLAGS) -lm km_mpi.c -o km_mpi

compare: SHELL := /bin/bash
compare:
	@diff <(./km_seq $(K_ARGS)) <(./km_pth $(K_ARGS) $(shell nproc))
	@diff <(./km_seq $(K_ARGS)) <(mpirun -np $(shell nproc) ./km_mpi $(K_ARGS))

clean:
	@rm km_{seq,pth,mpi}
